#include "CRCGenerator.h"
#include <string.h>


CRCGenerator::CRCGenerator()
{
}

CRCGenerator::~CRCGenerator()
{
}

unsigned int CRCGenerator::update(const bool* bInput, const unsigned int nInputLen, bool* bCRC)
{
	const unsigned int nInputNCRCLen = nInputLen + nCRCLENGTH;
	bool bRemainder[17];

	memset(bRemainder, 0, sizeof(bool) * nQCRCLENGTH);

	// Payload
	for (unsigned int i= 0; i < nInputLen; i++) {
		// Shift down
		for (unsigned int p = 0; p < nCRCLENGTH; p++) {
			bRemainder[p] = bRemainder[p + 1];
		}

		// Copy Dividend
		bRemainder[nCRCLENGTH] = bInput[i];
		
		// Add Polynomial if dividable
		if (bRemainder[0]) { 
			for (unsigned int p = 0; p < nQCRCLENGTH; p++) {
				bRemainder[p] = bRemainder[p] ^ bPOLYNOMIAL[p];
			}
		}
	}

	// Last 16bit @ Payload
	for (unsigned int i = 0; i < nCRCLENGTH; i++) {
	// Shift down
		for (unsigned int p = 0; p < nCRCLENGTH; p++) {
			bRemainder[p] = bRemainder[p + 1];
		}

		// No left Dividend
		bRemainder[nCRCLENGTH] = 0;

		// Add Polynomial if dividable
		if (bRemainder[0]) {
			for (unsigned int p = 0; p < nQCRCLENGTH; p++) {
				bRemainder[p] = bRemainder[p] ^ bPOLYNOMIAL[p];
			}
		}
	}

	// Copy Ramainder as generated CRC
	memcpy(bCRC, &bRemainder[1], sizeof(bool) * nCRCLENGTH);

	return nCRCLENGTH;
}
