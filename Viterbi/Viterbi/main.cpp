#include "tbccencoder.h"
#include "tbccdecoder.h"
#include "CRCGenerator.h"
#include "const.h"

#include <stdlib.h>
#include <iostream>
#include <complex>
using namespace std;

float Randn(void) 
{
	float v1, v2, w, z1;
	static float z2 = 0.0;

	if (z2 != 0.0) {
		z1 = z2;
		z2 = 0.0;
	}
	else {
		do {
			v1 = (float)(2.0 * ((float)rand() / (float)RAND_MAX) - 1.0);
			v2 = (float)(2.0 * ((float)rand() / (float)RAND_MAX) - 1.0);
			w = v1 * v1 + v2 * v2;
		} while (w >= 1.0);

		w = (float)sqrt((-2.0 * log(w)) / w);
		z1 = v1 * w;
		z2 = v2 * w;
	}
	return (z1);
}

void AWGN(std::complex<float>* in, unsigned long size, std::complex<float>* out, float fSNRdB)
{
	std::complex<float> fNoise;
	float fNoisePwr;

	fNoisePwr = 1 / ((float)pow((double)10, (0.1f * (fSNRdB))));

	for (unsigned int index = 0; index < size; index++)
	{
		fNoise.real(Randn() * fNoisePwr);
		fNoise.imag(Randn() * fNoisePwr);

		out[index] = in[index] + fNoise;
	}
}

void randbit(bool* out, unsigned int len)
{
	for (unsigned i = 0; i < len; i++)
	{
		int randnumber = rand(); // Generate random number [0 ~ RAND_MAX]

		if (randnumber > RAND_MAX / 2)
			out[i] = true;
		else
			out[i] = false;
	}
}

void randi(unsigned int* out, unsigned int len, unsigned int max)
{
	int* upper;

	upper = new int[max];

	for (unsigned i = 0; i < max; i++)
	{
		upper[i] = (i + 1) * (RAND_MAX / (max + 1));
	}

	for (unsigned i = 0; i < len; i++)
	{
		int randnumber = rand();

		for (unsigned k = 0; k < max; k++)
		{
			if (randnumber >= upper[k]) {
				out[i] = k;
				break;
			}
		}
	}

	delete[] upper;
}
 
void modulate(bool* in, unsigned int len, std::complex<float>* out, unsigned int modulation)
{
	if (modulation == 2)
	{
		for (unsigned i = 0; i < len; i++)
		{
			out[i].imag(0.0f);

			if (in[i])
				out[i].real(1.0f);
			else
				out[i].real(-1.0f);
		}
	}
	else if (modulation == 4)
	{
		for (unsigned i = 0, k = 0; i < len; i = i + 2,k++)
		{
			if ((!in[i]) && (!in[i+1])) {
				out[k].real(1.0f / sqrt(2.0f));
				out[k].imag(1.0f / sqrt(2.0f));
			}
			else if ((!in[i]) && (in[i + 1])) {
				out[k].real(1.0f / sqrt(2.0f));
				out[k].imag(-1.0f / sqrt(2.0f));
			}
			else if ((in[i]) && (!in[i + 1])) {
				out[k].real(-1.0f / sqrt(2.0f));
				out[k].imag(1.0f / sqrt(2.0f));
			}
			else {
				out[k].real(-1.0f / sqrt(2.0f));
				out[k].imag(-1.0f / sqrt(2.0f));
			}
		}
	}
}

void demodulate(std::complex<float>* in, unsigned int len, float* out, unsigned int modulation)
{
	if (modulation == 2)
	{
		for (unsigned i = 0; i < len; i++)
		{
			out[i] = in[i].real();
		}
	}
	else if (modulation == 4)
	{
		for (unsigned i = 0, k = 0; i < len; i++, k = k + 2)
		{
			out[k]		= -in[i].real() * sqrt(2.0f);
			out[k+1]	= -in[i].imag() * sqrt(2.0f);
		}
	}
}

int bertest(void)
{
	const unsigned long iteration = 10000;
	const unsigned int modulation = 4; // 2 for bpsk 4 for qpsk
	const unsigned int messagebits = 40;
	const unsigned int CRCbits = 16;
	const unsigned int coderates = 3;
	const unsigned int encodedbits = messagebits * coderates;
	const unsigned int signallength = encodedbits / (modulation / 2);

	const float SNRTable[] = { -3, -2.5, -2, -1.5, -1, -0.5, 0, 0.5, 1, 1.5, 2, 2.5, 3, 3.5, 4, 4.5, 5 };
	const unsigned int SNRdB_length = 17;
	float SNRdB;

	unsigned long biterr = 0;
	unsigned long stateerr = 0;

	bool message[messagebits];
	std::complex<float> txsignal[signallength];
	std::complex<float> rxsignal[signallength];
	float llr[encodedbits];

	TBCCEncoder Encoder(TBCC_CONSTRAINT_LEN, TBCC_CODE_RATE, TBCC_G);
	TBCCDecoder Decoder(TBCC_CONSTRAINT_LEN, TBCC_CODE_RATE, TBCC_G);

	for (unsigned int k = 0; k < SNRdB_length; k++)
	{
		biterr = 0;
		stateerr = 0;

		for (unsigned int i = 0; i < iteration; i++)
		{
			SNRdB = SNRTable[k];
			// generate message
			randbit(message, messagebits);
			// TBCC
			int inistate = Encoder.update(message, messagebits, true);
			// modulation
			modulate(Encoder.m_bOutput, Encoder.m_nOutputLen, txsignal, modulation);
			
#if 0
			for (unsigned i = 0; i < signallength; i++)
			{
				rxsignal[i] = txsignal[i];
			}
#else
			// awgn
			AWGN(txsignal, signallength, rxsignal, SNRdB);
#endif
			
			// demodulation
			demodulate(rxsignal, signallength, llr, modulation);
			// TBCC
			int eststate = Decoder.update(llr, Encoder.m_nOutputLen, true);
			
			for (unsigned int i = 0; i < Decoder.m_nOutputLen; i++)
				if (message[i] != Decoder.m_bOutput[i])
					biterr++;

			if (inistate != eststate)
				stateerr++;
		}
		cout << SNRdB << "dB," << (float)biterr/ (float)(iteration * messagebits) << "," << (float)stateerr / (float)(iteration) << endl;
	}
	return 0;
}

int simpletest(void)
{
	const unsigned int messagebits = 40;
	const unsigned int CRCbits = 16;
	const unsigned int coderates = 3;
	const unsigned int encodedbits = messagebits * coderates;

	bool message[messagebits] = {
			1, 0, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0,
			0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1,
			1, 1, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1,
			0, 0, 1, 0
	};

	float codeword[encodedbits];
	float received[encodedbits];
	unsigned int state;

	cout << "message : " << endl;
	for (unsigned int i = 0; i < messagebits; i++)
		cout << message[i];
	cout << endl;
	
	TBCCEncoder Encoder(TBCC_CONSTRAINT_LEN, TBCC_CODE_RATE, TBCC_G);

	//Encoder.PrintNextState();
	//Encoder.PrintEncodedOutput();

	state = Encoder.update(message, messagebits, true);

	cout << "Initial State : " << state << endl;

	for (unsigned int i = 0; i < Encoder.m_nOutputLen; i++)
	{
		codeword[i] = Encoder.m_bOutput[i];
	}

	for (unsigned int i = 0; i < Encoder.m_nOutputLen; i++)
	{
		if (codeword[i])
			received[i] = +1.0;
		else
			received[i] = -1.0;
	}

	TBCCDecoder Decoder(TBCC_CONSTRAINT_LEN, TBCC_CODE_RATE, TBCC_G);

	/*
	Decoder.PrintNextState();
	Decoder.PrintTableNextState();
	Decoder.PrintDecodedInput();
	Decoder.PrintTableDecodedInput();
	*/

 	state = Decoder.update(received, encodedbits, true);
	
	cout << "Estimated Initial State : " << state << endl;

	cout << "decoded message : " << endl;

	for (unsigned int i = 0; i < Decoder.m_nOutputLen; i++)
		cout << Decoder.m_bOutput[i];
	cout << endl;

	for (unsigned int i = 0; i < Decoder.m_nOutputLen; i++)
		if (message[i] == Decoder.m_bOutput[i])
			cout << ".";
		else
			cout << "X";

	cout << endl;

	bool bCRCResult[16];
	CRCGenerator crcchecker;
	crcchecker.update(Decoder.m_bOutput, messagebits - CRCbits, bCRCResult);
		
	cout << "CRC Check : ";

	if (bCRCResult)
		cout << "Pass" << endl;
	else
		cout << "Fail" << endl;

	for (unsigned int i = 0; i < CRCbits; i++)
		cout << bCRCResult[i];
	cout << endl;

	for (unsigned int i = 0; i < CRCbits; i++)
		cout << message[messagebits - CRCbits +i];
	cout << endl;

	for (unsigned int i = 0; i < CRCbits; i++)
		if (bCRCResult[i] == message[messagebits - CRCbits +i])
			cout << ".";
		else
			cout << "X";
	cout << endl;
	
	return 0;
}

int main(void)
{
	simpletest();
	bertest();

	return 0;
}
