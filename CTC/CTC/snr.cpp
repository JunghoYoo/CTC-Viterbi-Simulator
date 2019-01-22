/*
 * snr.cpp
 *
 *  Created on: 2013. 1. 14.
 *      Author: Peter
 */




#include "DataRandomizer.h"
#include "FECEncoder.h"
#include "Modulator.h"
#include "Demodulator.h"
#include "FECDecoder.h"
#include "AWGN.h"
#include "Fixed.h"

#include <iostream>
using namespace std;


#define ITERATION 100000
#define SNR 10
#define MCS WIMAX::QPSK_1_2

int snr_uncoded(void);

int main(void)
{
//	snr_uncoded();


	unsigned int length;
	unsigned int biterr=0;

	BIT datain[4800];
//	BIT dataout[4800];
	FECEncoder FECEncoder;
	DataRandomizer DataRandomizer(&FECEncoder);
	Modulator Modulator;
	AWGN AWGN;
	Demodulator Demodulator;
	FECDecoder FECDecoder;


	FECEncoder.m_MCS 			= MCS;
	FECEncoder.m_HARQ 			= WIMAX::NO;
	FECEncoder.m_BurstSize 		= 6;  // byte
	FECEncoder.m_Ns 			= 1;  // number of allocated slot

	FECEncoder.m_SPID			= 0;
	FECEncoder.m_SubpacketIndex	= 0;
	FECEncoder.m_RepititionFactor	= 1;
	FECEncoder.m_STCLayerNumber	= 1;
	FECEncoder.m_STCRate		= 1;

	FECDecoder.m_Iteration 		= 4;

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

	unsigned int i;

	unsigned int k;
	unsigned int iteration_snr;

	unsigned long biterrorcount;
	biterrorcount = 0;

	iteration_snr = ITERATION;

	cout << "CTC-Iteration " <<	(unsigned int)FECDecoder.m_Iteration << endl;

	for(AWGN.m_SNRdB=0;AWGN.m_SNRdB<SNR;AWGN.m_SNRdB=AWGN.m_SNRdB+0.5)
	{

		biterrorcount = 0;

		for(k=0;k<iteration_snr;k++)
		{
			//	cout << "source" << endl;

			for(i=0;i<length;i++)
			{
				if(i%2 == 0)
					datain[i] = 1;
				else
					datain[i] = 0;

				FECEncoder.m_datain[i] = datain[i];
		//		cout << (unsigned int)datain[i] ;
			}
		//	cout << endl;

			// Burst
			FECEncoder.SlotConcatenate();
			DataRandomizer.Padding();
			// FECBlock
			DataRandomizer.DataRandomize();

			FECEncoder.CTCEncoding();

			length = FECEncoder.m_EncodedBurstSize * 8;

			Modulator.m_BITSize = FECEncoder.m_EncodedBurstSize * 8;
			for(i=0;i<Modulator.m_BITSize;i++)
			{
				Modulator.m_datain[i] = FECEncoder.m_dataout[i];
			}

			Modulator.Modulate(FECEncoder.m_MCS);

			// Channel
			AWGN.Add(Modulator.m_dataout, Modulator.m_dataout, Modulator.m_SIGNALSize);

			// Soft Decision
			Demodulator.m_SIGNALSize = Modulator.m_SIGNALSize;
			for(i=0;i<Demodulator.m_SIGNALSize;i++)
			{
				Demodulator.m_datain[i] = Modulator.m_dataout[i];
			}

			Demodulator.Demodulate(FECEncoder.m_MCS);

			for(i=0;i<Demodulator.m_LLRSize;i++)
			{
				FECDecoder.m_Bdatain[i] = Demodulator.m_dataout[i];
			}

			FECDecoder.CTCDecoding();

			length = FECEncoder.m_BurstSize * 8; // bit

			biterr = 0;
			for(i=0;i<length;i++)
			{
				if(FECEncoder.m_datain[i] != FECDecoder.m_bdataout[i])
				{
					biterr++;
				}
			}

			biterrorcount += biterr;
		}
		cout << AWGN.m_SNRdB << " " ;
		cout << (((float)biterrorcount)/((float)iteration_snr * length)) << endl;
	}
}


int snr_uncoded(void)
{
	BIT datain[4800];
	BIT dataout[4800];
	Modulator Modulator;
	AWGN AWGN;
	Demodulator Demodulator;

	AWGN.m_SNRdB = SNR;


	unsigned int length=48;
	unsigned int biterr=0;
	unsigned int i;

	unsigned int k;
	unsigned int iteration_snr;

	unsigned long biterrorcount;
	biterrorcount = 0;

	iteration_snr = ITERATION;

	cout << "QPSK uncoded" << endl;

	for(AWGN.m_SNRdB=0;AWGN.m_SNRdB<SNR;AWGN.m_SNRdB=AWGN.m_SNRdB+0.5)
	{

		biterrorcount = 0;

		for(k=0;k<iteration_snr;k++)
		{
			for(i=0;i<length;i++)
			{
				if(i%2 == 0)
					datain[i] = 1;
				else
					datain[i] = 0;
			}

			Modulator.m_BITSize = length;
			for(unsigned int i=0;i<Modulator.m_BITSize;i++)
			{
				Modulator.m_datain[i] = datain[i];
			}

			Modulator.Modulate(MCS); // Same MCS

			// Channel
			AWGN.Add(Modulator.m_dataout, Demodulator.m_datain, Modulator.m_SIGNALSize);

			// Soft Decision
			Demodulator.m_SIGNALSize = Modulator.m_SIGNALSize;
	/*		for(unsigned int i=0;i<Demodulator.m_SIGNALSize;i++)
			{
				Demodulator.m_datain[i] = Modulator.m_dataout[i];
			}
	*/
			Demodulator.Demodulate(MCS);

			biterr = 0;
			for(i=0;i<(length/2);i++)
			{
/*
  				if((Demodulator.m_datain[i].real > 0) && (datain[2*i+1] != 0))
					biterr++;
				else if ((Demodulator.m_datain[i].real <= 0) && (datain[2*i+1] != 1))
					biterr++;
  				if((Demodulator.m_datain[i].imag > 0) && (datain[2*i] != 0))
					biterr++;
				else if ((Demodulator.m_datain[i].imag <= 0) && (datain[2*i] != 1))
					biterr++;
*/
  				if((Demodulator.m_dataout[i] > 0) && (datain[i] != 1))
					biterr++;
				else if ((Demodulator.m_dataout[i] <= 0) && (datain[i] != 0))
					biterr++;
			}

			biterrorcount += biterr;
		//	cout << endl;
		//	cout << "Total/Err = " << length << "/" << biterr;

		}

//		cout << biterrorcount << "/" << (iteration_snr * length) << " = ";
		cout << AWGN.m_SNRdB << " " ;
		cout << (((float)biterrorcount)/((float)iteration_snr * length)) << endl;
	}
	return 0;
}
