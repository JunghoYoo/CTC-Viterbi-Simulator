#pragma once

class CRCGenerator
{
public:
	CRCGenerator();
	~CRCGenerator();

	unsigned int update(const bool* bInput, const unsigned int nInputLen, bool* bCRC);

private:
	const unsigned int nCRCLENGTH = 16;
	const unsigned int nQCRCLENGTH = 17;
	// CRC-16-CCITT 0x1021(Forward)
	const bool bPOLYNOMIAL[17] = { 1, 0,0,0,1, 0,0,0,0, 0,0,1,0, 0,0,0,1 };
};
