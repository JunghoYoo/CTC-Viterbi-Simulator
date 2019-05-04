#include "DataRandomizer.h"
#include "FECEncoder.h"
#include "Modulator.h"
#include "Demodulator.h"
#include "FECDecoder.h"
#include "AWGN.h"
#include "Fixed.h"

#include <iostream>
#include <time.h>
using namespace std;

#define DEBUG_PRINT

int main(void)
{
	unsigned int length;
	unsigned int biterr;
	unsigned int i;
	unsigned int k;
	unsigned long minerr = 100;
	unsigned long miniteration = 10000;// 00;
	unsigned long maxiteration = 100000;// 00;
	unsigned long trial;

	BIT datain[4800];
	BIT dataout[4800];
	FECEncoder FECEncoder;
	DataRandomizer DataRandomizer(&FECEncoder);
	Modulator Modulator;
	AWGN AWGN;
	Demodulator Demodulator;
	FECDecoder FECDecoder;

	float SNRdB[] = {-3, -2.5, -2, -1.5, -1, -0.5, 0, 0.5, 1, 1.5, 2, 2.5, 3 };
	unsigned int SNRdB_length = 13;

	FECEncoder.m_MCS 			= WIMAX::QPSK_1_2;
	FECEncoder.m_HARQ 			= WIMAX::NO;
	FECEncoder.m_BurstSize 		= 60;  // byte
	FECEncoder.m_Ns 			= 10;  // number of allocated slot

	FECEncoder.m_SPID			= 0;
	FECEncoder.m_SubpacketIndex	= 0;
	FECEncoder.m_RepititionFactor	= 1;
	FECEncoder.m_STCLayerNumber	= 1;
	FECEncoder.m_STCRate		= 1;

	FECDecoder.m_Iteration 		= 8;

	FECDecoder.m_eMCS 				= FECEncoder.m_MCS;
	FECDecoder.m_eHARQ 				= FECEncoder.m_HARQ;
	FECDecoder.m_BBurstSize 		= FECEncoder.m_BurstSize;  // byte
	FECDecoder.m_nNs 				= FECEncoder.m_Ns;  // number of allocated slot
	FECDecoder.m_nSPID				= FECEncoder.m_SPID;
	FECDecoder.m_nSubpacketIndex	= FECEncoder.m_SubpacketIndex;
	FECDecoder.m_nRepititionFactor	= FECEncoder.m_RepititionFactor;
	FECDecoder.m_nSTCLayerNumber	= FECEncoder.m_STCLayerNumber;
	FECDecoder.m_nSTCRate			= FECEncoder.m_STCRate;

	length = FECEncoder.m_BurstSize * 8; // bit

	cout << "Burst size : " << FECEncoder.m_BurstSize << endl;

	cout << "SNR,    BER, Trials,  biterr";
	cout << endl;

	srand((unsigned int)time((unsigned int)0));

	for (k = 0; k < SNRdB_length; k++)
	{ 
		AWGN.m_SNRdB = SNRdB[k];

		biterr = 0;
		trial = 0;

		do
		{

#ifdef DEBUG_PRINT
			cout << "source" << endl;
#endif

			for (i = 0; i < length; i++)
			{
				if (rand() > RAND_MAX / 2)
					datain[i] = 1;
				else
					datain[i] = 0;
				FECEncoder.m_datain[i] = datain[i];
#ifdef DEBUG_PRINT
				cout << (unsigned int)datain[i];
#endif
			}

#ifdef DEBUG_PRINT
			cout << endl;
#endif

			// Burst
			FECEncoder.SlotConcatenate();
			DataRandomizer.Padding();
			// FECBlock
			DataRandomizer.DataRandomize();

#ifdef DEBUG_PRINT
			cout << "padding" << endl;
			for (i = 0; i < FECEncoder.m_PaddedBurstSize * 8; i++)
			{
				cout << (unsigned int)FECEncoder.m_datain[i];
			}
			cout << endl;

			cout << "symbol" << endl;
			for (i = 0; i < FECEncoder.m_PaddedBurstSize * 8; i = i + 2)
			{

				cout << (2 * FECEncoder.m_datain[i] + FECEncoder.m_datain[i + 1]);
			}
			cout << endl;
#endif

			FECEncoder.CTCEncoding();

			length = FECEncoder.m_EncodedBurstSize * 8;

#ifdef DEBUG_PRINT
			cout << "encoding " << endl;
			for (i = 0; i < length; i++)
			{
				dataout[i] = FECEncoder.m_dataout[i];
				cout << (unsigned int)dataout[i];
			}
			cout << endl;
#endif

			Modulator.m_BITSize = FECEncoder.m_EncodedBurstSize * 8;
			for (unsigned int i = 0; i < Modulator.m_BITSize; i++)
			{
				Modulator.m_datain[i] = FECEncoder.m_dataout[i];
			}

			Modulator.Modulate(FECEncoder.m_MCS);


#ifdef DEBUG_PRINT
			cout << "modulate " << endl;
			for (i = 0; i < Modulator.m_SIGNALSize; i++)
			{
				cout << Modulator.m_dataout[i].real << "+j" << Modulator.m_dataout[i].imag << endl;
			}
			cout << endl;
#endif

			// Channel
			AWGN.Add(Modulator.m_dataout, Modulator.m_dataout, Modulator.m_SIGNALSize);

#ifdef DEBUG_PRINT
			cout << "awgn " << endl;
			for (i = 0; i < Modulator.m_SIGNALSize; i++)
			{
				cout << Modulator.m_dataout[i].real << "+j" << Modulator.m_dataout[i].imag << endl;
			}
			cout << endl;
#endif

			// Soft Decision
			Demodulator.m_SIGNALSize = Modulator.m_SIGNALSize;
			for (unsigned int i = 0; i < Demodulator.m_SIGNALSize; i++)
			{
				Demodulator.m_datain[i] = Modulator.m_dataout[i];
			}

			Demodulator.Demodulate(FECEncoder.m_MCS);

#ifdef DEBUG_PRINT
			cout << "llr " << endl;
			for (i = 0; i < Demodulator.m_LLRSize; i++)
			{
				cout << (int)Demodulator.m_dataout[i] << endl;
			}
			cout << endl;
#endif


			for (unsigned int i = 0; i < Demodulator.m_LLRSize; i++)
			{
				FECDecoder.m_Bdatain[i] = Demodulator.m_dataout[i];
			}

			FECDecoder.CTCDecoding();

			length = FECEncoder.m_BurstSize * 8; // bit

			for (i = 0; i < length; i++)
			{
#ifdef DEBUG_PRINT
				cout << (unsigned int)FECDecoder.m_bdataout[i];
#endif

				if (FECEncoder.m_datain[i] != FECDecoder.m_bdataout[i])
				{
					biterr++;
#ifdef DEBUG_PRINT
					cout << "[" << i << "]";
#endif
				}
			}
			trial++;
#ifdef DEBUG_PRINT
			cout << endl;
#endif
		}while ( !((trial >= miniteration) && (biterr >= minerr)) 
			&& (trial < maxiteration) );

		cout << AWGN.m_SNRdB << ", ";
		cout << (float)biterr / ((float)length*trial) << ", ";
		cout << trial << ", ";
		cout << (float)biterr << ", ";
		cout << endl;
	}

	return 0;	
}
