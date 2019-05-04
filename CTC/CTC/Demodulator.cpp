/*
 * Demodulator.cpp
 *
 *  Created on: 2012. 12. 15.
 *      Author: Peter
 */

#include "Demodulator.h"

#define LLR_NUMOFSOFTBITS 8
//QPSK #define GAIN 0.015625f// 0.03125f // 128/4096 // Under no noise, LLR should AVG of LLR_NUMOFSOFTBITS
//#define GAIN 0.000244140625f// 0.015625f // QPSK
#define GAIN 1

//#define POW2TO12 4096
#define MASK16 0xffff

#define SQRT2x2  2.8284271247461900976033774484194f

#define SQRT2_10 0.63245553203367586639977870888654f
#define SQRT4_10 1.2649110640673517327995574177731f
#define SQRT8_10 2.5298221281347034655991148355462f

#define SQRT1_10 0.3162277660168379f
#define SQRT3_10 0.9486832980505138f
#define SQRT1_42 0.1543033499620919f
#define SQRT3_42 0.4629100498862757f
#define SQRT5_42 0.7715167498104596f
#define SQRT7_42 1.080123449734643f

#define SQRT2_42 0.308606699924183820522188925528f
#define SQRT4_42 0.617213399848367641044377851056f
#define SQRT6_42 0.925820099772551461566566776584f
#define SQRT8_42 1.234426799696735282088755702112f
#define SQRT12_42 1.851640199545102923133133553168f
#define SQRT16_42 2.468853599393470564177511404224f

Demodulator::Demodulator() {
	// TODO Auto-generated constructor stub

}

Demodulator::~Demodulator() {
	// TODO Auto-generated destructor stub
}

void Demodulator::Demodulate(WIMAXMCS mcs)
{
	unsigned int i;

	switch(mcs)
	{
	case QPSK_1_2:
	case QPSK_3_4:

	m_LLRSize = m_SIGNALSize * MODORDEROFQPSK;

	for(i=0;i<m_LLRSize;i++)
	{
		QPSKDemodulate(m_datain[i], &m_dataout[MODORDEROFQPSK*i]);
	}

	break;

	case QAM16_1_2:
	case QAM16_3_4:

	m_LLRSize = m_SIGNALSize * MODORDEROFQAM16;

	for(i=0;i<m_SIGNALSize;i++)
	{
		QAM16Demodulate(m_datain[i], &m_dataout[MODORDEROFQAM16*i]);
	}

	break;

	case QAM64_1_2:
	case QAM64_2_3:
	case QAM64_3_4:
	case QAM64_5_6:

	m_LLRSize = m_SIGNALSize * MODORDEROFQAM64;

	for(i=0;i<m_SIGNALSize;i++)
	{
		QAM64Demodulate(m_datain[i], &m_dataout[MODORDEROFQAM64*i]);
	}

	break;
	}
}

void Demodulator::QPSKDemodulate(SIGNAL in, BYTE * out)
{
//	const SIGNAL B00(  SQRT1_2,  SQRT1_2); // b0=0, b1=0
//	const SIGNAL B01( -SQRT1_2,  SQRT1_2); // b0=0, b1=1
//	const SIGNAL B10(  SQRT1_2, -SQRT1_2); // b0=1, b1=0
//	const SIGNAL B11( -SQRT1_2, -SQRT1_2); // b0=1, b1=1
	float out0, out1;
	out0 = (-1 * (SQRT2x2 * (float)in.imag)); // inphase
	out1 = (-1 * (SQRT2x2 * (float)in.real)); // quadrature

	if(out0 >= 2)
		out[0] = 127; // MAX 1
	else if(out0 <= -2)
		out[0] = -128; // MIN 0
	else
		out[0] = (BYTE) out0 * 64;

	if(out1 >= 2)
		out[1] = 127; // MAX 1
	else if(out1 <= -2)
		out[1] = -128; // MIN 0
	else
		out[1] = (BYTE) out1 * 64;
}

void Demodulator::QAM16Demodulate(SIGNAL in, BYTE * out)
{
//	const SIGNAL B0000(  SQRT1_10,  SQRT1_10); // b0=0, b1=0 b2=0, b3=0
//	const SIGNAL B0001(  SQRT1_10,  SQRT3_10); // b0=0, b1=0 b2=0, b3=1
//	const SIGNAL B0101(  SQRT3_10,  SQRT3_10); // b0=0, b1=1 b2=0, b3=1
//	const SIGNAL B0100(  SQRT3_10,  SQRT1_10); // b0=0, b1=1 b2=0, b3=0

//	const SIGNAL B1000( -SQRT1_10,  SQRT1_10); // b0=1, b1=0 b2=0, b3=0
//	const SIGNAL B1001( -SQRT1_10,  SQRT3_10); // b0=1, b1=0 b2=0, b3=1
//	const SIGNAL B1101( -SQRT3_10,  SQRT3_10); // b0=1, b1=1 b2=0, b3=1
//	const SIGNAL B1100( -SQRT3_10,  SQRT1_10); // b0=1, b1=1 b2=0, b3=0

//	const SIGNAL B1010( -SQRT1_10, -SQRT1_10); // b0=1, b1=0 b2=1, b3=0
//	const SIGNAL B1011( -SQRT1_10, -SQRT3_10); // b0=1, b1=0 b2=1, b3=1
//	const SIGNAL B1111( -SQRT3_10, -SQRT3_10); // b0=1, b1=1 b2=1, b3=1
//	const SIGNAL B1110( -SQRT3_10, -SQRT1_10); // b0=1, b1=1 b2=1, b3=0

//	const SIGNAL B0010(  SQRT1_10, -SQRT1_10); // b0=0, b1=0 b2=1, b3=0
//	const SIGNAL B0011(  SQRT1_10, -SQRT3_10); // b0=0, b1=0 b2=1, b3=1
//	const SIGNAL B0111(  SQRT3_10, -SQRT3_10); // b0=0, b1=1 b2=1, b3=1
//	const SIGNAL B0110(  SQRT3_10, -SQRT1_10); // b0=0, b1=1 b2=1, b3=0

	if((short)in.real > (((short)(SQRT2_10))) )
		out[0] = (BYTE)((-GAIN * SQRT8_10 * in.real) + (GAIN * 0.8f)); 	// inphase
	else if((short)in.real > (((short)(-SQRT2_10))) )
		out[0] = (BYTE)((-GAIN * SQRT4_10) * in.real); 				// inphase
	else
		out[0] = (BYTE)((-GAIN * SQRT8_10 * in.real) - (GAIN * 0.8f)); 	// inphase

	out[1] = (BYTE)((GAIN * SQRT4_10 * in.real) - (GAIN * 0.8f)); 	// inphase

	if((short)in.imag > (((short)(SQRT2_10))) )
		out[2] = (BYTE)((-GAIN * SQRT8_10 * in.imag) + (GAIN * 0.8f)); 	// quadrature
	else if((short)in.imag > (((short)(-SQRT2_10))) )
		out[2] = (BYTE)((-GAIN * SQRT4_10 * in.imag)); 				// quadrature
	else
		out[2] = (BYTE)((-GAIN * SQRT8_10 * in.imag) - (GAIN * 0.8f)); 	// quadrature

	out[3] = (BYTE)((GAIN * SQRT4_10 * in.imag) - (GAIN * 0.8f)); 	// quadrature
}

void Demodulator::QAM64Demodulate(SIGNAL in, BYTE * out)
{
/*
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
 */

	if(in.real > SQRT6_42 )
		out[0] = (BYTE)((GAIN * SQRT16_42) * (in.real - SQRT3_42));	// inphase
	else if(in.real > SQRT4_42 )
		out[0] = (BYTE)((GAIN * SQRT12_42) * (in.real - SQRT2_42));	// inphase
	else if(in.real > SQRT2_42 )
		out[0] = (BYTE)((GAIN * SQRT8_42)  * (in.real - SQRT1_42));	// inphase
	else if(in.real > -SQRT2_42 )
		out[0] = (BYTE)((GAIN * SQRT4_42)  * (in.real));	// inphase
	else if(in.real > -SQRT4_42 )
		out[0] = (BYTE)((GAIN * SQRT8_42)  * (in.real + SQRT1_42));	// inphase
	else if(in.real > -SQRT6_42 )
		out[0] = (BYTE)((GAIN * SQRT12_42) * (in.real + SQRT2_42));	// inphase
	else
		out[0] = (BYTE)((GAIN * SQRT16_42) * (in.real + SQRT3_42));	// inphase


	if((in.real > SQRT6_42 )||(in.real < -SQRT6_42 ))
		out[1] = (BYTE)((GAIN * SQRT8_42) * (-in.real + SQRT5_42)); // inphase
	else if((in.real > SQRT2_42 )||(in.real < -SQRT2_42 ))
		out[1] = (BYTE)((GAIN * SQRT4_42) * (-in.real + SQRT4_42)); // inphase
	else
		out[1] = (BYTE)((GAIN * SQRT8_42) * (-in.real + SQRT3_42)); // inphase


	if((in.real > SQRT4_42 )||(in.real < -SQRT4_42 ))
		out[2] = (BYTE)((GAIN * SQRT4_42) * (-in.real + SQRT6_42)); // inphase
	else
		out[2] = (BYTE)((GAIN * SQRT4_42) * ( in.real - SQRT2_42)); // inphase


	if(in.imag > SQRT6_42 )
		out[3] = (BYTE)((GAIN * SQRT16_42) * (in.imag - SQRT3_42));	// quadrature
	else if(in.imag > SQRT4_42 )
		out[3] = (BYTE)((GAIN * SQRT12_42) * (in.imag - SQRT2_42));	// quadrature
	else if(in.imag > SQRT2_42 )
		out[3] = (BYTE)((GAIN * SQRT8_42)  * (in.imag - SQRT1_42));	// quadrature
	else if(in.imag > -SQRT2_42 )
		out[3] = (BYTE)((GAIN * SQRT4_42)  * (in.imag));	// quadrature
	else if(in.imag > -SQRT4_42 )
		out[3] = (BYTE)((GAIN * SQRT8_42)  * (in.imag + SQRT1_42));	// quadrature
	else if(in.imag > -SQRT6_42 )
		out[3] = (BYTE)((GAIN * SQRT12_42) * (in.imag + SQRT2_42));	// quadrature
	else
		out[3] = (BYTE)((GAIN * SQRT16_42) * (in.imag + SQRT3_42));	// quadrature


	if((in.imag > SQRT6_42 )||(in.imag < -SQRT6_42 ))
		out[4] = (BYTE)((GAIN * SQRT8_42) * (-in.imag + SQRT5_42)); // quadrature
	else if((in.imag > SQRT2_42 )||(in.imag < -SQRT2_42 ))
		out[4] = (BYTE)((GAIN * SQRT4_42) * (-in.imag + SQRT4_42)); // quadrature
	else
		out[4] = (BYTE)((GAIN * SQRT8_42) * (-in.imag + SQRT3_42)); // quadrature


	if((in.imag > SQRT4_42 )||(in.imag < -SQRT4_42 ))
		out[5] = (BYTE)((GAIN * SQRT4_42) * (-in.imag + SQRT6_42)); // quadrature
	else
		out[5] = (BYTE)((GAIN * SQRT4_42) * ( in.imag - SQRT2_42)); // quadrature
}
