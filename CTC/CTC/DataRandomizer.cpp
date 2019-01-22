/*
 * DataRandomizer.cpp
 *
 *  Created on: 2012. 4. 7.
 *      Author: Peter
 */

#include "DataRandomizer.h"
#include "FECEncoder.h"

DataRandomizer::DataRandomizer(FECEncoder *FECEncoder)
{
	// TODO Auto-generated constructor stub
	m_FECEncoder = FECEncoder;
}

DataRandomizer::~DataRandomizer() {
	// TODO Auto-generated destructor stub
}

void DataRandomizer::Padding(void)
{
	unsigned int BurstAlloc;	// the amount of data allocated
	unsigned int PaddingLength; // Padding bytes


	if((m_FECEncoder->m_HARQ == NO)||(m_FECEncoder->m_HARQ == CC))
	{
		// Eq 8.4.9.1
		BurstAlloc = m_FECEncoder->m_MPRx6 * Floor( ((float)m_FECEncoder->m_Ns) / m_FECEncoder->m_RepititionFactor );
	}else if(m_FECEncoder->m_HARQ == IR)
	{

	}
	PaddingLength = BurstAlloc - m_FECEncoder->m_BurstSize; // byte

	m_FECEncoder->m_PaddedBurstSize = m_FECEncoder->m_BurstSize + PaddingLength; // total source + padding byte

	// Padding as 0xff 8.4.9.1
	for(unsigned int k=0;k<PaddingLength;k++)
	{
		m_FECEncoder->m_datain[m_FECEncoder->m_BurstSize + k  ] = 1;
		m_FECEncoder->m_datain[m_FECEncoder->m_BurstSize + k+1] = 1;
		m_FECEncoder->m_datain[m_FECEncoder->m_BurstSize + k+2] = 1;
		m_FECEncoder->m_datain[m_FECEncoder->m_BurstSize + k+3] = 1;

		m_FECEncoder->m_datain[m_FECEncoder->m_BurstSize + k+4] = 1;
		m_FECEncoder->m_datain[m_FECEncoder->m_BurstSize + k+5] = 1;
		m_FECEncoder->m_datain[m_FECEncoder->m_BurstSize + k+6] = 1;
		m_FECEncoder->m_datain[m_FECEncoder->m_BurstSize + k+7] = 1;
	}
}
void DataRandomizer::DataRandomize(void)
{
	// Randomize FECBlock
	unsigned int w;
	unsigned char t;
	unsigned char Index;
	BIT shift_reg[16]; // no use [0]

	// Randomize for each FEC Block (initial)
	for(Index=0;Index<m_FECEncoder->m_FECBlockNumber;Index++)
	{
		// Initial [LSB] 0 1 1 0 1 1 1 0 0 0 1 0 1 0 1 [MSB].

		shift_reg[1] = 0;
		shift_reg[2] = 1;
		shift_reg[3] = 1;
		shift_reg[4] = 0;

		shift_reg[5] = 1;
		shift_reg[6] = 1;
		shift_reg[7] = 1;
		shift_reg[8] = 0;

		shift_reg[9] = 0;
		shift_reg[10]= 0;
		shift_reg[11]= 1;
		shift_reg[12]= 0;

		shift_reg[13]= 1;
		shift_reg[14]= 0;
		shift_reg[15]= 1;

		// Fig 285
		for(w=m_FECEncoder->FECBlockIndex[Index].bStart;w<=m_FECEncoder->FECBlockIndex[Index].bEnd;w++)
		{
			// 1 + X14 + X15
			shift_reg[0] = shift_reg[14] ^ shift_reg[15];
			m_FECEncoder->m_datain[w] = m_FECEncoder->m_datain[w] ^ shift_reg[0];
			for(t=15;t>0;t--)
			{
				shift_reg[t] = shift_reg[t-1];
			}
		}
	}
}
