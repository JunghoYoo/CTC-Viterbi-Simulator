#pragma once
class TBCCDecoder
{
public:
	TBCCDecoder(const unsigned int nConstraintLen, const unsigned int nRate, const int* Polynomial);
	~TBCCDecoder();

	// For LTE, bSeparate = true
	unsigned int update(const float* fInput, const unsigned int nInputLen, const bool bSeparate);

	bool* m_bOutput;
	unsigned int m_nOutputLen;

	void PrintNextState();
	void PrintDecodedInput();

	void PrintTableNextState();
	void PrintTableDecodedInput();

private:
	void GenNextState();
	void AddCompareSelect(float* fReceivedVector, unsigned int* nDecidedPrevState);
	float GetEuclideanDistance(float* a, float* b);

	unsigned int m_nConstraintLen;
	unsigned int m_nNumStates;
	unsigned int m_nNumPolynomial;
	int* m_nPolynomial;
	unsigned int** m_nNextState;
	float*** m_fEncodedOutput;
	float** m_fBranchMetric;
	float* m_fPathMetric;
	float** m_fNewPathMetric;
	unsigned int** m_DecidedPrevState;


//	unsigned int m_nPrevState[64][2];
};

