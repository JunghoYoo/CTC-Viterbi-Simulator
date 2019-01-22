/*
 * FECDecoder.cpp
 *
 *  Created on: 2012. 5. 13.
 *      Author: Peter
 */

#include "FECDecoder.h"

#include <iostream>
using namespace std;

#define MININF -256

FECDecoder::FECDecoder()
{
	// TODO Auto-generated constructor stub
}

FECDecoder::~FECDecoder() {
	// TODO Auto-generated destructor stub
}

void FECDecoder::SlotConcatenate(void) {
	unsigned int t;
	unsigned int k;
	unsigned int m;
	unsigned int Lb1;
	unsigned int Lb2;
	unsigned int FECBlockLength;
	unsigned int FECDataBlockSize;


	// Table 501 Encoding slot concatenation for different rates in CTC
	switch(m_eMCS)
	{
		case QPSK_1_2:
			m_nj = m_nj_list[0];
			m_nMODORDER = 2;
			m_nMPRx6 = 6;
			break;
		case QPSK_3_4:
			m_nj = m_nj_list[1];
			m_nMODORDER = 2;
			m_nMPRx6 = 9;
			break;
		case QAM16_1_2:
			m_nj = m_nj_list[2];
			m_nMODORDER = 4;
			m_nMPRx6 = 12;
			break;
		case QAM16_3_4:
			m_nj = m_nj_list[3];
			m_nMODORDER = 4;
			m_nMPRx6 = 18;
			break;
		case QAM64_1_2:
			m_nj = m_nj_list[4];
			m_nMODORDER = 6;
			m_nMPRx6 = 18;
			break;
		case QAM64_2_3:
			m_nj = m_nj_list[5];
			m_nMODORDER = 6;
			m_nMPRx6 = 24;
			break;
		case QAM64_3_4:
			m_nj = m_nj_list[6];
			m_nMODORDER = 6;
			m_nMPRx6 = 27;
			break;
		case QAM64_5_6:
			m_nj = m_nj_list[7];
			m_nMODORDER = 6;
			m_nMPRx6 = 30;
			break;
	}
	// the number of allocated slot
	m_nn = Floor ( ((float)m_nNs * m_nSTCRate) / (m_nRepititionFactor * m_nSTCLayerNumber));

	// Table 500 Slot concatenation rule
	if((m_nn <= m_nj) &&(m_nn != 7)){
		m_nSlotNumber[0] = m_nn;
		m_nFECBlockNumber = 1;
	}else if((m_nn <= m_nj) &&(m_nn == 7)){
		m_nSlotNumber[0] = 4;
		m_nSlotNumber[1] = 3;
		m_nFECBlockNumber = 2;
	}else{ // m_n > m_j
		k = Floor (((float)m_nn / m_nj));
		if( m_nn % m_nj == 0){
			for(t=0;t<k;t++)
			{
				m_nSlotNumber[t]= m_nj;
			}
			m_nFECBlockNumber 			= k;
		}else{
			m = m_nn % m_nj;
			Lb1 = Ceil	(((float)(m+m_nj)/2));
			Lb2 = Floor	(((float)(m+m_nj)/2));

			if((Lb1 == 7) || (Lb2 ==7))
			{
				Lb1++;
				Lb2--;
			}

			for(t=0;t<(k-1);t++)
			{
				m_nSlotNumber[t] 	= m_nj;
			}
			m_nSlotNumber[k-1] 	= Lb1;
			m_nSlotNumber[k] 	= Lb2;
			m_nFECBlockNumber 	= k+1;
		}
	}

	k = 0;
	// Divide burst into FECblock
	for(m=0;m<m_nFECBlockNumber;m++)
	{
		FECDataBlockSize 		= m_nSlotNumber[m] * m_nMPRx6; // byte
		FECBlockLength 			= FECDataBlockSize * 8;		 // bit
		FECBlockIndex[m].Start 	= k;
		k 						= k + FECBlockLength; // Next Block Start
		FECBlockIndex[m].End 	= k - 1;
		FECBlockIndex[m].DataBlockSize = FECDataBlockSize;
	}
}

char FECDecoder::SetInterleaverParameter(unsigned char BlockIndex)
{
	char k;
	k = 0;

	// find Table 502 from Data block Size(byte)
	while( (k < nCTC_ChannelCoding_per_Modulation_Num) &&
		(!((sCTC_ChannelCoding_per_Modulation_noHARQ502[(unsigned char)k].BDataBlockSize == FECBlockIndex[BlockIndex].DataBlockSize) &&
		(sCTC_ChannelCoding_per_Modulation_noHARQ502[(unsigned char)k].eMCS == m_eMCS))) )
	{
		k++;
	}

	if(k >= nCTC_ChannelCoding_per_Modulation_Num)
	{
		k = -1; // unsupported FEC Block Size
		cout << "unsupported FEC Block Size";
	}
	return k;
}

unsigned int FECDecoder::InverseBitSelect(unsigned int bNepx3, unsigned int bEncodedDataBlockSize, unsigned int DataOutOffset, unsigned char BlockIndex) {
	unsigned int index;
	unsigned int offset;
	unsigned int Lk;
	unsigned int Sk0;
//	unsigned int Ski;
	unsigned int Fk;
//	unsigned int EncodedDataBlockSizeBit;
//	BIT EncodedBlock[MAXFECBLOCKSIZE*3];

	if((m_eHARQ == NO)||(m_eHARQ == CC))
	{
		Sk0 = 0;
		// FECBlock Dataout Start offset
		offset = DataOutOffset;
	}else if(m_eHARQ == IR){
		// Bit selection 8.4.9.2.3.4.4
		Lk = m_BEncodedDataBlockSize * 8; // bit
		Fk  = ((m_nSPID * Lk) % bNepx3);
		Sk0 = (Fk 	  % bNepx3);

		// FECBlock Dataout Start offset
		offset = DataOutOffset + Sk0;
	}

	// copy received encoded bits
	for(index=0;index<bEncodedDataBlockSize;index++)
	{
//		Ski = (Sk0 + index) % bNepx3;

		PaddedEncodedBlock[index] = m_Bdatain[FECBlockIndex[BlockIndex].Start+index];
	}

	// zero padding
	for(index=bEncodedDataBlockSize;index<bNepx3;index++)
	{
		PaddedEncodedBlock[index] = 0;// bit 0

	}
	// Next FECBlock Dataout Start offset
	DataOutOffset = offset + bEncodedDataBlockSize;

	return DataOutOffset;
}

void FECDecoder::InverseSubblockInterleaving(void) {
	unsigned char m, J;
	unsigned int i, k;
	unsigned int Tk;
	unsigned int m_bNx2;
	unsigned int m_bNx4;

	m_bNx2 = m_bN * 2; // Interleaved Y1 offset
	m_bNx4 = m_bN * 4; // Interleaved W1 offset

	// CTC_SubblockInterleaver_Parameter Table 505
	m = sCTC_SubblockInterleaver_Parameter_noHARQ[(unsigned char)m_nInterleaver_Index].nm;
	J = sCTC_SubblockInterleaver_Parameter_noHARQ[(unsigned char)m_nInterleaver_Index].nJ;

	// 8.4.9.2.3.4.2 Subblock Interleaving
	i = 0;
	k = 0;

	for(i=0;i<m_bN;i++)
	{
		Tk = m_bN; // initial
		while(Tk >= m_bN)
		{
			// range of k : 0 ~ (N-1) bits
			Tk = (1 << m) * (k % J) + BRO(Floor(((float)k)/J), m); // Formula c
			k++;
		}

		// Interleave separately

		// No Bit Grouping for systematic bits
		A[Tk] = PaddedEncodedBlock [i];
		B[Tk] = PaddedEncodedBlock [m_bN+i];

		// Bit Grouping 8.4.9.2.3.4.3
		Y1[Tk] = PaddedEncodedBlock[m_bNx2 + (2*i)];
		Y2[Tk] = PaddedEncodedBlock[m_bNx2 + (2*i+1)];

		W1[Tk] = PaddedEncodedBlock[m_bNx4 + (2*i)];
		W2[Tk] = PaddedEncodedBlock[m_bNx4 + (2*i+1)];
	}
}

void FECDecoder::CalcBranchMetric(char * AA, char * BB, char * Y, char * W) {
	unsigned int Index;
	int EXT_A_B[CTCINPUTNUM];

	//Branch Metric
	for(Index=0;Index<m_bN;Index++)
	{
        // Extrinsic Info + Symbol LLR(Systematic bits)
		// Relative LLR wrt symbol(00)
		EXT_A_B[0] = EXT[Index][0]                          ;// 00 - A[Index] - B[Index]
		EXT_A_B[1] = EXT[Index][1]             + BB[Index]  ;// 01 - A[Index]
		EXT_A_B[2] = EXT[Index][2] + AA[Index]		    	;// 10            - B[Index]
		EXT_A_B[3] = EXT[Index][3] + AA[Index] + BB[Index]  ;// 11


		// Output bits LLR(Parity bits)

		//                                correlation of Parity bits(output)
		// State 0	   							  				// A B  Y W
		BM[Index][0][0]  = EXT_A_B[0]                		; 	// 0 0  0 0
		BM[Index][0][1]  = EXT_A_B[1] + Y[Index] + W[Index]; 	// 0 1  1 1
		BM[Index][0][2]  = EXT_A_B[2] + Y[Index] + W[Index]; 	// 1 0  1 1
		BM[Index][0][3]  = EXT_A_B[3]                  		; 	// 1 1  0 0

		// State 1	   							  				// A B  Y W
		BM[Index][1][0]  = EXT_A_B[0]                		; 	// 0 0  0 0
		BM[Index][1][1]  = EXT_A_B[1] + Y[Index] + W[Index]; 	// 0 1  1 1
		BM[Index][1][2]  = EXT_A_B[2] + Y[Index] + W[Index]; 	// 1 0  1 1
		BM[Index][1][3]  = EXT_A_B[3]                		; 	// 1 1  0 0

		// State 2	   							  				// A B  Y W
		BM[Index][2][0]  = EXT_A_B[0] + Y[Index]        	; 	// 0 0  1 0 1
		BM[Index][2][1]  = EXT_A_B[1]         	  + W[Index]; 	// 0 1  0 1 6
		BM[Index][2][2]  = EXT_A_B[2]             + W[Index]; 	// 1 0  0 1 5
		BM[Index][2][3]  = EXT_A_B[3] + Y[Index]        	; 	// 1 1  1 0 2

		// State 3	   							  				// A B  Y W
		BM[Index][3][0]  = EXT_A_B[0] + Y[Index]        	; 	// 0 0  1 0 5
		BM[Index][3][1]  = EXT_A_B[1]             + W[Index]; 	// 0 1  0 1 2
		BM[Index][3][2]  = EXT_A_B[2]             + W[Index]; 	// 1 0  0 1 1
		BM[Index][3][3]  = EXT_A_B[3] + Y[Index]        	; 	// 1 1  1 0 6

		// State 4	   							  				// A B  Y W
		BM[Index][4][0]  = EXT_A_B[0] + Y[Index] + W[Index]; 	// 0 0  1 1
		BM[Index][4][1]  = EXT_A_B[1]                		; 	// 0 1  0 0
		BM[Index][4][2]  = EXT_A_B[2]                		; 	// 1 0  0 0
		BM[Index][4][3]  = EXT_A_B[3] + Y[Index] + W[Index]; 	// 1 1  1 1

		// State 5	   							  				// A B  Y W
		BM[Index][5][0]  = EXT_A_B[0] + Y[Index] + W[Index]; 	// 0 0  1 1
		BM[Index][5][1]  = EXT_A_B[1]                		; 	// 0 1  0 0
		BM[Index][5][2]  = EXT_A_B[2]                		; 	// 1 0  0 0
		BM[Index][5][3]  = EXT_A_B[3] + Y[Index] + W[Index]; 	// 1 1  1 1

		// State 6	   							  				// A B  Y W
		BM[Index][6][0]  = EXT_A_B[0]         	  + W[Index]; 	// 0 0  0 1
		BM[Index][6][1]  = EXT_A_B[1] + Y[Index]        	; 	// 0 1  1 0
		BM[Index][6][2]  = EXT_A_B[2] + Y[Index]        	; 	// 1 0  1 0
		BM[Index][6][3]  = EXT_A_B[3]        	  + W[Index]; 	// 1 1  0 1

		// State 7	   							  				// A B  Y W
		BM[Index][7][0]  = EXT_A_B[0]        	  + W[Index]; 	// 0 0  0 1
		BM[Index][7][1]  = EXT_A_B[1] + Y[Index]        	; 	// 0 1  1 0
		BM[Index][7][2]  = EXT_A_B[2] + Y[Index]        	; 	// 1 0  1 0
		BM[Index][7][3]  = EXT_A_B[3]        	  + W[Index]; 	// 1 1  0 1
	}
/*
	cout << endl;
	cout << "BM" << endl;

	for(Index=0;Index<m_bN;Index++)
	{
		for(unsigned i=0;i<8;i++)
		{
			for(unsigned j=0;j<4;j++)

			{
				cout << BM[Index][i][j] << " ";
			}

			cout << endl;
		}
		cout << endl;
	}
*/
}

void FECDecoder::CalcAlpha(void) {

	// Forward Metric Calculation

	unsigned int Index;
	int AlphaAtState[4];
	unsigned char MaxIndex;
	int AlphaMax;

	//Branch Metric
	for(Index=0;Index<m_bN;Index++)
	{
		// State [0]
		AlphaAtState[0] = Alpha[Index][ LastState[0][0] ] + BM[Index][ LastState[0][0] ][0];
		AlphaAtState[1] = Alpha[Index][ LastState[0][1] ] + BM[Index][ LastState[0][1] ][1];
		AlphaAtState[2] = Alpha[Index][ LastState[0][2] ] + BM[Index][ LastState[0][2] ][2];
		AlphaAtState[3] = Alpha[Index][ LastState[0][3] ] + BM[Index][ LastState[0][3] ][3];

		// Next Node
		Alpha[Index+1][0] = AlphaAtState[Max(AlphaAtState, CTCINPUTNUM)];
//		cout << AlphaAtState[0] << " "<< AlphaAtState[1] << " "<< AlphaAtState[2] << " "<< AlphaAtState[3] << endl;

		// State [1]
		AlphaAtState[0] = Alpha[Index][ LastState[1][0] ] + BM[Index][ LastState[1][0] ][0];
		AlphaAtState[1] = Alpha[Index][ LastState[1][1] ] + BM[Index][ LastState[1][1] ][1];
		AlphaAtState[2] = Alpha[Index][ LastState[1][2] ] + BM[Index][ LastState[1][2] ][2];
		AlphaAtState[3] = Alpha[Index][ LastState[1][3] ] + BM[Index][ LastState[1][3] ][3];

		// Next Node
		Alpha[Index+1][1] = AlphaAtState[Max(AlphaAtState, CTCINPUTNUM)];
//		cout << AlphaAtState[0] << " "<< AlphaAtState[1] << " "<< AlphaAtState[2] << " "<< AlphaAtState[3] << endl;

		// State [2]
		AlphaAtState[0] = Alpha[Index][ LastState[2][0] ] + BM[Index][ LastState[2][0] ][0];
		AlphaAtState[1] = Alpha[Index][ LastState[2][1] ] + BM[Index][ LastState[2][1] ][1];
		AlphaAtState[2] = Alpha[Index][ LastState[2][2] ] + BM[Index][ LastState[2][2] ][2];
		AlphaAtState[3] = Alpha[Index][ LastState[2][3] ] + BM[Index][ LastState[2][3] ][3];

		// Next Node
		Alpha[Index+1][2] = AlphaAtState[Max(AlphaAtState, CTCINPUTNUM)];
//		cout << AlphaAtState[0] << " "<< AlphaAtState[1] << " "<< AlphaAtState[2] << " "<< AlphaAtState[3] << endl;

		// State [3]
		AlphaAtState[0] = Alpha[Index][ LastState[3][0] ] + BM[Index][ LastState[3][0] ][0];
		AlphaAtState[1] = Alpha[Index][ LastState[3][1] ] + BM[Index][ LastState[3][1] ][1];
		AlphaAtState[2] = Alpha[Index][ LastState[3][2] ] + BM[Index][ LastState[3][2] ][2];
		AlphaAtState[3] = Alpha[Index][ LastState[3][3] ] + BM[Index][ LastState[3][3] ][3];

		// Next Node
		Alpha[Index+1][3] = AlphaAtState[Max(AlphaAtState, CTCINPUTNUM)];
//		cout << AlphaAtState[0] << " "<< AlphaAtState[1] << " "<< AlphaAtState[2] << " "<< AlphaAtState[3] << endl;

		// State [4]
		AlphaAtState[0] = Alpha[Index][ LastState[4][0] ] + BM[Index][ LastState[4][0] ][0];
		AlphaAtState[1] = Alpha[Index][ LastState[4][1] ] + BM[Index][ LastState[4][1] ][1];
		AlphaAtState[2] = Alpha[Index][ LastState[4][2] ] + BM[Index][ LastState[4][2] ][2];
		AlphaAtState[3] = Alpha[Index][ LastState[4][3] ] + BM[Index][ LastState[4][3] ][3];

		// Next Node
		Alpha[Index+1][4] = AlphaAtState[Max(AlphaAtState, CTCINPUTNUM)];
//		cout << AlphaAtState[0] << " "<< AlphaAtState[1] << " "<< AlphaAtState[2] << " "<< AlphaAtState[3] << endl;

		// State [5]
		AlphaAtState[0] = Alpha[Index][ LastState[5][0] ] + BM[Index][ LastState[5][0] ][0];
		AlphaAtState[1] = Alpha[Index][ LastState[5][1] ] + BM[Index][ LastState[5][1] ][1];
		AlphaAtState[2] = Alpha[Index][ LastState[5][2] ] + BM[Index][ LastState[5][2] ][2];
		AlphaAtState[3] = Alpha[Index][ LastState[5][3] ] + BM[Index][ LastState[5][3] ][3];

		// Next Node
		Alpha[Index+1][5] = AlphaAtState[Max(AlphaAtState, CTCINPUTNUM)];
//		cout << AlphaAtState[0] << " "<< AlphaAtState[1] << " "<< AlphaAtState[2] << " "<< AlphaAtState[3] << endl;


		// State [6]
		AlphaAtState[0] = Alpha[Index][ LastState[6][0] ] + BM[Index][ LastState[6][0] ][0];
		AlphaAtState[1] = Alpha[Index][ LastState[6][1] ] + BM[Index][ LastState[6][1] ][1];
		AlphaAtState[2] = Alpha[Index][ LastState[6][2] ] + BM[Index][ LastState[6][2] ][2];
		AlphaAtState[3] = Alpha[Index][ LastState[6][3] ] + BM[Index][ LastState[6][3] ][3];

		// Next Node
		Alpha[Index+1][6] = AlphaAtState[Max(AlphaAtState, CTCINPUTNUM)];
//		cout << AlphaAtState[0] << " "<< AlphaAtState[1] << " "<< AlphaAtState[2] << " "<< AlphaAtState[3] << endl;

		// State [7]
		AlphaAtState[0] = Alpha[Index][ LastState[7][0] ] + BM[Index][ LastState[7][0] ][0];
		AlphaAtState[1] = Alpha[Index][ LastState[7][1] ] + BM[Index][ LastState[7][1] ][1];
		AlphaAtState[2] = Alpha[Index][ LastState[7][2] ] + BM[Index][ LastState[7][2] ][2];
		AlphaAtState[3] = Alpha[Index][ LastState[7][3] ] + BM[Index][ LastState[7][3] ][3];

		// Next Node
		Alpha[Index+1][7] = AlphaAtState[Max(AlphaAtState, CTCINPUTNUM)];
//		cout << AlphaAtState[0] << " "<< AlphaAtState[1] << " "<< AlphaAtState[2] << " "<< AlphaAtState[3] << endl;

	    // Get Max
		MaxIndex = Max(Alpha[Index+1], CTCSTATENUM);
		AlphaMax = Alpha[Index+1][MaxIndex];

//		cout << Alpha[Index+1][MaxIndex] << " " ;
/*
		cout << Alpha[Index+1][0] << " "<< Alpha[Index+1][1] << " "<< Alpha[Index+1][2] << " "<< Alpha[Index+1][3]
			 << Alpha[Index+1][4] << " "<< Alpha[Index+1][5] << " "<< Alpha[Index+1][6] << " "<< Alpha[Index+1][7] << endl;
*/

		// Normalize (prohibit overflow)
		Alpha[Index+1][0] = Alpha[Index+1][0] - AlphaMax;
		Alpha[Index+1][1] = Alpha[Index+1][1] - AlphaMax;
		Alpha[Index+1][2] = Alpha[Index+1][2] - AlphaMax;
		Alpha[Index+1][3] = Alpha[Index+1][3] - AlphaMax;
		Alpha[Index+1][4] = Alpha[Index+1][4] - AlphaMax;
		Alpha[Index+1][5] = Alpha[Index+1][5] - AlphaMax;
		Alpha[Index+1][6] = Alpha[Index+1][6] - AlphaMax;
		Alpha[Index+1][7] = Alpha[Index+1][7] - AlphaMax;

//		cout << Alpha[Index+1][0] << " "<< Alpha[Index+1][1] << " "<< Alpha[Index+1][2] << " "<< Alpha[Index+1][3] << " "
//			 << Alpha[Index+1][4] << " "<< Alpha[Index+1][5] << " "<< Alpha[Index+1][6] << " "<< Alpha[Index+1][7] << endl;

	}
	Alpha[0][0] 	= Alpha[m_bN][0];
	Alpha[0][1] 	= Alpha[m_bN][1];
	Alpha[0][2] 	= Alpha[m_bN][2];
	Alpha[0][3] 	= Alpha[m_bN][3];
	Alpha[0][4] 	= Alpha[m_bN][4];
	Alpha[0][5] 	= Alpha[m_bN][5];
	Alpha[0][6] 	= Alpha[m_bN][6];
	Alpha[0][7] 	= Alpha[m_bN][7];
}

void FECDecoder::CalcBeta(void) {

	// Backward Metric Calculation

	unsigned int Index;
	unsigned int Index_1;
	int BetaAtState[4];
	unsigned char MaxIndex;
	int BetaMax;

	//Branch Metric
	for(Index=m_bN;Index>0;Index--)
	{
		Index_1 = Index - 1;

		// State [0]
		BetaAtState[0] = Beta[Index][ NextState[0][0] ] + BM[Index_1][0][0];
		BetaAtState[1] = Beta[Index][ NextState[0][1] ] + BM[Index_1][0][1];
		BetaAtState[2] = Beta[Index][ NextState[0][2] ] + BM[Index_1][0][2];
		BetaAtState[3] = Beta[Index][ NextState[0][3] ] + BM[Index_1][0][3];

		// Next Node
		Beta[Index_1][0] = BetaAtState[Max(BetaAtState, CTCINPUTNUM)];

		// State [1]
		BetaAtState[0] = Beta[Index][ NextState[1][0] ] + BM[Index_1][1][0];
		BetaAtState[1] = Beta[Index][ NextState[1][1] ] + BM[Index_1][1][1];
		BetaAtState[2] = Beta[Index][ NextState[1][2] ] + BM[Index_1][1][2];
		BetaAtState[3] = Beta[Index][ NextState[1][3] ] + BM[Index_1][1][3];

		// Next Node
		Beta[Index_1][1] = BetaAtState[Max(BetaAtState, CTCINPUTNUM)];

		// State [2]
		BetaAtState[0] = Beta[Index][ NextState[2][0] ] + BM[Index_1][2][0];
		BetaAtState[1] = Beta[Index][ NextState[2][1] ] + BM[Index_1][2][1];
		BetaAtState[2] = Beta[Index][ NextState[2][2] ] + BM[Index_1][2][2];
		BetaAtState[3] = Beta[Index][ NextState[2][3] ] + BM[Index_1][2][3];

		// Next Node
		Beta[Index_1][2] = BetaAtState[Max(BetaAtState, CTCINPUTNUM)];

		// State [3]
		BetaAtState[0] = Beta[Index][ NextState[3][0] ] + BM[Index_1][3][0];
		BetaAtState[1] = Beta[Index][ NextState[3][1] ] + BM[Index_1][3][1];
		BetaAtState[2] = Beta[Index][ NextState[3][2] ] + BM[Index_1][3][2];
		BetaAtState[3] = Beta[Index][ NextState[3][3] ] + BM[Index_1][3][3];

		// Next Node
		Beta[Index_1][3] = BetaAtState[Max(BetaAtState, CTCINPUTNUM)];

		// State [4]
		BetaAtState[0] = Beta[Index][ NextState[4][0] ] + BM[Index_1][4][0];
		BetaAtState[1] = Beta[Index][ NextState[4][1] ] + BM[Index_1][4][1];
		BetaAtState[2] = Beta[Index][ NextState[4][2] ] + BM[Index_1][4][2];
		BetaAtState[3] = Beta[Index][ NextState[4][3] ] + BM[Index_1][4][3];

		// Next Node
		Beta[Index_1][4] = BetaAtState[Max(BetaAtState, CTCINPUTNUM)];

		// State [5]
		BetaAtState[0] = Beta[Index][ NextState[5][0] ] + BM[Index_1][5][0];
		BetaAtState[1] = Beta[Index][ NextState[5][1] ] + BM[Index_1][5][1];
		BetaAtState[2] = Beta[Index][ NextState[5][2] ] + BM[Index_1][5][2];
		BetaAtState[3] = Beta[Index][ NextState[5][3] ] + BM[Index_1][5][3];

		// Next Node
		Beta[Index_1][5] = BetaAtState[Max(BetaAtState, CTCINPUTNUM)];

		// State [6]
		BetaAtState[0] = Beta[Index][ NextState[6][0] ] + BM[Index_1][6][0];
		BetaAtState[1] = Beta[Index][ NextState[6][1] ] + BM[Index_1][6][1];
		BetaAtState[2] = Beta[Index][ NextState[6][2] ] + BM[Index_1][6][2];
		BetaAtState[3] = Beta[Index][ NextState[6][3] ] + BM[Index_1][6][3];

		// Next Node
		Beta[Index_1][6] = BetaAtState[Max(BetaAtState, CTCINPUTNUM)];

		// State [7]
		BetaAtState[0] = Beta[Index][ NextState[7][0] ] + BM[Index_1][7][0];
		BetaAtState[1] = Beta[Index][ NextState[7][1] ] + BM[Index_1][7][1];
		BetaAtState[2] = Beta[Index][ NextState[7][2] ] + BM[Index_1][7][2];
		BetaAtState[3] = Beta[Index][ NextState[7][3] ] + BM[Index_1][7][3];

		// Next Node
		Beta[Index_1][7] = BetaAtState[Max(BetaAtState, CTCINPUTNUM)];

	    // Get Max
		MaxIndex = Max(Beta[Index_1], CTCSTATENUM);
		BetaMax = Beta[Index_1][MaxIndex];

		// Normalize (prohibit overflow)
		Beta[Index_1][0] = Beta[Index_1][0] - BetaMax;
		Beta[Index_1][1] = Beta[Index_1][1] - BetaMax;
		Beta[Index_1][2] = Beta[Index_1][2] - BetaMax;
		Beta[Index_1][3] = Beta[Index_1][3] - BetaMax;
		Beta[Index_1][4] = Beta[Index_1][4] - BetaMax;
		Beta[Index_1][5] = Beta[Index_1][5] - BetaMax;
		Beta[Index_1][6] = Beta[Index_1][6] - BetaMax;
		Beta[Index_1][7] = Beta[Index_1][7] - BetaMax;

	}
	Beta[m_bN][0] 	= Beta[0][0];
	Beta[m_bN][1] 	= Beta[0][1];
	Beta[m_bN][2] 	= Beta[0][2];
	Beta[m_bN][3] 	= Beta[0][3];
	Beta[m_bN][4] 	= Beta[0][4];
	Beta[m_bN][5] 	= Beta[0][5];
	Beta[m_bN][6] 	= Beta[0][6];
	Beta[m_bN][7] 	= Beta[0][7];
}

void FECDecoder::SetInitialState(unsigned char Index) {

	// Circular Tail Biting
	Alpha[0][0] 	= MININF;
	Alpha[0][1] 	= MININF;
	Alpha[0][2] 	= MININF;
	Alpha[0][3] 	= MININF;
	Alpha[0][4] 	= MININF;
	Alpha[0][5] 	= MININF;
	Alpha[0][6] 	= MININF;
	Alpha[0][7] 	= MININF;

	// Circular Tail Biting
	Beta[m_bN][0] 	= MININF;
	Beta[m_bN][1] 	= MININF;
	Beta[m_bN][2] 	= MININF;
	Beta[m_bN][3] 	= MININF;
	Beta[m_bN][4] 	= MININF;
	Beta[m_bN][5] 	= MININF;
	Beta[m_bN][6] 	= MININF;
	Beta[m_bN][7] 	= MININF;

	// Sc (max probability = 0)
	Alpha[0][Index] 	= 0;
	Beta[m_bN][Index] 	= 0;
}


unsigned char FECDecoder::EstCircularState(void) {
	int         EstSc  [CTCSTATENUM  ];
	unsigned char MaxIndex;

	// probability of all state
	EstSc[0] = Alpha[m_bN][0] 	+ Beta[0][0];
	EstSc[1] = Alpha[m_bN][1] 	+ Beta[0][1];
	EstSc[2] = Alpha[m_bN][2] 	+ Beta[0][2];
	EstSc[3] = Alpha[m_bN][3] 	+ Beta[0][3];
	EstSc[4] = Alpha[m_bN][4] 	+ Beta[0][4];
	EstSc[5] = Alpha[m_bN][5] 	+ Beta[0][5];
	EstSc[6] = Alpha[m_bN][6] 	+ Beta[0][6];
	EstSc[7] = Alpha[m_bN][7] 	+ Beta[0][7];

	// Estimation of Sc
	MaxIndex = Max(EstSc, CTCSTATENUM);

/*
	for(unsigned i=0;i<8;i++)
	{
		cout << Alpha[m_bN][i] << " ";
	}
	cout << endl;
	for(unsigned i=0;i<8;i++)
	{
		cout << Beta[0][i] << " ";
	}
	cout << endl;
	for(unsigned i=0;i<8;i++)
	{
		cout << EstSc[i] << " ";
	}
	cout << endl;
*/
	return MaxIndex;
}

void FECDecoder::CalcAPP(void) {

	// App Calculation

	unsigned int Index;
	unsigned char MaxIndex;
	unsigned int Index_1;
	int APPAtState[CTCSTATENUM];
	int APPMax;

	unsigned char APPState[4];

	// APP
	for(Index=1;Index<(m_bN+1);Index++)
	{
		Index_1 = Index - 1;

		// INPUT 00
		APPAtState[0] = Alpha[Index_1][0] + BM[Index_1][0][0] + Beta[Index][ NextState[0][0] ];
		APPAtState[1] = Alpha[Index_1][1] + BM[Index_1][1][0] + Beta[Index][ NextState[1][0] ];
		APPAtState[2] = Alpha[Index_1][2] + BM[Index_1][2][0] + Beta[Index][ NextState[2][0] ];
		APPAtState[3] = Alpha[Index_1][3] + BM[Index_1][3][0] + Beta[Index][ NextState[3][0] ];
		APPAtState[4] = Alpha[Index_1][4] + BM[Index_1][4][0] + Beta[Index][ NextState[4][0] ];
		APPAtState[5] = Alpha[Index_1][5] + BM[Index_1][5][0] + Beta[Index][ NextState[5][0] ];
		APPAtState[6] = Alpha[Index_1][6] + BM[Index_1][6][0] + Beta[Index][ NextState[6][0] ];
		APPAtState[7] = Alpha[Index_1][7] + BM[Index_1][7][0] + Beta[Index][ NextState[7][0] ];

/*		cout << "Index : " << Index << endl;
		cout << Alpha[Index_1][0] << " " << BM[Index_1][0][0] << " " << Beta[Index][ NextState[0][0] ] << " " << NextState[0][0] << endl;
		cout << APPAtState[0] << " " << APPAtState[1] << " " << APPAtState[2] << " " << APPAtState[3] << " "
			 <<	APPAtState[4] << " " << APPAtState[5] << " " << APPAtState[6] << " " << APPAtState[7] ;
		cout << endl;
*/
		// Next Node
		APP[Index_1][0] = APPAtState[Max(APPAtState, CTCSTATENUM)];
		APPState[0] = Max(APPAtState, CTCSTATENUM);

		// INPUT 01
		APPAtState[0] = Alpha[Index_1][0] + BM[Index_1][0][1] + Beta[Index][ NextState[0][1] ];
		APPAtState[1] = Alpha[Index_1][1] + BM[Index_1][1][1] + Beta[Index][ NextState[1][1] ];
		APPAtState[2] = Alpha[Index_1][2] + BM[Index_1][2][1] + Beta[Index][ NextState[2][1] ];
		APPAtState[3] = Alpha[Index_1][3] + BM[Index_1][3][1] + Beta[Index][ NextState[3][1] ];
		APPAtState[4] = Alpha[Index_1][4] + BM[Index_1][4][1] + Beta[Index][ NextState[4][1] ];
		APPAtState[5] = Alpha[Index_1][5] + BM[Index_1][5][1] + Beta[Index][ NextState[5][1] ];
		APPAtState[6] = Alpha[Index_1][6] + BM[Index_1][6][1] + Beta[Index][ NextState[6][1] ];
		APPAtState[7] = Alpha[Index_1][7] + BM[Index_1][7][1] + Beta[Index][ NextState[7][1] ];

/*		cout << APPAtState[0] << " " << APPAtState[1] << " " << APPAtState[2] << " " << APPAtState[3] << " "
			 <<	APPAtState[4] << " " << APPAtState[5] << " " << APPAtState[6] << " " << APPAtState[7] ;
		cout << endl;
*/
		// Next Node
		APP[Index_1][1] = APPAtState[Max(APPAtState, CTCSTATENUM)];
		APPState[1] = Max(APPAtState, CTCSTATENUM);

		// INPUT 10
		APPAtState[0] = Alpha[Index_1][0] + BM[Index_1][0][2] + Beta[Index][ NextState[0][2] ];
		APPAtState[1] = Alpha[Index_1][1] + BM[Index_1][1][2] + Beta[Index][ NextState[1][2] ];
		APPAtState[2] = Alpha[Index_1][2] + BM[Index_1][2][2] + Beta[Index][ NextState[2][2] ];
		APPAtState[3] = Alpha[Index_1][3] + BM[Index_1][3][2] + Beta[Index][ NextState[3][2] ];
		APPAtState[4] = Alpha[Index_1][4] + BM[Index_1][4][2] + Beta[Index][ NextState[4][2] ];
		APPAtState[5] = Alpha[Index_1][5] + BM[Index_1][5][2] + Beta[Index][ NextState[5][2] ];
		APPAtState[6] = Alpha[Index_1][6] + BM[Index_1][6][2] + Beta[Index][ NextState[6][2] ];
		APPAtState[7] = Alpha[Index_1][7] + BM[Index_1][7][2] + Beta[Index][ NextState[7][2] ];
/*
		cout << APPAtState[0] << " " << APPAtState[1] << " " << APPAtState[2] << " " << APPAtState[3] << " "
			 <<	APPAtState[4] << " " << APPAtState[5] << " " << APPAtState[6] << " " << APPAtState[7] ;
		cout << endl;
*/
		// Next Node
		APP[Index_1][2] = APPAtState[Max(APPAtState, CTCSTATENUM)];
		APPState[2] = Max(APPAtState, CTCSTATENUM);

		// INPUT 11
		APPAtState[0] = Alpha[Index_1][0] + BM[Index_1][0][3] + Beta[Index][ NextState[0][3] ];
		APPAtState[1] = Alpha[Index_1][1] + BM[Index_1][1][3] + Beta[Index][ NextState[1][3] ];
		APPAtState[2] = Alpha[Index_1][2] + BM[Index_1][2][3] + Beta[Index][ NextState[2][3] ];
		APPAtState[3] = Alpha[Index_1][3] + BM[Index_1][3][3] + Beta[Index][ NextState[3][3] ];
		APPAtState[4] = Alpha[Index_1][4] + BM[Index_1][4][3] + Beta[Index][ NextState[4][3] ];
		APPAtState[5] = Alpha[Index_1][5] + BM[Index_1][5][3] + Beta[Index][ NextState[5][3] ];
		APPAtState[6] = Alpha[Index_1][6] + BM[Index_1][6][3] + Beta[Index][ NextState[6][3] ];
		APPAtState[7] = Alpha[Index_1][7] + BM[Index_1][7][3] + Beta[Index][ NextState[7][3] ];
/*
		cout << APPAtState[0] << " " << APPAtState[1] << " " << APPAtState[2] << " " << APPAtState[3] << " "
			 <<	APPAtState[4] << " " << APPAtState[5] << " " << APPAtState[6] << " " << APPAtState[7] ;
		cout << endl;
*/
		// Next Node
		APP[Index_1][3] = APPAtState[Max(APPAtState, CTCSTATENUM)];
		APPState[3] = Max(APPAtState, CTCSTATENUM);

		// Get Max
		MaxIndex = Max(APP[Index_1], CTCINPUTNUM);
		APPMax = APP[Index_1][MaxIndex];

//		cout << (unsigned int) APPState[MaxIndex] << " ";

//		cout <<(unsigned int)MaxIndex << " " ;
//		cout << APP[Index_1][0] << " " << APP[Index_1][1] << " " << APP[Index_1][2] << " " << APP[Index_1][3] << endl;

		// Normalize (prohibit overflow)
		APP[Index_1][0] = APP[Index_1][0] - APPMax;
		APP[Index_1][1] = APP[Index_1][1] - APPMax;
		APP[Index_1][2] = APP[Index_1][2] - APPMax;
		APP[Index_1][3] = APP[Index_1][3] - APPMax;

	}
}

void FECDecoder::ExtrinsicScaling(char * AA, char * BB) {

	unsigned int Index;

//	unsigned int MaxIndex;
//	cout << "EXt " << endl;

	// 0.75
	for(Index=0;Index<m_bN;Index++)
	{
/*
		EXTi[Index][0] = 0;
		EXTi[Index][1] = Floor( ((float)(APP[Index][1]-APP[Index][0]-EXT[Index][1]		    -BB[Index])) * 0.75 );
		EXTi[Index][2] = Floor( ((float)(APP[Index][2]-APP[Index][0]-EXT[Index][2]-AA[Index]		  )) * 0.75 );
		EXTi[Index][3] = Floor( ((float)(APP[Index][3]-APP[Index][0]-EXT[Index][3]-AA[Index]-BB[Index])) * 0.75 );
*/

		EXTi[Index][0] = 0;
		EXTi[Index][1] = Floor( ((float)(APP[Index][1]-APP[Index][0]-EXT[Index][1]		    )) * 0.75 );
		EXTi[Index][2] = Floor( ((float)(APP[Index][2]-APP[Index][0]-EXT[Index][2]		  )) * 0.75 );
		EXTi[Index][3] = Floor( ((float)(APP[Index][3]-APP[Index][0]-EXT[Index][3])) * 0.75 );
/*

		EXTi[Index][0] = Floor( ((float)(APP[Index][0]-EXT[Index][0])) * 0.75 );
		EXTi[Index][1] = Floor( ((float)(APP[Index][1]-EXT[Index][1])) * 0.75 );
		EXTi[Index][2] = Floor( ((float)(APP[Index][2]-EXT[Index][2])) * 0.75 );
		EXTi[Index][3] = Floor( ((float)(APP[Index][3]-EXT[Index][3])) * 0.75 );
*/
		//		MaxIndex = Max(EXTi[Index], CTCINPUTNUM);
//		cout << MaxIndex << " " ;

		/*
		cout << Index << endl;
		cout << APP[Index][0] << " " << APP[Index][1] << " " << APP[Index][2] << " " << APP[Index][3] << endl;
		cout << EXT[Index][0] << " " << EXT[Index][1] << " " << EXT[Index][2] << " " << EXT[Index][3] << endl;
		cout << EXTi[Index][0] << " " << EXTi[Index][1] << " " << EXTi[Index][2] << " " << EXTi[Index][3] << endl;
*/
	}
}

void FECDecoder::InterleavingSystematic(void) {

	unsigned int Index;
	unsigned int Index_mul4;

	unsigned int N_div4;

	unsigned int Pj0;
	unsigned int Pj1;
	unsigned int Pj2;
	unsigned int Pj3;

	N_div4 = m_bN >> 2; // N/2

	if((m_eHARQ == NO)||(m_eHARQ == CC))
	{
		//	m_N  = CTC_ChannelCoding_per_Modulation_502_noHARQ[(unsigned char)m_Interleaver_Index].N;
		P0 = sCTC_ChannelCoding_per_Modulation_noHARQ502[(unsigned char)m_nInterleaver_Index].bP0;
		P1 = sCTC_ChannelCoding_per_Modulation_noHARQ502[(unsigned char)m_nInterleaver_Index].bP1;
		P2 = sCTC_ChannelCoding_per_Modulation_noHARQ502[(unsigned char)m_nInterleaver_Index].bP2;
		P3 = sCTC_ChannelCoding_per_Modulation_noHARQ502[(unsigned char)m_nInterleaver_Index].bP3;

//		m_EncodedDataBlockSize = sCTC_ChannelCoding_per_Modulation_noHARQ502[(unsigned char)//m_Interleaver_Index].EncodedDataBlockSize;
	}else if(m_eHARQ == IR){
		//	m_N  = CTC_ChannelCoding_per_Modulation_502_noHARQ[(unsigned char)m_Interleaver_Index].N;
		P0 = sCTC_ChannelCoding_per_Modulation_HARQ503[(unsigned char)m_nInterleaver_Index].bP0;
		P1 = sCTC_ChannelCoding_per_Modulation_HARQ503[(unsigned char)m_nInterleaver_Index].bP1;
		P2 = sCTC_ChannelCoding_per_Modulation_HARQ503[(unsigned char)m_nInterleaver_Index].bP2;
		P3 = sCTC_ChannelCoding_per_Modulation_HARQ503[(unsigned char)m_nInterleaver_Index].bP3;

//		m_EncodedDataBlockSize = 6 * m_Nsch * m_MODORDER; // Lk @ BitSelection byte
	}

//	cout << "Interleaving" << endl;

	// Step 1&2. P(j)
	for(Index=0;Index<N_div4;Index++)
	{
		Index_mul4 = 4 * Index;

		Pj0 = (P0 * (Index_mul4    ) + 1              ) % m_bN; // odd
		Pj1 = (P0 * (Index_mul4 + 1) + 1 + m_bN/2 + P1) % m_bN; // even
		Pj2 = (P0 * (Index_mul4 + 2) + 1          + P2) % m_bN; // even
		Pj3 = (P0 * (Index_mul4 + 3) + 1 + m_bN/2 + P3) % m_bN; // even

		if((Pj0 % 2)==0)
		{
			// 0, 2, 4, 6 ...
			Ai[Index_mul4  ] = A[Pj0];
			Bi[Index_mul4  ] = B[Pj0];
		}else{
			// 1, 3, 5, 7 ...
			Ai[Index_mul4  ] = B[Pj0];
			Bi[Index_mul4  ] = A[Pj0];
		}

		if((Pj1 % 2)==0)
		{
			// 0, 2, 4, 6 ...
			Ai[Index_mul4+1] = A[Pj1];
			Bi[Index_mul4+1] = B[Pj1];
		}else{
			// 1, 3, 5, 7 ...
			Ai[Index_mul4+1] = B[Pj1];
			Bi[Index_mul4+1] = A[Pj1];
		}

		if((Pj2 % 2)==0)
		{
			// 0, 2, 4, 6 ...
			Ai[Index_mul4+2] = A[Pj2];
			Bi[Index_mul4+2] = B[Pj2];
		}else{
			// 1, 3, 5, 7 ...
			Ai[Index_mul4+2] = B[Pj2];
			Bi[Index_mul4+2] = A[Pj2];
		}

		if((Pj3 % 2)==0)
		{
			// 0, 2, 4, 6 ...
			Ai[Index_mul4+3] = A[Pj3];
			Bi[Index_mul4+3] = B[Pj3];
		}else{
			// 1, 3, 5, 7 ...
			Ai[Index_mul4+3] = B[Pj3];
			Bi[Index_mul4+3] = A[Pj3];
		}
	}
}

void FECDecoder::Interleaving(void) {

	unsigned int Index;
	int Temp;
	unsigned int Index_mul4;

//	unsigned int N_div2;
	unsigned int N_div4;

	unsigned int Pj0;
	unsigned int Pj1;
	unsigned int Pj2;
	unsigned int Pj3;

	unsigned int P0;
	unsigned int P1;
	unsigned int P2;
	unsigned int P3;


//	N_div2 = m_bN >> 1; // N
	N_div4 = m_bN >> 2; // N/2

	if((m_eHARQ == NO)||(m_eHARQ == CC))
	{
		//	m_N  = CTC_ChannelCoding_per_Modulation_502_noHARQ[(unsigned char)m_Interleaver_Index].N;
		P0 = sCTC_ChannelCoding_per_Modulation_noHARQ502[(unsigned char)m_nInterleaver_Index].bP0;
		P1 = sCTC_ChannelCoding_per_Modulation_noHARQ502[(unsigned char)m_nInterleaver_Index].bP1;
		P2 = sCTC_ChannelCoding_per_Modulation_noHARQ502[(unsigned char)m_nInterleaver_Index].bP2;
		P3 = sCTC_ChannelCoding_per_Modulation_noHARQ502[(unsigned char)m_nInterleaver_Index].bP3;

//		m_EncodedDataBlockSize = sCTC_ChannelCoding_per_Modulation_noHARQ502[(unsigned char)//m_Interleaver_Index].EncodedDataBlockSize;
	}else if(m_eHARQ == IR){
		//	m_N  = CTC_ChannelCoding_per_Modulation_502_noHARQ[(unsigned char)m_Interleaver_Index].N;
		P0 = sCTC_ChannelCoding_per_Modulation_HARQ503[(unsigned char)m_nInterleaver_Index].bP0;
		P1 = sCTC_ChannelCoding_per_Modulation_HARQ503[(unsigned char)m_nInterleaver_Index].bP1;
		P2 = sCTC_ChannelCoding_per_Modulation_HARQ503[(unsigned char)m_nInterleaver_Index].bP2;
		P3 = sCTC_ChannelCoding_per_Modulation_HARQ503[(unsigned char)m_nInterleaver_Index].bP3;

//		m_EncodedDataBlockSize = 6 * m_Nsch * m_MODORDER; // Lk @ BitSelection byte
	}

	// Step 1. Switch alternate couples, CTC Interleaver
	// 01 -> 10 & 10 -> 01 for even order
	for(Index=1;Index<m_bN;Index=Index + 2)
	{
		// no switch -> skip index (0, 2, 4, ...)
		// switch
		Temp = EXTi[Index][1];
		EXTi[Index][1] = EXTi[Index][2];
		EXTi[Index][2] = Temp;
	}

//	cout << "Interleaving" << endl;

	// Step 2. P(j)
	for(Index=0;Index<N_div4;Index++)
	{
		Index_mul4 = 4 * Index;

		Pj0 = (P0 * (Index_mul4    ) + 1              ) % m_bN; // odd
		Pj1 = (P0 * (Index_mul4 + 1) + 1 + m_bN/2 + P1) % m_bN; // even
		Pj2 = (P0 * (Index_mul4 + 2) + 1          + P2) % m_bN; // even
		Pj3 = (P0 * (Index_mul4 + 3) + 1 + m_bN/2 + P3) % m_bN; // even

		// do not use same variable between left side and right side
		EXT[Index_mul4  ][0] = EXTi[Pj0][0];
		EXT[Index_mul4  ][1] = EXTi[Pj0][1];
		EXT[Index_mul4  ][2] = EXTi[Pj0][2];
		EXT[Index_mul4  ][3] = EXTi[Pj0][3];

		EXT[Index_mul4+1][0] = EXTi[Pj1][0];
		EXT[Index_mul4+1][1] = EXTi[Pj1][1];
		EXT[Index_mul4+1][2] = EXTi[Pj1][2];
		EXT[Index_mul4+1][3] = EXTi[Pj1][3];

		EXT[Index_mul4+2][0] = EXTi[Pj2][0];
		EXT[Index_mul4+2][1] = EXTi[Pj2][1];
		EXT[Index_mul4+2][2] = EXTi[Pj2][2];
		EXT[Index_mul4+2][3] = EXTi[Pj2][3];

		EXT[Index_mul4+3][0] = EXTi[Pj3][0];
		EXT[Index_mul4+3][1] = EXTi[Pj3][1];
		EXT[Index_mul4+3][2] = EXTi[Pj3][2];
		EXT[Index_mul4+3][3] = EXTi[Pj3][3];


/*		cout << Index << endl;
		cout << EXT[Index_mul4][0] << " " << EXT[Index_mul4][1] << " " << EXT[Index_mul4][2] << " " << EXT[Index_mul4][3] << endl;
		cout << EXTi[Index_mul4][0] << " " << EXTi[Index_mul4][1] << " " << EXTi[Index_mul4][2] << " " << EXTi[Index_mul4][3] << endl;

		cout << EXT[Index_mul4+1][0] << " " << EXT[Index_mul4+1][1] << " " << EXT[Index_mul4+1][2] << " " << EXT[Index_mul4+1][3] << endl;
		cout << EXTi[Index_mul4+1][0] << " " << EXTi[Index_mul4+1][1] << " " << EXTi[Index_mul4+1][2] << " " << EXTi[Index_mul4+1][3] << endl;

		cout << EXT[Index_mul4+2][0] << " " << EXT[Index_mul4+2][1] << " " << EXT[Index_mul4+2][2] << " " << EXT[Index_mul4+2][3] << endl;
		cout << EXTi[Index_mul4+2][0] << " " << EXTi[Index_mul4+2][1] << " " << EXTi[Index_mul4+2][2] << " " << EXTi[Index_mul4+2][3] << endl;

		cout << EXT[Index_mul4+3][0] << " " << EXT[Index_mul4+3][1] << " " << EXT[Index_mul4+3][2] << " " << EXT[Index_mul4+3][3] << endl;
		cout << EXTi[Index_mul4+3][0] << " " << EXTi[Index_mul4+3][1] << " " << EXTi[Index_mul4+3][2] << " " << EXTi[Index_mul4+3][3] << endl;
*/
	}
}

void FECDecoder::InverseInterleaving(void) {

	unsigned int Index;
	int Temp;
	unsigned int Index_mul4;

//	unsigned int N_div2;
	unsigned int N_div4;

	unsigned int Pj0;
	unsigned int Pj1;
	unsigned int Pj2;
	unsigned int Pj3;

	unsigned int P0;
	unsigned int P1;
	unsigned int P2;
	unsigned int P3;


//	N_div2 = m_bN >> 1; // N
	N_div4 = m_bN >> 2; // N/2

	if((m_eHARQ == NO)||(m_eHARQ == CC))
	{
		//	m_N  = CTC_ChannelCoding_per_Modulation_502_noHARQ[(unsigned char)m_Interleaver_Index].N;
		P0 = sCTC_ChannelCoding_per_Modulation_noHARQ502[(unsigned char)m_nInterleaver_Index].bP0;
		P1 = sCTC_ChannelCoding_per_Modulation_noHARQ502[(unsigned char)m_nInterleaver_Index].bP1;
		P2 = sCTC_ChannelCoding_per_Modulation_noHARQ502[(unsigned char)m_nInterleaver_Index].bP2;
		P3 = sCTC_ChannelCoding_per_Modulation_noHARQ502[(unsigned char)m_nInterleaver_Index].bP3;

//		m_EncodedDataBlockSize = sCTC_ChannelCoding_per_Modulation_noHARQ502[(unsigned char)//m_Interleaver_Index].EncodedDataBlockSize;
	}else if(m_eHARQ == IR){
		//	m_N  = CTC_ChannelCoding_per_Modulation_502_noHARQ[(unsigned char)m_Interleaver_Index].N;
		P0 = sCTC_ChannelCoding_per_Modulation_HARQ503[(unsigned char)m_nInterleaver_Index].bP0;
		P1 = sCTC_ChannelCoding_per_Modulation_HARQ503[(unsigned char)m_nInterleaver_Index].bP1;
		P2 = sCTC_ChannelCoding_per_Modulation_HARQ503[(unsigned char)m_nInterleaver_Index].bP2;
		P3 = sCTC_ChannelCoding_per_Modulation_HARQ503[(unsigned char)m_nInterleaver_Index].bP3;

//		m_EncodedDataBlockSize = 6 * m_Nsch * m_MODORDER; // Lk @ BitSelection byte
	}

//	cout << "InverseInterleaving" << endl;



	// Step 2. P(j)
	for(Index=0;Index<N_div4;Index++)
	{
		Index_mul4 = 4 * Index;

		Pj0 = (P0 * (Index_mul4    ) + 1              ) % m_bN; // odd
		Pj1 = (P0 * (Index_mul4 + 1) + 1 + m_bN/2 + P1) % m_bN; // even
		Pj2 = (P0 * (Index_mul4 + 2) + 1          + P2) % m_bN; // even
		Pj3 = (P0 * (Index_mul4 + 3) + 1 + m_bN/2 + P3) % m_bN; // even

		// do not use same variable between left side and right side
		EXT[Pj0][0] = EXTi[Index_mul4  ][0];
		EXT[Pj0][1] = EXTi[Index_mul4  ][1];
		EXT[Pj0][2] = EXTi[Index_mul4  ][2];
		EXT[Pj0][3] = EXTi[Index_mul4  ][3];

		EXT[Pj1][0] = EXTi[Index_mul4+1][0];
		EXT[Pj1][1] = EXTi[Index_mul4+1][1];
		EXT[Pj1][2] = EXTi[Index_mul4+1][2];
		EXT[Pj1][3] = EXTi[Index_mul4+1][3];

		EXT[Pj2][0] = EXTi[Index_mul4+2][0];
		EXT[Pj2][1] = EXTi[Index_mul4+2][1];
		EXT[Pj2][2] = EXTi[Index_mul4+2][2];
		EXT[Pj2][3] = EXTi[Index_mul4+2][3];

		EXT[Pj3][0] = EXTi[Index_mul4+3][0];
		EXT[Pj3][1] = EXTi[Index_mul4+3][1];
		EXT[Pj3][2] = EXTi[Index_mul4+3][2];
		EXT[Pj3][3] = EXTi[Index_mul4+3][3];

//		cout << Pj0 << " " << Pj1 << " " << Pj2 << " " << Pj3 << endl;

		/*
		cout << Index << endl;
		cout << EXT[Index_mul4][0] << " " << EXT[Index_mul4][1] << " " << EXT[Index_mul4][2] << " " << EXT[Index_mul4][3] << endl;
		cout << EXTi[Index_mul4][0] << " " << EXTi[Index_mul4][1] << " " << EXTi[Index_mul4][2] << " " << EXTi[Index_mul4][3] << endl;

		cout << EXT[Index_mul4+1][0] << " " << EXT[Index_mul4+1][1] << " " << EXT[Index_mul4+1][2] << " " << EXT[Index_mul4+1][3] << endl;
		cout << EXTi[Index_mul4+1][0] << " " << EXTi[Index_mul4+1][1] << " " << EXTi[Index_mul4+1][2] << " " << EXTi[Index_mul4+1][3] << endl;

		cout << EXT[Index_mul4+2][0] << " " << EXT[Index_mul4+2][1] << " " << EXT[Index_mul4+2][2] << " " << EXT[Index_mul4+2][3] << endl;
		cout << EXTi[Index_mul4+2][0] << " " << EXTi[Index_mul4+2][1] << " " << EXTi[Index_mul4+2][2] << " " << EXTi[Index_mul4+2][3] << endl;

		cout << EXT[Index_mul4+3][0] << " " << EXT[Index_mul4+3][1] << " " << EXT[Index_mul4+3][2] << " " << EXT[Index_mul4+3][3] << endl;
		cout << EXTi[Index_mul4+3][0] << " " << EXTi[Index_mul4+3][1] << " " << EXTi[Index_mul4+3][2] << " " << EXTi[Index_mul4+3][3] << endl;
*/
	}

	// Step 1. Switch alternate couples, CTC Interleaver
	// 01 -> 10 & 10 -> 01 for even order
	for(Index=1;Index<m_bN;Index=Index + 2)
	{
		// no switch -> skip index (0, 2, 4, ...)
		// switch
		Temp = EXT[Index][1];
		EXT[Index][1] = EXT[Index][2];
		EXT[Index][2] = Temp;
	}
}


void FECDecoder::MaxLogMAPDecoding(unsigned char Iteration) {


	if(Iteration != 0)
		SetInitialState(m_InitialStateIndex[0]);

	// CTC Decoding 1
	// Branch Metric
	CalcBranchMetric(A, B, Y1, W1);
	// Forward
	CalcAlpha();
	// Backward
	CalcBeta();
	// A Posteriori Probabilities
	CalcAPP();

	// Estimate Circular State for next iteration
	m_InitialStateIndex[0] = EstCircularState();

//	cout << "-Sc1 : " << (unsigned int) m_InitialStateIndex[0] << endl;


	ExtrinsicScaling(A, B);

	Interleaving();

	// CTC Decoding 2

	if(Iteration != 0)
		SetInitialState(m_InitialStateIndex[1]);

	// Branch Metric
	CalcBranchMetric(Ai, Bi, Y2, W2);
	// Forward
	CalcAlpha();
	// Backward
	CalcBeta();
	// A Posteriori Probabilities
	CalcAPP();

	// Estimate Circular State for next iteration
	m_InitialStateIndex[1] = EstCircularState();

//	cout << "-Sc2 : " << (unsigned int) m_InitialStateIndex[1] << endl;

	ExtrinsicScaling(Ai, Bi);

	InverseInterleaving();
}

void FECDecoder::Decision(unsigned char FECBlockIndex) {

	unsigned char Index;
	unsigned char MaxIndex;

	// Decision from APP after Inverse Interleaving
	for(Index=0;Index<m_bN;Index++)
	{
		EXTi[Index][0] = APP[Index][0];
		EXTi[Index][1] = APP[Index][1];
		EXTi[Index][2] = APP[Index][2];
		EXTi[Index][3] = APP[Index][3];
	}

	InverseInterleaving();


	for(Index=0;Index<m_bN;Index++)
	{
		MaxIndex = Max(EXT[Index], CTCINPUTNUM);

//		cout << (unsigned int)MaxIndex << " " ;
		// Debug purpose
//		EXTi[Index][0] = EXT[Index][MinIndex];


		switch(MaxIndex)
		{ // ? FECBlock Index???
			case 0:
				m_bdataout[2*Index    ] = 0;
				m_bdataout[2*Index + 1] = 0;
				break;
			case 1:
				m_bdataout[2*Index    ] = 0;
				m_bdataout[2*Index + 1] = 1;
				break;
			case 2:
				m_bdataout[2*Index    ] = 1;
				m_bdataout[2*Index + 1] = 0;
				break;
			case 3:
				m_bdataout[2*Index    ] = 1;
				m_bdataout[2*Index + 1] = 1;
				break;
		}
	}
}

void FECDecoder::CTCDecoding(void) {
	unsigned char Index;
	unsigned int  DataOutOffset;
	unsigned int  bNepx3;
	unsigned int  bEncodedDataBlockSize;
	unsigned char k;

	DataOutOffset = 0;
	m_BEncodedBurstSize = 0;
	if(m_eHARQ == NO)
	{
		SlotConcatenate();
	}

	for(Index=0;Index<m_nFECBlockNumber;Index++)
	{
		m_nInterleaver_Index = SetInterleaverParameter(Index); 		// Find N, P0~P3

		m_bN   = sCTC_ChannelCoding_per_Modulation_noHARQ502[(unsigned char)m_nInterleaver_Index].bN;

		bEncodedDataBlockSize = sCTC_ChannelCoding_per_Modulation_noHARQ502[(unsigned char)m_nInterleaver_Index].BEncodedDataBlockSize * 8; // Encoded data block size(bits)
		bNepx3 = (3* (2* m_bN));
//		Nmod7 = m_bN % 7;

		// Clear
		for(k=0;k<(m_bN+1);k++)
		{
			EXT[k][0] = 0; EXT[k][1] = 0; EXT[k][2] = 0; EXT[k][3] = 0;
			Alpha[k][0] = 0;Alpha[k][1] = 0;Alpha[k][2] = 0;Alpha[k][3] = 0;
			Alpha[k][4] = 0;Alpha[k][5] = 0;Alpha[k][6] = 0;Alpha[k][7] = 0;
			Beta[k][0] = 0;Beta[k][1] = 0;Beta[k][2] = 0;Beta[k][3] = 0;
			Beta[k][4] = 0;Beta[k][5] = 0;Beta[k][6] = 0;Beta[k][7] = 0;
		}

		// Inverse bitselect & zero padding
		DataOutOffset = InverseBitSelect(bNepx3, bEncodedDataBlockSize, DataOutOffset, Index);
/*
		cout << endl;
		cout << "PaddedEncodedBlock " << endl;
		for(k=0;k<144;k++)
		{
			if(m_Bdatain[k] == 0x80)
				cout << "1";
			else
				cout << "0";
		}
		cout << endl;
*/
		// Inverse bit grouping & SubBlock interleaving
		InverseSubblockInterleaving();
		InterleavingSystematic();
/*
		cout << "Source " << endl;
		for(k=0;k<m_bN;k++)
		{
			if((A[k] > 0) && (B[k] > 0))
				cout << "3";
			else if((A[k] > 0) && (B[k] < 0))
				cout << "2";
			else if((A[k] < 0) && (B[k] > 0))
				cout << "1";
			else
				cout << "0";

			cout << " ";
		}


		cout << endl;
		cout << "A ";
		for(k=0;k<m_bN;k++)
		{
			cout << " ";

			if(A[k] > 0)
				cout << "1";
			else
				cout << "0";
		}
		cout << endl;
		cout << "B ";
		for(k=0;k<m_bN;k++)
		{
			cout << " ";

			if(B[k]  > 0)
				cout << "1";
			else
				cout << "0";
		}
		cout << endl;

		cout << "Y1";
		for(k=0;k<m_bN;k++)
		{
			cout << " ";

			if(Y1[k] > 0)
				cout << "1";
			else
				cout << "0";
		}
		cout << endl;
		cout << "W1";
		for(k=0;k<m_bN;k++)
		{
			cout << " ";

			if(W1[k] > 0)
				cout << "1";
			else
				cout << "0";
		}
		cout << endl;

		cout << "Y2";
		for(k=0;k<m_bN;k++)
		{
			cout << " ";

			if(Y2[k] > 0)
				cout << "1";
			else
				cout << "0";
		}
		cout << endl;
		cout << "W2";
		for(k=0;k<m_bN;k++)
		{
			cout << " ";

			if(W2[k] > 0)
				cout << "1";
			else
				cout << "0";
		}
		cout << endl;
*/
		for(k=0;k<m_Iteration;k++)
		{
//			cout << endl << "Iteration : " << k ;

			MaxLogMAPDecoding(k);
		}

		// detector
		Decision(Index);
	}
}
