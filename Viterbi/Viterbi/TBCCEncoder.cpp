#include "TBCCEncoder.h"
#include <iostream>
using namespace std;

#ifndef NULL
#define NULL 0
#endif

TBCCEncoder::TBCCEncoder(const unsigned int nConstraintLen, const unsigned int nRate, const int *nPolynomial)
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
	
	m_bEncodedOutput = new bool**[m_nNumStates];

	for (unsigned i = 0; i < m_nNumStates; i++)
	{
		m_bEncodedOutput[i] = new bool*[2];

		for (unsigned k = 0; k < 2; k++)
		{
			m_bEncodedOutput[i][k] = new bool[m_nNumPolynomial];
		}
	}
	
	m_bOutput = NULL;

	GenNextState();
}

TBCCEncoder::~TBCCEncoder()
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
			delete m_bEncodedOutput[i][k];
		}
		delete[] m_bEncodedOutput[i];
	}

	delete[] m_bEncodedOutput;

	if (m_bOutput != NULL)
		delete[] m_bOutput;
}
void TBCCEncoder::GenNextState()
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

				m_bEncodedOutput[nstate][b][t] = (nxorout != 0) ? true : false;
			}
		}
	}
}
void TBCCEncoder::PrintNextState()
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
void TBCCEncoder::PrintEncodedOutput()
{
	cout << "-0- -1-" << endl;
	for (unsigned nstate = 0; nstate < m_nNumStates; nstate++)
	{
		for (unsigned b = 0; b < 2; b++) // input bit 0 or 1
		{
			for (unsigned int t = 0; t < m_nNumPolynomial; t++)
			{
				cout << m_bEncodedOutput[nstate][b][t];
			}
			cout << " ";
		}
		cout << endl;
	}
}
unsigned int TBCCEncoder::update(const bool *bInput, const unsigned int nInputLen, const bool bSeparate)
{
	unsigned int ninitialstate = 0;
	unsigned int nstate;
	unsigned int nindex = 0;

	m_nOutputLen = nInputLen * m_nNumPolynomial;

	// Encoding Intial State from a few bits at the end of message bits
	for (unsigned int i = nInputLen - m_nConstraintLen + 1; i < nInputLen; i++)
	{
		ninitialstate = (ninitialstate >> 1) | (bInput[i] << (m_nConstraintLen - 2));
	}

	// delete previously allocated output
	if (m_bOutput != NULL)
		delete[] m_bOutput;

	m_bOutput = new bool[m_nOutputLen];

	// Set Initial State (= Final State)
	nstate = ninitialstate;

//	cout << "0-" << nstate << endl;

	for (unsigned int i = 0; i < nInputLen; i++)
	{
		//cout << i << "-";
		for (unsigned int t = 0; t < m_nNumPolynomial; t++)
		{	
			if (bSeparate) // out = [D0 D1 D2] where D0,D1, and D2 are the separate vectors resulting from encoding the input input with the individual octal polynomials G0=133, G1=171, and G2=165.
				nindex = i + nInputLen * t; // g0 ..... g1 ....... g2 ......
			else
				nindex = m_nNumPolynomial * i + t; // g0 g1 g2 g0 g1 g2 g0 g1 g2 ..

			m_bOutput[nindex] = m_bEncodedOutput[nstate][(bool)bInput[i]][t]; 
			//cout << m_bOutput[nindex];
		}

		//cout << endl;

		// Get Next State
		nstate = m_nNextState[nstate][(unsigned)bInput[i]];
		
//		cout << i << "-" << bInput[i] << endl;
//		cout << i << "-" << nstate << endl;
	}

	return ninitialstate;
}
