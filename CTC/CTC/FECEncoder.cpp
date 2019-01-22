/*
 * FECEncoder.cpp
 *
 *  Created on: 2012. 4. 5.
 *      Author: Peter
 */

#include "FECEncoder.h"

#include <iostream>
using namespace std;

FECEncoder::FECEncoder() 
{
	// TODO Auto-generated constructor stub
}
FECEncoder::~FECEncoder() {
	// TODO Auto-generated destructor stub
}


void FECEncoder::SetDataBlock(unsigned char BlockIndex) {
	unsigned int index;

	// alternately fed 8.4.9.2.3.1
	for(index=0;index< (FECBlockIndex[BlockIndex].BDataBlockSize * 4);index++)
	{
		A[index] = m_datain[FECBlockIndex[BlockIndex].bStart+index*2];
		B[index] = m_datain[FECBlockIndex[BlockIndex].bStart+index*2+1];
	}
}

unsigned char FECEncoder::ConstituentEncoding(BIT * K1, BIT * K2) {
	unsigned char S0N_1;
	unsigned int index;
	BIT s[3];
	BIT sum;

	s[0] = 0; s[1] = 0; s[2] = 0; // initialize
	for(index=0;index<m_N;index++)
	{   // Fig 289 CTC Encoder
//		cout << 2*K1[index] + K2[index] << " ";
		sum  = K1[index] ^ K2[index] ^ s[0] ^s[2];
	    s[2] = s[1] ^ K2[index];
	    s[1] = s[0] ^ K2[index];
	    s[0] = sum;
	}
	S0N_1 = 4*s[0] + 2*s[1] + s[2]; // circular state determine
//	cout << endl;
	return S0N_1;
}

void FECEncoder::SlotConcatenate(void) {
	unsigned int t;
	unsigned int k;
	unsigned int m;
	unsigned int Lb1;
	unsigned int Lb2;
	unsigned int FECBlockLength;
	unsigned int FECDataBlockSize;


	// Table 501 Encoding slot concatenation for different rates in CTC
	switch(m_MCS)
	{
		case QPSK_1_2:
			m_j = m_j_list[0];
			m_MODORDER = 2;
			m_MPRx6 = 6;
			break;
		case QPSK_3_4:
			m_j = m_j_list[1];
			m_MODORDER = 2;
			m_MPRx6 = 9;
			break;
		case QAM16_1_2:
			m_j = m_j_list[2];
			m_MODORDER = 4;
			m_MPRx6 = 12;
			break;
		case QAM16_3_4:
			m_j = m_j_list[3];
			m_MODORDER = 4;
			m_MPRx6 = 18;
			break;
		case QAM64_1_2:
			m_j = m_j_list[4];
			m_MODORDER = 6;
			m_MPRx6 = 18;
			break;
		case QAM64_2_3:
			m_j = m_j_list[5];
			m_MODORDER = 6;
			m_MPRx6 = 24;
			break;
		case QAM64_3_4:
			m_j = m_j_list[6];
			m_MODORDER = 6;
			m_MPRx6 = 27;
			break;
		case QAM64_5_6:
			m_j = m_j_list[7];
			m_MODORDER = 6;
			m_MPRx6 = 30;
			break;
	}
	// the number of allocated slot
	m_n = Floor ( ((float)m_Ns * m_STCRate) / (m_RepititionFactor * m_STCLayerNumber));

	// Table 500 Slot concatenation rule
	if((m_n <= m_j) &&(m_n != 7)){
		m_SlotNumber[0] = m_n;
		m_FECBlockNumber = 1;
	}else if((m_n <= m_j) &&(m_n == 7)){
		m_SlotNumber[0] = 4;
		m_SlotNumber[1] = 3;
		m_FECBlockNumber = 2;
	}else{ // m_n > m_j
		k = Floor (((float)m_n / m_j));
		if( m_n % m_j == 0){
			for(t=0;t<k;t++)
			{
				m_SlotNumber[t]= m_j;
			}
			m_FECBlockNumber 			= k;
		}else{
			m = m_n % m_j;
			Lb1 = Ceil	(((float)(m+m_j)/2));
			Lb2 = Floor	(((float)(m+m_j)/2));

			if((Lb1 == 7) || (Lb2 ==7))
			{
				Lb1++;
				Lb2--;
			}

			for(t=0;t<(k-1);t++)
			{
				m_SlotNumber[t] 	= m_j;
			}
			m_SlotNumber[k-1] 	= Lb1;
			m_SlotNumber[k] 	= Lb2;
			m_FECBlockNumber 	= k+1;
		}
	}

	k = 0;
	// Divide burst into FECblock
	for(m=0;m<m_FECBlockNumber;m++)
	{
		FECDataBlockSize 		= m_SlotNumber[m] * m_MPRx6; // byte
		FECBlockLength 			= FECDataBlockSize * 8;		 // bit
		FECBlockIndex[m].bStart 	= k;
		k 						= k + FECBlockLength; // Next Block Start
		FECBlockIndex[m].bEnd 	= k - 1;
		FECBlockIndex[m].BDataBlockSize = FECDataBlockSize;
	}
}

void FECEncoder::ConstituentEncoding(unsigned char Sc, BIT * K1, BIT * K2, BIT * Y, BIT * W) {
	unsigned int index;
	BIT s[3];
	BIT sum;

	s[0] = (Sc & 0x4)>>2; s[1] = (Sc & 0x2)>>1; s[2] = (Sc & 0x1); // initialize
//    cout << (s[0] <<2) + (s[1] <<1) + (s[2] ) << " ";
	for(index=0;index<m_N;index++)
	{   // Fig 289 CTC Encoder
		sum  = K1[index] ^ K2[index] ^ s[0] ^s[2];
		Y[index] = sum ^ s[1] ^ s[2];
		W[index] = sum ^ s[2];
	    s[2] = s[1] ^ K2[index];
	    s[1] = s[0] ^ K2[index];
	    s[0] = sum;

//	    cout << (s[0] <<2) + (s[1] <<1) + (s[2] ) << " " << (unsigned int)K1[index] << (unsigned int)K2[index] << (unsigned int)Y[index] << (unsigned int)W[index] << endl;
	}
}

char FECEncoder::SetInterleaverParameter(unsigned char BlockIndex)
{
	char k;
	k = 0;

	// find Table 502 from Data block Size(byte)
	while( (k < CTC_ChannelCoding_per_Modulation_Num) &&
		(!((CTC_ChannelCoding_per_Modulation_noHARQ502[(unsigned char)k].DataBlockSize == FECBlockIndex[BlockIndex].BDataBlockSize) &&
		(CTC_ChannelCoding_per_Modulation_noHARQ502[(unsigned char)k].MCS == m_MCS))) )
	{
		k++;
	}

	if(k >= CTC_ChannelCoding_per_Modulation_Num)
	{
		k = -1; // unsupported FEC Block Size
		cout << "unsupported FEC Block Size";
	}
	return k;
}

void FECEncoder::Interleaving(unsigned char BlockIndex) {
	unsigned int Block_Index;
	unsigned int Block_Index_mul4;

	unsigned int N_div2;
	unsigned int N_div4;

	unsigned int Pj0;
	unsigned int Pj1;
	unsigned int Pj2;
	unsigned int Pj3;

	unsigned int P0;
	unsigned int P1;
	unsigned int P2;
	unsigned int P3;

	if((m_HARQ == NO)||(m_HARQ == CC))
	{
		//	m_N  = CTC_ChannelCoding_per_Modulation_502_noHARQ[(unsigned char)m_Interleaver_Index].N;
		P0 = CTC_ChannelCoding_per_Modulation_noHARQ502[(unsigned char)m_Interleaver_Index].P0;
		P1 = CTC_ChannelCoding_per_Modulation_noHARQ502[(unsigned char)m_Interleaver_Index].P1;
		P2 = CTC_ChannelCoding_per_Modulation_noHARQ502[(unsigned char)m_Interleaver_Index].P2;
		P3 = CTC_ChannelCoding_per_Modulation_noHARQ502[(unsigned char)m_Interleaver_Index].P3;

		m_EncodedDataBlockSize = CTC_ChannelCoding_per_Modulation_noHARQ502[(unsigned char)m_Interleaver_Index].EncodedDataBlockSize;
	}else if(m_HARQ == IR){
		//	m_N  = CTC_ChannelCoding_per_Modulation_502_noHARQ[(unsigned char)m_Interleaver_Index].N;
		P0 = CTC_ChannelCoding_per_Modulation_HARQ503[(unsigned char)m_Interleaver_Index].P0;
		P1 = CTC_ChannelCoding_per_Modulation_HARQ503[(unsigned char)m_Interleaver_Index].P1;
		P2 = CTC_ChannelCoding_per_Modulation_HARQ503[(unsigned char)m_Interleaver_Index].P2;
		P3 = CTC_ChannelCoding_per_Modulation_HARQ503[(unsigned char)m_Interleaver_Index].P3;

		m_EncodedDataBlockSize = 6 * m_Nsch * m_MODORDER; // Lk @ BitSelection byte
	}

	m_EncodedBurstSize += m_EncodedDataBlockSize; // Acc for FEC blocks

	N_div2 = m_N >> 1; // N
	N_div4 = m_N >> 2; // N/2

	if(m_Interleaver_Index != -1)
	{
		// Step 1. Switch alternate couples, CTC Interleaver
		for(Block_Index=0;Block_Index<N_div2;Block_Index++)
		{
			// no switch
			Y2[2*Block_Index]   = m_datain[4*Block_Index  ];
			W2[2*Block_Index]   = m_datain[4*Block_Index+1];
			// switch
			Y2[2*Block_Index+1] = m_datain[4*Block_Index+3];
			W2[2*Block_Index+1] = m_datain[4*Block_Index+2];
		}

		// Step 2. P(j)
		for(Block_Index=0;Block_Index<N_div4;Block_Index++)
		{
			Block_Index_mul4 = 4*Block_Index;

			Pj0 = (P0 * (Block_Index_mul4    ) + 1             ) % m_N; // odd
			Pj1 = (P0 * (Block_Index_mul4 + 1) + 1 + m_N/2 + P1) % m_N; // even
			Pj2 = (P0 * (Block_Index_mul4 + 2) + 1         + P2) % m_N; // even
			Pj3 = (P0 * (Block_Index_mul4 + 3) + 1 + m_N/2 + P3) % m_N; // even

			// do not use same variable between left side and right side
			Ai[Block_Index_mul4  ] = Y2[Pj0];
			Bi[Block_Index_mul4  ] = W2[Pj0];
			Ai[Block_Index_mul4+1] = Y2[Pj1];
			Bi[Block_Index_mul4+1] = W2[Pj1];
			Ai[Block_Index_mul4+2] = Y2[Pj2];
			Bi[Block_Index_mul4+2] = W2[Pj2];
			Ai[Block_Index_mul4+3] = Y2[Pj3];
			Bi[Block_Index_mul4+3] = W2[Pj3];
		}
	}
}

void FECEncoder::SubblockInterleaving(void) {
	unsigned char m, J;
	unsigned int i, k;
	unsigned int Tk;
	unsigned int index;

	// CTC_SubblockInterleaver_Parameter Table 505
	m = CTC_SubblockInterleaver_Parameter_noHARQ[(unsigned char)m_Interleaver_Index].m;
	J = CTC_SubblockInterleaver_Parameter_noHARQ[(unsigned char)m_Interleaver_Index].J;

	// 8.4.9.2.3.4.2 Subblock Interleaving
	i = 0;
	k = 0;

	for(i=0;i<m_N;i++)
	{
		Tk = m_N; // initial
		while(Tk >= m_N)
		{
			// range of k : 0 ~ (N-1) bits
			Tk = (1 << m) * (k % J) + BRO(Floor(((float)k)/J), m); // Formula c
			k++;
		}
//		AD[i] = Tk;

		// Interleave separately

		// No Bit Grouping for systematic bits
		Ai[i] = A[Tk];
		Bi[i] = B[Tk];

		// Bit Grouping 8.4.9.2.3.4.3
		if(i < (m_N/2) )
		{
			Y1i[2*i]   = Y1[Tk];
			Y1i[2*i+1] = Y2[Tk];

			W1i[2*i]   = W1[Tk];
			W1i[2*i+1] = W2[Tk];
		}else{
			index = (i - m_N/2);

			Y2i[2*index]   = Y1[Tk];
			Y2i[2*index+1] = Y2[Tk];

			W2i[2*index]   = W1[Tk];
			W2i[2*index+1] = W2[Tk];
		}
	}
}

unsigned int FECEncoder::BitSelect(unsigned int BlockIndex, unsigned int DataOutOffset) {
	unsigned int index;
	unsigned int offset;
	unsigned int Lk;
	unsigned int Nepx3;
	unsigned int Sk0;
	unsigned int Ski;
	unsigned int Fk;
	unsigned int EncodedDataBlockSizeBit;
	BIT EncodedBlock[MAXFECBLOCKSIZE*3];

	EncodedDataBlockSizeBit = m_EncodedDataBlockSize * 8;
	Nepx3 = (3 * (2 * m_N));

	if((m_HARQ == NO)||(m_HARQ == CC))
	{
		Sk0 = 0;
		// FECBlock Dataout Start offset
		offset = DataOutOffset;
	}else if(m_HARQ == IR){
		// Bit selection 8.4.9.2.3.4.4
		Nepx3 = (3* (2*m_N));
		Lk = m_EncodedDataBlockSize * 8; // bit
		Fk  = ((m_SPID * Lk) % Nepx3);
		Sk0 = (Fk 	  % Nepx3);

		// FECBlock Dataout Start offset
		offset = DataOutOffset + Sk0;
	}

	// Copy to buffer 1/3 code
	for(index=0;index<m_N;index++)
	{
		EncodedBlock[        index] =  Ai[index];
		EncodedBlock[  m_N + index] =  Bi[index];

		EncodedBlock[2*m_N + index] = Y1i[index];
		EncodedBlock[3*m_N + index] = Y2i[index];

		EncodedBlock[4*m_N + index] = W1i[index];
		EncodedBlock[5*m_N + index] = W2i[index];
	}

	for(index=0;index<EncodedDataBlockSizeBit;index++)
	{
		Ski = (Sk0 + index) % Nepx3; // eq 117
		m_dataout[offset + index] = EncodedBlock[Ski];
	}

	// Next FECBlock Dataout Start offset
	DataOutOffset = offset + EncodedDataBlockSizeBit;

	return DataOutOffset;
}

void FECEncoder::CTCEncoding(void) {
	unsigned char Nmod7;
	unsigned char S0N_1_C1, S0N_1_C2;
	unsigned char Sc1, Sc2;
	unsigned char BlockIndex;
	unsigned int  DataOutOffset;

	DataOutOffset = 0;
	m_EncodedBurstSize = 0;
	if(m_HARQ == NO)
	{
		SlotConcatenate();
	}

	for(BlockIndex=0;BlockIndex<m_FECBlockNumber;BlockIndex++)
	{
		m_Interleaver_Index = SetInterleaverParameter(BlockIndex);

		m_N  = CTC_ChannelCoding_per_Modulation_noHARQ502[(unsigned char)m_Interleaver_Index].N;
		Nmod7 = m_N % 7;

		SetDataBlock(BlockIndex); // Copy Datablock into A, B

		// CTC Circular State 8.4.9.2.3.3
		S0N_1_C1 = ConstituentEncoding(this->A, this->B);
		Sc1 = CTC_Circulation_State[Nmod7-1][S0N_1_C1];
		// C1 encoding
		ConstituentEncoding(Sc1, this->A, this->B, this->Y1, this->W1);

//		cout << "Sc1 : " << (unsigned int) Sc1 << endl;

		// Interleaving 8.4.9.2.3.2
		Interleaving(BlockIndex);
		// CTC Circular State 8.4.9.2.3.3
		S0N_1_C2 = ConstituentEncoding(this->Ai, this->Bi);
		Sc2 = CTC_Circulation_State[Nmod7-1][S0N_1_C2];
		// C2 encoding
		ConstituentEncoding(Sc2, this->Ai, this->Bi, this->Y2, this->W2);

//		cout << "Sc2 : " << (unsigned int) Sc2 << endl;

		// SubblockInterleaving 8.4.9.2.3.4.2 & Bit grouping 8.4.9.2.3.4.3
		SubblockInterleaving();
		// Bit Selection 8.4.9.2.3.4.4
		DataOutOffset = BitSelect(BlockIndex, DataOutOffset);
	}
}
