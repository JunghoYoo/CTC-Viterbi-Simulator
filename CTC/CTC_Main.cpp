#include "DataRandomizer.h"
#include "FECEncoder.h"
#include "Modulator.h"
#include "FECDecoder.h"

#include <iostream>
using namespace std;

int main(void)
{
	unsigned int length;
	unsigned int biterr=0;

	BIT datain[4800];
	BIT dataout[4800];
	FECEncoder FECEncoder;
	DataRandomizer DataRandomizer(&FECEncoder);
	Modulator Modulator;
	FECDecoder FECDecoder;

	FECEncoder.m_MCS 	= WIMAX::QPSK_1_2;
	FECEncoder.m_HARQ 	= WIMAX::NO;
	FECEncoder.m_BurstSize 		= 6;  // byte
	FECEncoder.m_Ns 			= 1;  // number of allocated slot
	FECEncoder.m_SPID			= 0;
	FECEncoder.m_SubpacketIndex	= 0;
	FECEncoder.m_RepititionFactor	= 1;
	FECEncoder.m_STCLayerNumber	= 1;
	FECEncoder.m_STCRate		= 1;

	FECDecoder.m_Iteration = 2;

	FECDecoder.m_eMCS 	= FECEncoder.m_MCS;
	FECDecoder.m_eHARQ 	= FECEncoder.m_HARQ;
	FECDecoder.m_BBurstSize 		= FECEncoder.m_BurstSize;  // byte
	FECDecoder.m_nNs 			= FECEncoder.m_Ns;  // number of allocated slot
	FECDecoder.m_nSPID			= FECEncoder.m_SPID;
	FECDecoder.m_nSubpacketIndex	= FECEncoder.m_SubpacketIndex;
	FECDecoder.m_nRepititionFactor	= FECEncoder.m_RepititionFactor;
	FECDecoder.m_nSTCLayerNumber	= FECEncoder.m_STCLayerNumber;
	FECDecoder.m_nSTCRate		= FECEncoder.m_STCRate;

	length = FECEncoder.m_BurstSize * 8; // bit

	unsigned int i;

//	cout << "source" << endl;

	for(i=0;i<length;i++)
	{
/*		if(i%2 == 0)
			datain[i] = 1;
		else
*/			datain[i] = 0;
		FECEncoder.m_datain[i] = datain[i];
//		cout << (unsigned int)datain[i] ;
	}
//	cout << endl;

	// Burst
	FECEncoder.SlotConcatenate();
	DataRandomizer.Padding();
	// FECBlock
	DataRandomizer.DataRandomize();

	cout << "padding % randomizer" << endl;
	for(i=0;i<FECEncoder.m_PaddedBurstSize*8;i++)
	{
		cout << (unsigned int)FECEncoder.m_datain[i] ;
	}
	cout << endl;

	FECEncoder.CTCEncoding();

	length = FECEncoder.m_EncodedBurstSize * 8;
/*
	cout << "encoding " << endl;
	for(i=0;i<length;i++)
	{
		dataout[i] = FECEncoder.m_dataout[i];
		cout << (unsigned int)dataout[i] ;
	}
	cout << endl;
/*
	Modulator.m_BITSize = FECEncoder.m_EncodedBurstSize;
	for(unsigned int i=0;i<Modulator.m_BITSize;i++)
	{
		Modulator.m_datain[i] = FECEncoder.m_dataout[i];
	}

	Modulator.Modulate(FECEncoder.m_MCS);
*/
	// Soft Decision
	for(unsigned int i=0;i<length;i++)
	{
		if(FECEncoder.m_dataout[i])
			FECDecoder.m_Bdatain[i] = (BYTE)0x80; // 1
		else
			FECDecoder.m_Bdatain[i] = (BYTE)0x7F; // 0
	}
/*
	cout << "encoding " << endl;
	for(i=0;i<length;i++)
	{
		if(FECDecoder.m_Bdatain[i] == 0x80)
			cout << "1";
		else
			cout << "0";
	}
*/
	FECDecoder.CTCDecoding();

	length = FECEncoder.m_BurstSize * 8; // bit
	cout << "decoding " << endl;
	for(i=0;i<length;i++)
	{
		cout << (unsigned int)FECDecoder.m_bdataout[i] ;

		if(FECEncoder.m_datain[i] != FECDecoder.m_bdataout[i])
			biterr++;
	}

	cout << endl;
	cout << "Total/Err = " << length << "/" << biterr;

	return 0;	
}

