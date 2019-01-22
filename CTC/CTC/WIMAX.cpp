/*
 * WIMAX.cpp
 *
 *  Created on: 2012. 4. 5.
 *      Author: Peter
 */

#include "WIMAX.h"

WIMAX::WIMAX()
{
	// TODO Auto-generated constructor stub
}

WIMAX::~WIMAX() {
	// TODO Auto-generated destructor stub

}

int WIMAX::Floor (double data)
{
	int data_int;

	data_int = (int)data;

	return data_int;
}

int WIMAX::Floor (float data)
{
	int data_int;

	data_int = (int)data;

	return data_int;
}

int WIMAX::Ceil (double data)
{
	double frac;
	int data_int;

	data_int = (int) data;
	frac = data - data_int;

	if(frac > 0.0)
		data_int++; // round up if fractional exists

	return data_int;
}

int WIMAX::Ceil (float data)
{
	float frac;
	int data_int;

	data_int = (int) data;
	frac = data - data_int;

	if(frac > 0.0)
		data_int++; // round up if fractional exists

	return data_int;
}

unsigned int WIMAX::BRO (unsigned int y, unsigned char m)
{
	// Bitreverse CTC subblock interleave
	unsigned int  result;
	unsigned char index;

	BIT digit[16];

	result = 0;

	// bit level extract (16bit)
	for(index=0;index<16;index++)
	{
		if( ((y >> index) & 0x01 ) == 1)
			digit[index] = 1;
		else
			digit[index] = 0;

	}

	// re combine with reverse order
	for(index=0;index<=(m-1);index++)
	{
		result += digit[index] << (m-1-index);
	}

	return result;
}

BYTE WIMAX::BIT2BYTE(BIT * in) // MSB First
{
	BYTE out;

	out = (in[0] << 7) | (in[1] << 6) | (in[2] << 5) | (in[3] << 4) |
		  (in[4] << 3) | (in[5] << 2) | (in[6] << 1) | (in[7]     );

	return out;
}
void WIMAX::BYTE2BIT(BYTE in, BIT *out) // MSB First
{
	out[0] = (in & 0x80 ) >> 7;
	out[1] = (in & 0x40 ) >> 6;
	out[2] = (in & 0x20 ) >> 5;
	out[3] = (in & 0x10 ) >> 4;

	out[4] = (in & 0x08 ) >> 3;
	out[5] = (in & 0x04 ) >> 2;
	out[6] = (in & 0x02 ) >> 1;
	out[7] = (in & 0x01 )     ;
}

unsigned char WIMAX::Min(int *Data, unsigned char Number)
{
	unsigned int Index;
	int minimum;
	unsigned char minimumindex;

	minimum = Data[0];
	minimumindex = 0;

	for(Index = 0;Index<Number;Index++)
	{
		if(minimum > Data[Index])
		{
			minimumindex = Index;
			minimum = Data[Index];
		}
	}

	return minimumindex;
}

unsigned char WIMAX::Min(unsigned int *Data, unsigned char Number)
{
	unsigned int Index;
	unsigned int minimum;
	unsigned char minimumindex;

	minimum = Data[0];
	minimumindex = 0;

	for(Index = 0;Index<Number;Index++)
	{
		if(minimum > Data[Index])
		{
			minimumindex = Index;
			minimum = Data[Index];
		}
	}

	return minimumindex;
}

unsigned char WIMAX::Max(int *Data, unsigned char Number)
{
	unsigned int Index;
	int maximum;
	unsigned char maximumindex;

	maximum = Data[0];
	maximumindex = 0;

	for(Index = 0;Index<Number;Index++)
	{
		if(maximum < Data[Index])
		{
			maximumindex = Index;
			maximum = Data[Index];
		}
	}

	return maximumindex;
}

unsigned char WIMAX::Max(unsigned int *Data, unsigned char Number)
{
	unsigned int Index;
	unsigned int maximum;
	unsigned char maximumindex;

	maximum = Data[0];

	for(Index = 0;Index<Number;Index++)
	{
		if(maximum < (unsigned int)Data[Index])
		{
			maximumindex = Index;
			maximum = Data[Index];
		}
	}

	return maximumindex;
}
