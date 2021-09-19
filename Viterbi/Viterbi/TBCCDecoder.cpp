#include "TBCCDecoder.h"
#include "const.h"
#include <iostream>
using namespace std;

#ifndef NULL
#define NULL 0
#endif

TBCCDecoder::TBCCDecoder(const unsigned int nConstraintLen, const unsigned int nRate, const int* nPolynomial)
{
	m_nConstraintLen = nConstraintLen;
	m_nNumStates = 1 << (nConstraintLen - 1);
	m_nNumPolynomial = nRate;

	m_nPolynomial = new int[m_nNumPolynomial];

	// Octal -> Binary for all polynomials
	for (unsigned i = 0; i < m_nNumPolynomial; i++)
	{
		int nDigitPolynomial = nPolynomial[i];
		int nshift = 1;
		m_nPolynomial[i] = 0;

		while (nDigitPolynomial) {
			int nOct = 0;
			int nPolydigit = (nDigitPolynomial % 10);

			m_nPolynomial[i] |= nPolydigit * nshift;

			nshift = nshift * 8;
			nDigitPolynomial = nDigitPolynomial / 10;
		}
	}
	
	m_nNextState = new unsigned int* [m_nNumStates];

	for (unsigned i = 0; i < m_nNumStates; i++)
	{
		m_nNextState[i] = new unsigned int[2];
	}
	
	m_fEncodedOutput = new float** [m_nNumStates];

	for (unsigned i = 0; i < m_nNumStates; i++)
	{
		m_fEncodedOutput[i] = new float* [2];

		for (unsigned k = 0; k < 2; k++)
		{
			m_fEncodedOutput[i][k] = new float[m_nNumPolynomial];
		}
	}
	
	m_bOutput = NULL;

	GenNextState();
}

TBCCDecoder::~TBCCDecoder()
{
	delete[] m_nPolynomial;

	for (unsigned i = 0; i < m_nNumStates; i++)
	{
		delete m_nNextState[i];
	}

	delete[] m_nNextState;

	for (unsigned i = 0; i < m_nNumStates; i++)
	{
		for (unsigned k = 0; k < 2; k++)
		{
			delete m_fEncodedOutput[i][k];
		}
		delete[] m_fEncodedOutput[i];
	}

	delete[] m_fEncodedOutput;

	if (m_bOutput != NULL)
		delete[] m_bOutput;
}

void TBCCDecoder::GenNextState()
{
	for (unsigned nstate = 0; nstate < m_nNumStates; nstate++)
	{
		for (unsigned b = 0; b < 2; b++) // input bit 0 or 1
		{
			m_nNextState[nstate][b] = (nstate >> 1) | (b << (m_nConstraintLen - 2)); // Input bit @ higher bit

			for (unsigned int t = 0; t < m_nNumPolynomial; t++)
			{
				int nxorout = 0;
				int npolynomial = m_nPolynomial[t];
				int nstatereg = (nstate) | (b << (m_nConstraintLen - 1));

				for (unsigned int k = 0; k < m_nConstraintLen; npolynomial >>= 1, nstatereg >>= 1, k++)
				{
					nxorout ^= (nstatereg & 1) & (npolynomial & 1);
				}

				m_fEncodedOutput[nstate][b][t] = (nxorout != 0) ? +1.0f : -1.0f; // BPSK
			}
		}
	}
}

void TBCCDecoder::PrintNextState()
{
	cout << "-0- -1-" << endl;
	for (unsigned nstate = 0; nstate < m_nNumStates; nstate++)
	{
		for (unsigned b = 0; b < 2; b++) // input bit 0 or 1
		{
			cout << m_nNextState[nstate][b] << " ";
		}
		cout << endl;
	}
}

void TBCCDecoder::PrintTableNextState()
{
	cout << "{";
	for (unsigned nstate = 0; nstate < m_nNumStates; nstate++)
	{
		cout << "{";
		cout << m_nNextState[nstate][0] << ", ";

		if (nstate != m_nNumStates-1)
			cout << m_nNextState[nstate][1] << "}, ";
		else
			cout << m_nNextState[nstate][1] << "} ";

		if (nstate % 5 == 4)
			cout << endl;

	}
	cout << "};" << endl;
}

void TBCCDecoder::PrintDecodedInput()
{
	cout << "-0- -1-" << endl;
	for (unsigned nstate = 0; nstate < m_nNumStates; nstate++)
	{
		for (unsigned b = 0; b < 2; b++) // input bit 0 or 1
		{
			for (unsigned int t = 0; t < m_nNumPolynomial; t++)
			{
				cout << m_fEncodedOutput[nstate][b][t];
			}
			cout << " ";
		}
		cout << endl;
	}
}

void TBCCDecoder::PrintTableDecodedInput()
{
	cout << "{ ";
	for (unsigned nstate = 0; nstate < m_nNumStates; nstate++)
	{
		cout << "{";
		cout << "{";
		cout << m_fEncodedOutput[nstate][0][0] << ", ";
		cout << m_fEncodedOutput[nstate][0][1] << ", ";
		cout << m_fEncodedOutput[nstate][0][2] << "},";
		cout << "{";
		cout << m_fEncodedOutput[nstate][1][0] << ", ";
		cout << m_fEncodedOutput[nstate][1][1] << ", ";
		cout << m_fEncodedOutput[nstate][1][2] << "}";
		//	cout << " ";

		if (nstate != m_nNumStates - 1) {
			cout << "},";
			cout << endl;
		}
		else
			cout << "}";
	}
	cout << "};" << endl;
}

void TBCCDecoder::AddCompareSelect(float* fReceivedVector, unsigned int* nDecidedPrevState)
{
	unsigned int nprevstate[2];
	
	// Get Branch Metric (Add)
	for (unsigned nnextstate = 0; nnextstate < m_nNumStates; nnextstate++)
	{
		bool binput = (nnextstate >> (m_nConstraintLen - 2)) ? true : false;

		for (unsigned p = 0; p < 2; p++) // 2 incoming path
		{
			nprevstate[p] = ((nnextstate << 1) & (m_nNumStates - 1)) | p;

			// Branch Metric @ a state
			m_fBranchMetric[nnextstate][p] = GetEuclideanDistance(m_fEncodedOutput[nprevstate[p]][binput], fReceivedVector);
			//cout << m_fBranchMetric[nnextstate][p] << endl;

			// Add Path Metric & Branch Metric (nprevstate[p] -> nnextstate)
			m_fNewPathMetric[nnextstate][p] = m_fPathMetric[nprevstate[p]] + m_fBranchMetric[nnextstate][p];
		}
		//cout << nnextstate << endl;
		//cout << m_fPathMetric[nnextstate] << endl;
		//cout << m_fBranchMetric[nnextstate][0] << " " << m_fBranchMetric[nnextstate][1] << endl;
		//cout << m_fNewPathMetric[nnextstate][0] << " " << m_fNewPathMetric[nnextstate][1] << endl;
	}
	
	for (unsigned int nstate = 0; nstate < m_nNumStates; nstate++)
	{
		//cout << nstate << ":" << m_fBranchMetric[nstate][0] << " " << m_fBranchMetric[nstate][1] << endl;
		//cout << nstate << ":" << m_fNewPathMetric[nstate][0] << " " << m_fNewPathMetric[nstate][1] << endl;
		//cout << nstate << ":" << m_fPathMetric[nstate] << endl;
	}

	// Update Path Metric (Compare & Select)
	for (unsigned nnextstate = 0; nnextstate < m_nNumStates; nnextstate++)
	{
		// Compare for two incoming path
		if (m_fNewPathMetric[nnextstate][0] < m_fNewPathMetric[nnextstate][1])
		{
			m_fPathMetric[nnextstate] = m_fNewPathMetric[nnextstate][0]; // Select 0 path
			nDecidedPrevState[nnextstate] = ((nnextstate << 1) & (m_nNumStates - 1)) | 0;
		} else {
			m_fPathMetric[nnextstate] = m_fNewPathMetric[nnextstate][1]; // Select 1 path
			nDecidedPrevState[nnextstate] = ((nnextstate << 1) & (m_nNumStates - 1)) | 1;
		}
	}
}

float TBCCDecoder::GetEuclideanDistance(float* a, float* b)
{
	float c = 0.0f;

	for (unsigned int t = 0; t < m_nNumPolynomial; t++)
	{
		// fabs()
		if (a[t] > b[t])
			c += a[t] - b[t];
		else
			c += b[t] - a[t];
	}

	return c;
}

unsigned int TBCCDecoder::update(const float* fInput, const unsigned int nInputLen, const bool bSeparate)
{
	unsigned int ninitialstate = 0;
	unsigned int ntbstate = 0;
	float fminpathmetric = std::numeric_limits<float>::max();
	unsigned int nindex = 0;
	float* freceivedvector;

	m_nOutputLen = nInputLen / m_nNumPolynomial;

	m_fPathMetric = new float [m_nNumStates];
	m_fNewPathMetric = new float* [m_nNumStates];
	m_fBranchMetric = new float* [m_nNumStates];
	freceivedvector = new float [m_nNumPolynomial];
	m_DecidedPrevState = new unsigned int* [m_nOutputLen];

	for (unsigned int nstate = 0; nstate < m_nNumStates; nstate++)
	{
		m_fNewPathMetric[nstate] = new float[2];
		m_fBranchMetric[nstate] = new float[2];
	}

	for (unsigned int i = 0; i < m_nOutputLen; i++)
	{
		m_DecidedPrevState[i] = new unsigned int[m_nNumStates];
	}

	for (unsigned int nstate = 0; nstate < m_nNumStates; nstate++)
	{
		m_fPathMetric[nstate] = 0.0f; // equal state to estimate initial state 
	}

	// Decdoing with equal path metrics to Estimate Intial State 
	for (unsigned int t = 0; t < TBCC_ITERATIONFORINITIALSTATE; t++)
	{
		for (unsigned int i = 0; i < m_nOutputLen; i++)
		{
			for (unsigned int t = 0; t < m_nNumPolynomial; t++)
			{
				if (bSeparate) // out = [D0 D1 D2] where D0,D1, and D2 are the separate vectors resulting from encoding the input input with the individual octal polynomials G0=133, G1=171, and G2=165.
					nindex = i + m_nOutputLen * t; // g0 ..... g1 ....... g2 ......
				else
					nindex = m_nNumPolynomial * i + t; // g0 g1 g2 g0 g1 g2 g0 g1 g2 ..

				freceivedvector[t] = fInput[nindex];
			}

			AddCompareSelect(freceivedvector, m_DecidedPrevState[i]);
		}
	}
	
	// Estimate Intial State from path metric
	for (unsigned int nstate = 0; nstate < m_nNumStates; nstate++)
	{
		if (fminpathmetric > m_fPathMetric[nstate])
		{
			fminpathmetric = m_fPathMetric[nstate];
			ninitialstate = nstate;
		}
	}

	// Re-initialize path metric
	for (unsigned int nstate = 0; nstate < m_nNumStates; nstate++)
	{
		m_fPathMetric[nstate] = std::numeric_limits<float>::max() / 2; // penalty but +/- margin to prohibit from overflow
	}

	// Give priority for estimated state
	m_fPathMetric[ninitialstate] = 0.0f;

	//cout << "Initial State :" << ninitialstate << endl;

	// Re-Decdoing with equal path metrics to decode bits
	for (unsigned int i = 0; i < m_nOutputLen; i++)
	{
		for (unsigned int t = 0; t < m_nNumPolynomial; t++)
		{
			if (bSeparate) // out = [D0 D1 D2] where D0,D1, and D2 are the separate vectors resulting from encoding the input input with the individual octal polynomials G0=133, G1=171, and G2=165.
				nindex = i + m_nOutputLen * t; // g0 ..... g1 ....... g2 ......
			else
				nindex = m_nNumPolynomial * i + t; // g0 g1 g2 g0 g1 g2 g0 g1 g2 ..

			freceivedvector[t] = fInput[nindex];
		}

/*		cout << i << endl;

		for (unsigned int nstate = 0; nstate < m_nNumStates; nstate++)
		{
			cout << nstate << "-----" << m_fPathMetric[nstate] << endl;
		}
		cout << endl;
		*/
		AddCompareSelect(freceivedvector, m_DecidedPrevState[i]);
	}

	// Select state to trace back
	fminpathmetric = std::numeric_limits<float>::max();
	for (unsigned int nstate = 0; nstate < m_nNumStates; nstate++)
	{
		//cout << nstate << "-----" << m_fPathMetric[nstate] << endl;

		if (fminpathmetric > m_fPathMetric[nstate])
		{
			fminpathmetric = m_fPathMetric[nstate];
			ntbstate = nstate;
		}
	}

	//cout << "Traceback State :" << ntbstate << endl;

	// delete previously allocated output
	if (m_bOutput != NULL)
		delete[] m_bOutput;

	m_bOutput = new bool[m_nOutputLen];

	// Trace back
	for (int i = m_nOutputLen - 1; i >= 0; i--)
	{
		//cout << i << "--" << ntbstate << endl;

		m_bOutput[i] = (ntbstate >> (m_nConstraintLen - 2)) ? true : false;
		ntbstate = m_DecidedPrevState[i][ntbstate];
	}

	for (unsigned int nstate = 0; nstate < m_nNumStates; nstate++)
	{
		delete m_fNewPathMetric[nstate];
		delete m_fBranchMetric[nstate];
	}
	delete[] m_fNewPathMetric;
	delete[] m_fBranchMetric;

	for (unsigned int i = 0; i < m_nOutputLen; i++)
	{
		delete m_DecidedPrevState[i];
	}
	delete[] m_DecidedPrevState;

	delete[] m_fPathMetric;
	delete[] freceivedvector;

	return ninitialstate;
}
