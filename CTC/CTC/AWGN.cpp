/*
 * AWGN.cpp
 *
 *  Created on: 2012. 10. 4.
 *      Author: SEC
 */

#include "AWGN.h"
#include <stdlib.h>
#include <math.h>
//#define RAND_MAX 0x7fff
//#define POW2TO12 4096
#define SQRT1_2  0.7071067811865475f


AWGN::AWGN() {
	// TODO Auto-generated constructor stub

}

AWGN::~AWGN() {
	// TODO Auto-generated destructor stub
}

float AWGN::Randn(void) // 가우시안 분포 노이즈 알고리즘
{
	float v1, v2, w, z1;
	static float z2 = 0.0;

	if (z2 != 0.0)  {
		z1 = z2;
		z2 = 0.0;
	}
	else {
		do {
			v1 = (float)(2.0*((float)rand() / (float)RAND_MAX)  - 1.0);
			v2 = (float)(2.0*((float)rand() / (float)RAND_MAX)  - 1.0);
			w= v1*v1 + v2*v2;
		}while (w >= 1.0);

		w = (float)sqrt((-2.0*log(w))/w);
		z1 = v1 * w;
		z2 = v2 * w;
	}
	return (z1);
}

void AWGN::Add(SIGNAL * in, SIGNAL * out, unsigned long size) {


	unsigned long index;
	float noise_real, noise_imag;
	float noise_pwr;

	m_SNRlinear = (float) pow((double)10, (0.1f*(m_SNRdB)));
	noise_pwr = 1/m_SNRlinear;

	for(index=0;index<size;index++)
	{
//		noise_real =  (POW2TO12 * Randn()) * (float)m_SNRlinear;
//		noise_imag =  (POW2TO12 * Randn()) * (float)m_SNRlinear;
		noise_real = (1 / (float)sqrt(2)) * Randn() * noise_pwr;
		noise_imag = (1 / (float)sqrt(2)) * Randn() * noise_pwr;

		out[index].real = in[index].real +noise_real;
		out[index].imag = in[index].imag +noise_imag;
	}
}


/*
 unsigned long index;

	var1=2.0*( double(rand())/double(RAND_MAX) ) - 1.0;
	var2=2.0*( double(rand())/double(RAND_MAX) ) - 1.0;


	for(index=0;index<size;index++)
	{
		noise = rand
		data[index] = data[index] + noise;
	}
*/

/*
w=(1/sqrt(2*EbNo(n)))*(randn(1,l)+j*randn(1,l));  %Random noise generation

noise_var_0dB = (1/sqrt(2))*(randn(1,((3*N_SYMSAM)+ch_length)) + i*randn(1,((3*N_SYMSAM)+ch_length)));
noise = noise_var_0dB * 10^(Noise_dB2/20);
RxSignal_sft=RxSignal_sft + noise;
*/
