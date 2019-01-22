/*
 * FECDecoder.h
 *
 *  Created on: 2012. 5. 13.
 *      Author: Peter
 */

#ifndef FECDECODER_H_
#define FECDECODER_H_

#include "WIMAX.h"

class FECDecoder: WIMAX {
public:
	FECDecoder();
	virtual ~FECDecoder();

	void CTCDecoding(void);

	friend class DataRandomizer;

public:
	BYTE m_Bdatain[MAXFECBLOCKSIZE];
	BIT  m_bdataout[MAXFECBLOCKSIZE];

	WIMAXMCS 		m_eMCS;
	HARQMODE		m_eHARQ;

	unsigned int 	m_BBurstSize; 		 // byte
	unsigned int 	m_BPaddedBurstSize;   // byte
	unsigned int 	m_BEncodedBurstSize;  // byte

	unsigned int    m_nNs; //the number of the slots allocated for the data burst

	unsigned char   m_nRepititionFactor;
	unsigned char   m_nSTCLayerNumber;
	unsigned char   m_nSTCRate;

	// HARQ
	unsigned char 	m_nSPID;
	unsigned char 	m_nSubpacketIndex;
	unsigned int    m_nNsch;

	unsigned char   m_Iteration;

private:
	struct CTC_ChannelCoding_per_Modulation_noHARQ // Table 502
	{
		WIMAXMCS	 eMCS;
		unsigned int BDataBlockSize;
		unsigned int BEncodedDataBlockSize;
		unsigned int bN;
		unsigned int bP0;
		unsigned int bP1;
		unsigned int bP2;
		unsigned int bP3;
	};
	struct CTC_ChannelCoding_per_Modulation_HARQ  // Table 503
	{
		unsigned int BDataBlockSize;
		unsigned int bN;
		unsigned int bP0;
		unsigned int bP1;
		unsigned int bP2;
		unsigned int bP3;
	};
	struct CTC_SubblockInterleaver_Parameter // Table 505
	{
		unsigned char nm;
		unsigned char nJ;
	};
	const CTC_ChannelCoding_per_Modulation_noHARQ sCTC_ChannelCoding_per_Modulation_noHARQ502[32] =
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
	const CTC_ChannelCoding_per_Modulation_HARQ sCTC_ChannelCoding_per_Modulation_HARQ503[12] =
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
	const unsigned char nCTC_ChannelCoding_per_Modulation_Num = 32;
	const unsigned char nCTC_Circulation_State[6][8] =
	{ // Table 504 Circulation state lookup table
		{ 0, 6, 4, 2, 7, 1, 3, 5 },
		{ 0, 3, 7, 4, 5, 6, 2, 1 },
		{ 0, 5, 3, 6, 2, 7, 1, 4 },
		{ 0, 4, 1, 5, 6, 2, 7, 3 },
		{ 0, 2, 5, 7, 1, 3, 4, 6 },
		{ 0, 7, 6, 1, 3, 4, 5, 2 }
	};
	const CTC_SubblockInterleaver_Parameter sCTC_SubblockInterleaver_Parameter_noHARQ[32] =
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
	const CTC_SubblockInterleaver_Parameter sCTC_SubblockInterleaver_Parameter_HARQ[12] =
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
	const unsigned char m_nj_list[8] = { 10, 6, 5, 3, 3, 2, 2, 2 }; // Slot Concatenation
	const unsigned int nCTC_IR_PDUSize[16] =
	{ 32, 80, 128, 176, 272, 368, 464, 944,
	1904, 2864, 3824, 4784, 9584, 14384, 19184, 23984 }; // 8.4.9.2.3.5.1 Padding  bits
	const unsigned char LastState[CTCSTATENUM][CTCINPUTNUM] =
	{ { 0, 6, 1, 7 }, { 2, 4, 3, 5 }, { 5, 3, 4, 2 }, { 7, 1, 6, 0 },
		{ 1, 7, 0, 6 }, { 3, 5, 2, 4 }, { 4, 2, 5, 3 }, { 6, 0, 7, 1 } };
	const unsigned char NextState[CTCSTATENUM][CTCINPUTNUM] =
	{ { 0, 7, 4, 3 }, { 4, 3, 0, 7 }, { 1, 6, 5, 2 }, { 5, 2, 1, 6 },
		{ 6, 1, 2, 5 }, { 2, 5, 6, 1 }, { 7, 0, 3, 4 }, { 3, 4, 7, 0 } };

private:
	char PaddedEncodedBlock[MAXFECBLOCKSIZE+3];

	char A [MAXFECBLOCKSIZE/2];
	char B [MAXFECBLOCKSIZE/2];
	char Ai [MAXFECBLOCKSIZE/2];
	char Bi [MAXFECBLOCKSIZE/2];
	char Y1[MAXFECBLOCKSIZE/2];
	char Y2[MAXFECBLOCKSIZE/2];
	char W1[MAXFECBLOCKSIZE/2];
	char W2[MAXFECBLOCKSIZE/2];

	unsigned int P0;
	unsigned int P1;
	unsigned int P2;
	unsigned int P3;

	int BM    [MAXFECBLOCKSIZE][CTCSTATENUM][CTCINPUTNUM];//BlockSize(N),State, Path

	int EXT [MAXFECBLOCKSIZE+1][CTCINPUTNUM];
	int EXTi[MAXFECBLOCKSIZE+1][CTCINPUTNUM];
	int         Alpha[MAXFECBLOCKSIZE+1][CTCSTATENUM];
	int         Beta [MAXFECBLOCKSIZE+1][CTCSTATENUM];
	int         APP  [MAXFECBLOCKSIZE  ][CTCINPUTNUM];

	unsigned int    m_BEncodedDataBlockSize;
	char 			m_nInterleaver_Index;
	unsigned char 	m_nj;
	unsigned int 	m_nn;
	unsigned int 	m_bN;

	unsigned char 	m_nMPRx6; 		// Modulation per coderate multiple by 6
	unsigned char   m_nMODORDER;
	unsigned char   m_nFECBlockNumber;
	unsigned int    m_nSlotNumber[MAXFECBLOCKNUM];

	unsigned char m_InitialStateIndex[2];


	struct {
		unsigned int Start;
		unsigned int End;
		unsigned int DataBlockSize;
	}FECBlockIndex[MAXFECBLOCKNUM];

	void SlotConcatenate(void);
	unsigned int InverseBitSelect(unsigned int bNepx3, unsigned int bEncodedDataBlockSize, unsigned int DataOutOffset, unsigned char BlockIndex);
	char SetInterleaverParameter(unsigned char BlockIndex);
	void InverseSubblockInterleaving(void);
	void MaxLogMAPDecoding(unsigned char Iteration);
	void CalcBranchMetric(char * AA, char * BB, char * Y, char * W) ;
	void CalcBranchMetricSP(char * A, char * B, char * Y, char * W);
	void CalcBranchMetricP(char * A, char * B, char * Y, char * W);
	void CalcAlpha(void);
	void CalcBeta(void);
	void CalcAPP(void);
	void ExtrinsicScaling(char * AA, char * BB);
	void Interleaving(void);
	void InterleavingSystematic(void);
	void InverseInterleaving(void);
	void Decision(unsigned char FECBlockIndex);
	unsigned char EstCircularState(void);
	void SetInitialState(unsigned char Index);
};

#endif /* FECDECODER_H_ */
