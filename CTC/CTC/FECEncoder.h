/*
 * FECEncoder.h
 *
 *  Created on: 2012. 4. 5.
 *      Author: Peter
 */

#ifndef FECENCODER_H_
#define FECENCODER_H_

#include "WIMAX.h"

class FECEncoder : WIMAX {
public:
	FECEncoder();
	virtual ~FECEncoder();

	void SlotConcatenate(void);
	void CTCEncoding(void);

	friend class DataRandomizer;

public:
	BIT m_datain[MAXFECBLOCKSIZE];
	BIT m_dataout[MAXFECBLOCKSIZE];

	WIMAXMCS 		m_MCS;
	HARQMODE		m_HARQ;
	WIMAXTDDMODE	m_TDD;

	unsigned int 	m_BurstSize; 		 // byte
	unsigned int 	m_PaddedBurstSize;   // byte
	unsigned int 	m_EncodedBurstSize;  // byte

	unsigned int    m_Ns; //the number of the slots allocated for the data burst

	unsigned char   m_RepititionFactor;
	unsigned char   m_STCLayerNumber;
	unsigned char   m_STCRate;

	// HARQ
	unsigned char 	m_SPID;
	unsigned char 	m_SubpacketIndex;
	unsigned int    m_Nsch;


private:
	struct CTC_ChannelCoding_per_Modulation_noHARQ // Table 502
	{
		WIMAXMCS	 MCS;
		unsigned int DataBlockSize;
		unsigned int EncodedDataBlockSize;
		unsigned int N;
		unsigned int P0;
		unsigned int P1;
		unsigned int P2;
		unsigned int P3;
	};
	struct CTC_ChannelCoding_per_Modulation_HARQ  // Table 503
	{
		unsigned int DataBlockSize;
		unsigned int N;
		unsigned int P0;
		unsigned int P1;
		unsigned int P2;
		unsigned int P3;
	};
	struct CTC_SubblockInterleaver_Parameter // Table 505
	{
		unsigned char m;
		unsigned char J;
	};
	const CTC_ChannelCoding_per_Modulation_noHARQ CTC_ChannelCoding_per_Modulation_noHARQ502[32] =
	{
		//   MCS     in  out    N   P0   P1   P2   P3  // Table 502 pp 1038
		 { QPSK_1_2,  6,  12,  24,   5,   0,   0,   0 },
		 { QPSK_1_2, 12,  24,  48,  13,  24,   0,  24 },
		 { QPSK_1_2, 18,  36,  72,  11,   6,   0,   6 },
		 { QPSK_1_2, 24,  48,  96,   7,  48,  24,  72 },
		 { QPSK_1_2, 30,  60, 120,  13,  60,   0,  60 },
		 { QPSK_1_2, 36,  72, 144,  17,  74,  72,   2 },
		 { QPSK_1_2, 48,  96, 192,  11,  96,  48, 144 },
		 { QPSK_1_2, 54, 108, 216,  13, 108,   0, 108 },
		 { QPSK_1_2, 60, 120, 240,  13, 120,  60, 180 },
		 { QPSK_3_4,  9,  12,  36,  11,  18,   0,  18 },
		 { QPSK_3_4, 18,  24,  72,  11,   6,   0,   6 },
		 { QPSK_3_4, 27,  36, 108,  11,  54,  56,   2 },
		 { QPSK_3_4, 36,  48, 144,  17,  74,  72,   2 },
		 { QPSK_3_4, 45,  60, 180,  11,  90,   0,  90 },
		 { QPSK_3_4, 54,  72, 216,  13, 108,   0, 108 },

		 { QAM16_1_2, 12,  24,  48,  13,  24,   0,  24 },
		 { QAM16_1_2, 24,  48,  96,   7,  48,  24,  72 },
		 { QAM16_1_2, 36,  72, 144,  17,  74,  72,   2 },
		 { QAM16_1_2, 48,  96, 192,  11,  96,  48, 144 },
		 { QAM16_1_2, 60, 120, 240,  13, 120,  60, 180 },
		 { QAM16_3_4, 18,  24,  72,  11,   6,   0,   6 },
		 { QAM16_3_4, 36,  48, 144,  17,  74,  72,   2 },
		 { QAM16_3_4, 54,  72, 216,  13, 108,   0, 108 },

		 { QAM64_1_2, 18,  36,  72,  11,   6,   0,   6 },
		 { QAM64_1_2, 36,  72, 144,  17,  74,  72,   2 },
		 { QAM64_1_2, 54, 108, 216,  13, 108,   0, 108 },
		 { QAM64_2_3, 24,  36,  96,   7,  48,  24,  72 },
		 { QAM64_2_3, 48,  72, 192,  11,  96,  48, 144 },
		 { QAM64_3_4, 27,  36, 108,  11,  54,  56,   2 },
		 { QAM64_3_4, 54,  72, 216,  13, 108,   0, 108 },
		 { QAM64_5_6, 30,  36, 120,  13,  60,   0,  60 },
		 { QAM64_5_6, 60,  72, 240,  13, 120,  60, 180 }
	}; 
	const CTC_ChannelCoding_per_Modulation_HARQ CTC_ChannelCoding_per_Modulation_HARQ503[12] =
	{
		//   MCS     in          N   P0   P1   P2   P3  // Table 502 pp 1038
		 {             6,       24,   5,   0,   0,   0 },
		 {            12,       48,  13,  24,   0,  24 },
		 {            18,       72,  11,   6,   0,   6 },
		 {            24,       96,   7,  48,  24,  72 },

		 {            36,      144,  17,  74,  72,   2 },
		 {            48,      192,  11,  96,  48, 144 },
		 {            60,      240,  13, 120,  60, 180 },
		 {           120,      480,  53,  62,  12,   2 },

		 {           240,      960,  43,  64, 300, 824 },
		 {           360,     1440,  43, 720, 360, 540 },
		 {           480,     1920,  31,   8,  24,  16 },
		 {           600,     2400,  53,  66,  24,   2 }
	};
	const unsigned char CTC_ChannelCoding_per_Modulation_Num = 32;
	unsigned char CTC_Circulation_State[6][8] =
	{ // Table 504 Circulation state lookup table
			{ 0, 6, 4, 2, 7, 1, 3, 5 },
			{ 0, 3, 7, 4, 5, 6, 2, 1 },
			{ 0, 5, 3, 6, 2, 7, 1, 4 },
			{ 0, 4, 1, 5, 6, 2, 7, 3 },
			{ 0, 2, 5, 7, 1, 3, 4, 6 },
			{ 0, 7, 6, 1, 3, 4, 5, 2 }
	};
	CTC_SubblockInterleaver_Parameter CTC_SubblockInterleaver_Parameter_noHARQ[32] =
	{ // m  J
	   { 3, 3 },
	   { 4, 3 },
	   { 5, 3 },
	   { 5, 3 },
	   { 6, 2 },
	   { 6, 3 },
	   { 6, 3 },
	   { 6, 4 },
	   { 7, 2 },
	   { 4, 3 },
	   { 5, 3 },
	   { 5, 4 },
	   { 6, 3 },
	   { 6, 4 },

	   { 4, 3 },
	   { 5, 3 },
	   { 6, 3 },
	   { 6, 3 },
	   { 7, 2 },
	   { 5, 3 },
	   { 6, 3 },
	   { 6, 4 },

	   { 5, 3 },
	   { 6, 3 },
	   { 6, 4 },
	   { 5, 3 },
	   { 6, 3 },
	   { 5, 4 },
	   { 6, 4 },
	   { 6, 2 },
	   { 7, 2 }
	};
	CTC_SubblockInterleaver_Parameter CTC_SubblockInterleaver_Parameter_HARQ[12] =
	{ // m  J
	   { 3, 3 },
	   { 4, 3 },
	   { 5, 3 },
	   { 5, 3 },

	   { 6, 3 },
	   { 6, 3 },
	   { 7, 2 },
	   { 8, 2 },

	   { 9, 2 },
	   { 9, 3 },
	   {10, 2 },
	   {10, 3 }
	};
	unsigned char m_j_list[8] = { 10, 6, 5, 3, 3, 2, 2, 2 }; // Slot Concatenation
	unsigned int CTC_IR_PDUSize[16] =
	{ 32, 80, 128, 176, 272, 368, 464, 944,
	1904, 2864, 3824, 4784, 9584, 14384, 19184, 23984 }; // 8.4.9.2.3.5.1 Padding  bits

private:
	BIT A[MAXFECBLOCKSIZE/2];
	BIT B[MAXFECBLOCKSIZE/2];
	BIT Y1[MAXFECBLOCKSIZE/2];
	BIT Y2[MAXFECBLOCKSIZE/2];
	BIT W1[MAXFECBLOCKSIZE/2];
	BIT W2[MAXFECBLOCKSIZE/2];

	BIT Ai[MAXFECBLOCKSIZE/2];
	BIT Bi[MAXFECBLOCKSIZE/2];
	BIT Y1i[MAXFECBLOCKSIZE/2];
	BIT Y2i[MAXFECBLOCKSIZE/2];
	BIT W1i[MAXFECBLOCKSIZE/2];
	BIT W2i[MAXFECBLOCKSIZE/2];

	unsigned int    m_EncodedDataBlockSize;
	char 			m_Interleaver_Index;
	unsigned char 	m_j;
	unsigned int 	m_n;
	unsigned int 	m_N;

	unsigned char 	m_MPRx6; 		// Modulation per coderate multiple by 6
	unsigned char   m_MODORDER;
	unsigned char   m_FECBlockNumber;
	unsigned int    m_SlotNumber[MAXFECBLOCKNUM];

	struct {
		unsigned int bStart;
		unsigned int bEnd;
		unsigned int BDataBlockSize;
	}FECBlockIndex[MAXFECBLOCKNUM];

	void SetDataBlock(unsigned char BlockIndex);
	unsigned char ConstituentEncoding(BIT * K1, BIT * K2);
	void ConstituentEncoding(unsigned char Sc, BIT * K1, BIT * K2, BIT * Y, BIT * W);
	char SetInterleaverParameter(unsigned char BlockIndex);
	void Interleaving(unsigned char BlockIndex);
	void SubblockInterleaving(void);
	unsigned int BitSelect(unsigned int BlockIndex, unsigned int DataOutOffset);
};

#endif /* CTCENCODER_H_ */
