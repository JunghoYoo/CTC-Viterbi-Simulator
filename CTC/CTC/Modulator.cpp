/*
 * Modulator.cpp
 *
 *  Created on: 2012. 4. 22.
 *      Author: Peter
 */

#include "Modulator.h"

#define SQRT1_2  0.7071067811865475f
#define SQRT1_10 0.3162277660168379f
#define SQRT3_10 0.9486832980505138f
#define SQRT1_42 0.1543033499620919f
#define SQRT3_42 0.4629100498862757f
#define SQRT5_42 0.7715167498104596f
#define SQRT7_42 1.080123449734643f

Modulator::Modulator() {
	// TODO Auto-generated constructor stub

}

Modulator::~Modulator() {
	// TODO Auto-generated destructor stub
}

void Modulator::Modulate(WIMAXMCS mcs)
{
	unsigned int i;

	switch(mcs)
	{
	case QPSK_1_2:
	case QPSK_3_4:

	m_SIGNALSize = m_BITSize/2;

	for(i=0;i<m_SIGNALSize;i++)
	{
		m_dataout[i] = QPSKModulate(&m_datain[2*i]);
	}

	break;

	case QAM16_1_2:
	case QAM16_3_4:

	m_SIGNALSize = m_BITSize/4;

	for(i=0;i<m_SIGNALSize;i++)
	{
		m_dataout[i] = QAM16Modulate(&m_datain[4*i]);
	}

	break;

	case QAM64_1_2:
	case QAM64_2_3:
	case QAM64_3_4:
	case QAM64_5_6:

	m_SIGNALSize = m_BITSize/4;

	for(i=0;i<m_SIGNALSize;i++)
	{
		m_dataout[i] = QAM64Modulate(&m_datain[4*i]);
	}

	break;
	}
}

const SIGNAL Modulator::BPSKModulate(BIT * in)
{
	const SIGNAL B0( 1, 0);
	const SIGNAL B1(-1, 0);

	const SIGNAL NULL(0, 0);

	if(in[0] == 0)
	{
		return B0;
	}else if(in[0] == 1)
	{
		return B1;
	}
	return NULL;
}

const SIGNAL Modulator::QPSKModulate(BIT * in)
{
	const SIGNAL B00(  SQRT1_2,  SQRT1_2); // b0=0, b1=0
	const SIGNAL B01( -SQRT1_2,  SQRT1_2); // b0=0, b1=1
	const SIGNAL B10(  SQRT1_2, -SQRT1_2); // b0=1, b1=0
	const SIGNAL B11( -SQRT1_2, -SQRT1_2); // b0=1, b1=1

	const SIGNAL NULL(0, 0);

	if(      (in[0] == 0)&&(in[1] == 0))
	{
		return B00;
	}else if((in[0] == 0)&&(in[1] == 1))
	{
		return B01;
	}else if((in[0] == 1)&&(in[1] == 0))
	{
		return B10;
	}else if((in[0] == 1)&&(in[1] == 1))
	{
		return B11;
	}
	return NULL;
}

const SIGNAL Modulator::QAM16Modulate(BIT * in)
{
	const SIGNAL B0000(  SQRT1_10,  SQRT1_10); // b0=0, b1=0 b2=0, b3=0
	const SIGNAL B0001(  SQRT1_10,  SQRT3_10); // b0=0, b1=0 b2=0, b3=1
	const SIGNAL B0101(  SQRT3_10,  SQRT3_10); // b0=0, b1=1 b2=0, b3=1
	const SIGNAL B0100(  SQRT3_10,  SQRT1_10); // b0=0, b1=1 b2=0, b3=0

	const SIGNAL B1000( -SQRT1_10,  SQRT1_10); // b0=1, b1=0 b2=0, b3=0
	const SIGNAL B1001( -SQRT1_10,  SQRT3_10); // b0=1, b1=0 b2=0, b3=1
	const SIGNAL B1101( -SQRT3_10,  SQRT3_10); // b0=1, b1=1 b2=0, b3=1
	const SIGNAL B1100( -SQRT3_10,  SQRT1_10); // b0=1, b1=1 b2=0, b3=0

	const SIGNAL B1010( -SQRT1_10, -SQRT1_10); // b0=1, b1=0 b2=1, b3=0
	const SIGNAL B1011( -SQRT1_10, -SQRT3_10); // b0=1, b1=0 b2=1, b3=1
	const SIGNAL B1111( -SQRT3_10, -SQRT3_10); // b0=1, b1=1 b2=1, b3=1
	const SIGNAL B1110( -SQRT3_10, -SQRT1_10); // b0=1, b1=1 b2=1, b3=0

	const SIGNAL B0010(  SQRT1_10, -SQRT1_10); // b0=0, b1=0 b2=1, b3=0
	const SIGNAL B0011(  SQRT1_10, -SQRT3_10); // b0=0, b1=0 b2=1, b3=1
	const SIGNAL B0111(  SQRT3_10, -SQRT3_10); // b0=0, b1=1 b2=1, b3=1
	const SIGNAL B0110(  SQRT3_10, -SQRT1_10); // b0=0, b1=1 b2=1, b3=0

	const SIGNAL NULL(0, 0);

	if(      (in[3] == 0)&&(in[2] == 0)&&(in[1] == 0)&&(in[0] == 0))
	{
		return B0000;
	}else if((in[3] == 0)&&(in[2] == 0)&&(in[1] == 0)&&(in[0] == 1))
	{
		return B0001;
	}else if((in[3] == 0)&&(in[2] == 0)&&(in[1] == 1)&&(in[0] == 0))
	{
		return B0010;
	}else if((in[3] == 0)&&(in[2] == 0)&&(in[1] == 1)&&(in[0] == 1))
	{
		return B0011;
	}else if((in[3] == 0)&&(in[2] == 1)&&(in[1] == 0)&&(in[0] == 0))
	{
		return B0100;
	}else if((in[3] == 0)&&(in[2] == 1)&&(in[1] == 0)&&(in[0] == 1))
	{
		return B0101;
	}else if((in[3] == 0)&&(in[2] == 1)&&(in[1] == 1)&&(in[0] == 0))
	{
		return B0110;
	}else if((in[3] == 0)&&(in[2] == 1)&&(in[1] == 1)&&(in[0] == 1))
	{
		return B0111;
	}else if((in[3] == 1)&&(in[2] == 0)&&(in[1] == 0)&&(in[0] == 0))
	{
		return B1000;
	}else if((in[3] == 1)&&(in[2] == 0)&&(in[1] == 0)&&(in[0] == 1))
	{
		return B1001;
	}else if((in[3] == 1)&&(in[2] == 0)&&(in[1] == 1)&&(in[0] == 0))
	{
		return B1010;
	}else if((in[3] == 1)&&(in[2] == 0)&&(in[1] == 1)&&(in[0] == 1))
	{
		return B1011;
	}else if((in[3] == 1)&&(in[2] == 1)&&(in[1] == 0)&&(in[0] == 0))
	{
		return B1100;
	}else if((in[3] == 1)&&(in[2] == 1)&&(in[1] == 0)&&(in[0] == 1))
	{
		return B1101;
	}else if((in[3] == 1)&&(in[2] == 1)&&(in[1] == 1)&&(in[0] == 0))
	{
		return B1110;
	}else if((in[3] == 1)&&(in[2] == 1)&&(in[1] == 1)&&(in[0] == 1))
	{
		return B1111;
	}
	return NULL;
}

const SIGNAL Modulator::QAM64Modulate(BIT * in)
{
	const SIGNAL B111111( -SQRT7_42, -SQRT7_42);
	const SIGNAL B111110( -SQRT7_42, -SQRT5_42);
	const SIGNAL B111100( -SQRT7_42, -SQRT3_42);
	const SIGNAL B111101( -SQRT7_42, -SQRT1_42);
	const SIGNAL B111001( -SQRT7_42,  SQRT1_42);
	const SIGNAL B111000( -SQRT7_42,  SQRT3_42);
	const SIGNAL B111010( -SQRT7_42,  SQRT5_42);
	const SIGNAL B111011( -SQRT7_42,  SQRT7_42);

	const SIGNAL B110111( -SQRT5_42, -SQRT7_42);
	const SIGNAL B110110( -SQRT5_42, -SQRT5_42);
	const SIGNAL B110100( -SQRT5_42, -SQRT3_42);
	const SIGNAL B110101( -SQRT5_42, -SQRT1_42);
	const SIGNAL B110001( -SQRT5_42,  SQRT1_42);
	const SIGNAL B110000( -SQRT5_42,  SQRT3_42);
	const SIGNAL B110010( -SQRT5_42,  SQRT5_42);
	const SIGNAL B110011( -SQRT5_42,  SQRT7_42);

	const SIGNAL B100111( -SQRT3_42, -SQRT7_42);
	const SIGNAL B100110( -SQRT3_42, -SQRT5_42);
	const SIGNAL B100100( -SQRT3_42, -SQRT3_42);
	const SIGNAL B100101( -SQRT3_42, -SQRT1_42);
	const SIGNAL B100001( -SQRT3_42,  SQRT1_42);
	const SIGNAL B100000( -SQRT3_42,  SQRT3_42);
	const SIGNAL B100010( -SQRT3_42,  SQRT5_42);
	const SIGNAL B100011( -SQRT3_42,  SQRT7_42);

	const SIGNAL B101111( -SQRT1_42, -SQRT7_42);
	const SIGNAL B101110( -SQRT1_42, -SQRT5_42);
	const SIGNAL B101100( -SQRT1_42, -SQRT3_42);
	const SIGNAL B101101( -SQRT1_42, -SQRT1_42);
	const SIGNAL B101001( -SQRT1_42,  SQRT1_42);
	const SIGNAL B101000( -SQRT1_42,  SQRT3_42);
	const SIGNAL B101010( -SQRT1_42,  SQRT5_42);
	const SIGNAL B101011( -SQRT1_42,  SQRT7_42);

	const SIGNAL B001111(  SQRT1_42, -SQRT7_42);
	const SIGNAL B001110(  SQRT1_42, -SQRT5_42);
	const SIGNAL B001100(  SQRT1_42, -SQRT3_42);
	const SIGNAL B001101(  SQRT1_42, -SQRT1_42);
	const SIGNAL B001001(  SQRT1_42,  SQRT1_42);
	const SIGNAL B001000(  SQRT1_42,  SQRT3_42);
	const SIGNAL B001010(  SQRT1_42,  SQRT5_42);
	const SIGNAL B001011(  SQRT1_42,  SQRT7_42);

	const SIGNAL B000111(  SQRT3_42, -SQRT7_42);
	const SIGNAL B000110(  SQRT3_42, -SQRT5_42);
	const SIGNAL B000100(  SQRT3_42, -SQRT3_42);
	const SIGNAL B000101(  SQRT3_42, -SQRT1_42);
	const SIGNAL B000001(  SQRT3_42,  SQRT1_42);
	const SIGNAL B000000(  SQRT3_42,  SQRT3_42);
	const SIGNAL B000010(  SQRT3_42,  SQRT5_42);
	const SIGNAL B000011(  SQRT3_42,  SQRT7_42);

	const SIGNAL B010111(  SQRT5_42, -SQRT7_42);
	const SIGNAL B010110(  SQRT5_42, -SQRT5_42);
	const SIGNAL B010100(  SQRT5_42, -SQRT3_42);
	const SIGNAL B010101(  SQRT5_42, -SQRT1_42);
	const SIGNAL B010001(  SQRT5_42,  SQRT1_42);
	const SIGNAL B010000(  SQRT5_42,  SQRT3_42);
	const SIGNAL B010010(  SQRT5_42,  SQRT5_42);
	const SIGNAL B010011(  SQRT5_42,  SQRT7_42);

	const SIGNAL B011111(  SQRT7_42, -SQRT7_42);
	const SIGNAL B011110(  SQRT7_42, -SQRT5_42);
	const SIGNAL B011100(  SQRT7_42, -SQRT3_42);
	const SIGNAL B011101(  SQRT7_42, -SQRT1_42);
	const SIGNAL B011001(  SQRT7_42,  SQRT1_42);
	const SIGNAL B011000(  SQRT7_42,  SQRT3_42);
	const SIGNAL B011010(  SQRT7_42,  SQRT5_42);
	const SIGNAL B011011(  SQRT7_42,  SQRT7_42);

	const SIGNAL NULL(0, 0);

	if(      (in[5] == 0)&&(in[4] == 0)&&(in[3] == 0)&&(in[2] == 0)&&(in[1] == 0)&&(in[0] == 0))
	{
		return B000000;
	}else if((in[5] == 0)&&(in[4] == 0)&&(in[3] == 0)&&(in[2] == 0)&&(in[1] == 0)&&(in[0] == 1))
	{
		return B000001;
	}else if((in[5] == 0)&&(in[4] == 0)&&(in[3] == 0)&&(in[2] == 0)&&(in[1] == 1)&&(in[0] == 0))
	{
		return B000010;
	}else if((in[5] == 0)&&(in[4] == 0)&&(in[3] == 0)&&(in[2] == 0)&&(in[1] == 1)&&(in[0] == 1))
	{
		return B000011;
	}else if((in[5] == 0)&&(in[4] == 0)&&(in[3] == 0)&&(in[2] == 1)&&(in[1] == 0)&&(in[0] == 0))
	{
		return B000100;
	}else if((in[5] == 0)&&(in[4] == 0)&&(in[3] == 0)&&(in[2] == 1)&&(in[1] == 0)&&(in[0] == 1))
	{
		return B000101;
	}else if((in[5] == 0)&&(in[4] == 0)&&(in[3] == 0)&&(in[2] == 1)&&(in[1] == 1)&&(in[0] == 0))
	{
		return B000110;
	}else if((in[5] == 0)&&(in[4] == 0)&&(in[3] == 0)&&(in[2] == 1)&&(in[1] == 1)&&(in[0] == 1))
	{
		return B000111;
	}else if((in[5] == 0)&&(in[4] == 0)&&(in[3] == 1)&&(in[2] == 0)&&(in[1] == 0)&&(in[0] == 0))
	{
		return B001000;
	}else if((in[5] == 0)&&(in[4] == 0)&&(in[3] == 1)&&(in[2] == 0)&&(in[1] == 0)&&(in[0] == 1))
	{
		return B001001;
	}else if((in[5] == 0)&&(in[4] == 0)&&(in[3] == 1)&&(in[2] == 0)&&(in[1] == 1)&&(in[0] == 0))
	{
		return B001010;
	}else if((in[5] == 0)&&(in[4] == 0)&&(in[3] == 1)&&(in[2] == 0)&&(in[1] == 1)&&(in[0] == 1))
	{
		return B001011;
	}else if((in[5] == 0)&&(in[4] == 0)&&(in[3] == 1)&&(in[2] == 1)&&(in[1] == 0)&&(in[0] == 0))
	{
		return B001100;
	}else if((in[5] == 0)&&(in[4] == 0)&&(in[3] == 1)&&(in[2] == 1)&&(in[1] == 0)&&(in[0] == 1))
	{
		return B001101;
	}else if((in[5] == 0)&&(in[4] == 0)&&(in[3] == 1)&&(in[2] == 1)&&(in[1] == 1)&&(in[0] == 0))
	{
		return B001110;
	}else if((in[5] == 0)&&(in[4] == 0)&&(in[3] == 1)&&(in[2] == 1)&&(in[1] == 1)&&(in[0] == 1))
	{
		return B001111;
	}else if((in[5] == 0)&&(in[4] == 1)&&(in[3] == 0)&&(in[2] == 0)&&(in[1] == 0)&&(in[0] == 0))
	{
		return B010000;
	}else if((in[5] == 0)&&(in[4] == 1)&&(in[3] == 0)&&(in[2] == 0)&&(in[1] == 0)&&(in[0] == 1))
	{
		return B010001;
	}else if((in[5] == 0)&&(in[4] == 1)&&(in[3] == 0)&&(in[2] == 0)&&(in[1] == 1)&&(in[0] == 0))
	{
		return B010010;
	}else if((in[5] == 0)&&(in[4] == 1)&&(in[3] == 0)&&(in[2] == 0)&&(in[1] == 1)&&(in[0] == 1))
	{
		return B010011;
	}else if((in[5] == 0)&&(in[4] == 1)&&(in[3] == 0)&&(in[2] == 1)&&(in[1] == 0)&&(in[0] == 0))
	{
		return B010100;
	}else if((in[5] == 0)&&(in[4] == 1)&&(in[3] == 0)&&(in[2] == 1)&&(in[1] == 0)&&(in[0] == 1))
	{
		return B010101;
	}else if((in[5] == 0)&&(in[4] == 1)&&(in[3] == 0)&&(in[2] == 1)&&(in[1] == 1)&&(in[0] == 0))
	{
		return B010110;
	}else if((in[5] == 0)&&(in[4] == 1)&&(in[3] == 0)&&(in[2] == 1)&&(in[1] == 1)&&(in[0] == 1))
	{
		return B010111;
	}else if((in[5] == 0)&&(in[4] == 1)&&(in[3] == 1)&&(in[2] == 0)&&(in[1] == 0)&&(in[0] == 0))
	{
		return B011000;
	}else if((in[5] == 0)&&(in[4] == 1)&&(in[3] == 1)&&(in[2] == 0)&&(in[1] == 0)&&(in[0] == 1))
	{
		return B011001;
	}else if((in[5] == 0)&&(in[4] == 1)&&(in[3] == 1)&&(in[2] == 0)&&(in[1] == 1)&&(in[0] == 0))
	{
		return B011010;
	}else if((in[5] == 0)&&(in[4] == 1)&&(in[3] == 1)&&(in[2] == 0)&&(in[1] == 1)&&(in[0] == 1))
	{
		return B011011;
	}else if((in[5] == 0)&&(in[4] == 1)&&(in[3] == 1)&&(in[2] == 1)&&(in[1] == 0)&&(in[0] == 0))
	{
		return B011100;
	}else if((in[5] == 0)&&(in[4] == 1)&&(in[3] == 1)&&(in[2] == 1)&&(in[1] == 0)&&(in[0] == 1))
	{
		return B011101;
	}else if((in[5] == 0)&&(in[4] == 1)&&(in[3] == 1)&&(in[2] == 1)&&(in[1] == 1)&&(in[0] == 0))
	{
		return B011110;
	}else if((in[5] == 0)&&(in[4] == 1)&&(in[3] == 1)&&(in[2] == 1)&&(in[1] == 1)&&(in[0] == 1))
	{
		return B011111;
	}else if((in[5] == 1)&&(in[4] == 0)&&(in[3] == 0)&&(in[2] == 0)&&(in[1] == 0)&&(in[0] == 0))
	{
		return B100000;
	}else if((in[5] == 1)&&(in[4] == 0)&&(in[3] == 0)&&(in[2] == 0)&&(in[1] == 0)&&(in[0] == 1))
	{
		return B100001;
	}else if((in[5] == 1)&&(in[4] == 0)&&(in[3] == 0)&&(in[2] == 0)&&(in[1] == 1)&&(in[0] == 0))
	{
		return B100010;
	}else if((in[5] == 1)&&(in[4] == 0)&&(in[3] == 0)&&(in[2] == 0)&&(in[1] == 1)&&(in[0] == 1))
	{
		return B100011;
	}else if((in[5] == 1)&&(in[4] == 0)&&(in[3] == 0)&&(in[2] == 1)&&(in[1] == 0)&&(in[0] == 0))
	{
		return B100100;
	}else if((in[5] == 1)&&(in[4] == 0)&&(in[3] == 0)&&(in[2] == 1)&&(in[1] == 0)&&(in[0] == 1))
	{
		return B100101;
	}else if((in[5] == 1)&&(in[4] == 0)&&(in[3] == 0)&&(in[2] == 1)&&(in[1] == 1)&&(in[0] == 0))
	{
		return B100110;
	}else if((in[5] == 1)&&(in[4] == 0)&&(in[3] == 0)&&(in[2] == 1)&&(in[1] == 1)&&(in[0] == 1))
	{
		return B100111;
	}else if((in[5] == 1)&&(in[4] == 0)&&(in[3] == 1)&&(in[2] == 0)&&(in[1] == 0)&&(in[0] == 0))
	{
		return B101000;
	}else if((in[5] == 1)&&(in[4] == 0)&&(in[3] == 1)&&(in[2] == 0)&&(in[1] == 0)&&(in[0] == 1))
	{
		return B101001;
	}else if((in[5] == 1)&&(in[4] == 0)&&(in[3] == 1)&&(in[2] == 0)&&(in[1] == 1)&&(in[0] == 0))
	{
		return B101010;
	}else if((in[5] == 1)&&(in[4] == 0)&&(in[3] == 1)&&(in[2] == 0)&&(in[1] == 1)&&(in[0] == 1))
	{
		return B101011;
	}else if((in[5] == 1)&&(in[4] == 0)&&(in[3] == 1)&&(in[2] == 1)&&(in[1] == 0)&&(in[0] == 0))
	{
		return B101100;
	}else if((in[5] == 1)&&(in[4] == 0)&&(in[3] == 1)&&(in[2] == 1)&&(in[1] == 0)&&(in[0] == 1))
	{
		return B101101;
	}else if((in[5] == 1)&&(in[4] == 0)&&(in[3] == 1)&&(in[2] == 1)&&(in[1] == 1)&&(in[0] == 0))
	{
		return B101110;
	}else if((in[5] == 1)&&(in[4] == 0)&&(in[3] == 1)&&(in[2] == 1)&&(in[1] == 1)&&(in[0] == 1))
	{
		return B101111;
	}else if((in[5] == 1)&&(in[4] == 1)&&(in[3] == 0)&&(in[2] == 0)&&(in[1] == 0)&&(in[0] == 0))
	{
		return B110000;
	}else if((in[5] == 1)&&(in[4] == 1)&&(in[3] == 0)&&(in[2] == 0)&&(in[1] == 0)&&(in[0] == 1))
	{
		return B110001;
	}else if((in[5] == 1)&&(in[4] == 1)&&(in[3] == 0)&&(in[2] == 0)&&(in[1] == 1)&&(in[0] == 0))
	{
		return B110010;
	}else if((in[5] == 1)&&(in[4] == 1)&&(in[3] == 0)&&(in[2] == 0)&&(in[1] == 1)&&(in[0] == 1))
	{
		return B110011;
	}else if((in[5] == 1)&&(in[4] == 1)&&(in[3] == 0)&&(in[2] == 1)&&(in[1] == 0)&&(in[0] == 0))
	{
		return B110100;
	}else if((in[5] == 1)&&(in[4] == 1)&&(in[3] == 0)&&(in[2] == 1)&&(in[1] == 0)&&(in[0] == 1))
	{
		return B110101;
	}else if((in[5] == 1)&&(in[4] == 1)&&(in[3] == 0)&&(in[2] == 1)&&(in[1] == 1)&&(in[0] == 0))
	{
		return B110110;
	}else if((in[5] == 1)&&(in[4] == 1)&&(in[3] == 0)&&(in[2] == 1)&&(in[1] == 1)&&(in[0] == 1))
	{
		return B110111;
	}else if((in[5] == 1)&&(in[4] == 1)&&(in[3] == 1)&&(in[2] == 0)&&(in[1] == 0)&&(in[0] == 0))
	{
		return B111000;
	}else if((in[5] == 1)&&(in[4] == 1)&&(in[3] == 1)&&(in[2] == 0)&&(in[1] == 0)&&(in[0] == 1))
	{
		return B111001;
	}else if((in[5] == 1)&&(in[4] == 1)&&(in[3] == 1)&&(in[2] == 0)&&(in[1] == 1)&&(in[0] == 0))
	{
		return B111010;
	}else if((in[5] == 1)&&(in[4] == 1)&&(in[3] == 1)&&(in[2] == 0)&&(in[1] == 1)&&(in[0] == 1))
	{
		return B111011;
	}else if((in[5] == 1)&&(in[4] == 1)&&(in[3] == 1)&&(in[2] == 1)&&(in[1] == 0)&&(in[0] == 0))
	{
		return B111100;
	}else if((in[5] == 1)&&(in[4] == 1)&&(in[3] == 1)&&(in[2] == 1)&&(in[1] == 0)&&(in[0] == 1))
	{
		return B111101;
	}else if((in[5] == 1)&&(in[4] == 1)&&(in[3] == 1)&&(in[2] == 1)&&(in[1] == 1)&&(in[0] == 0))
	{
		return B111110;
	}else if((in[5] == 1)&&(in[4] == 1)&&(in[3] == 1)&&(in[2] == 1)&&(in[1] == 1)&&(in[0] == 1))
	{
		return B111111;
	}

	return NULL;
}
