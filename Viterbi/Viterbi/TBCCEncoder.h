#pragma once
class TBCCEncoder
{
public:
	TBCCEncoder(const unsigned int nConstraintLen, const unsigned int nRate, const int *Polynomial);
	~TBCCEncoder();

	// For LTE, bSeparate = true
	unsigned int update(const bool *bInput, const unsigned int nInputLen, const bool bSeparate);

	bool* m_bOutput;
	unsigned int m_nOutputLen;

	void PrintNextState();
	void PrintEncodedOutput();

private:
	void GenNextState();
	
	unsigned int m_nConstraintLen;
	unsigned int m_nNumStates;
	unsigned int m_nNumPolynomial;
	int* m_nPolynomial; 
	unsigned int ** m_nNextState;
	bool *** m_bEncodedOutput;
};

