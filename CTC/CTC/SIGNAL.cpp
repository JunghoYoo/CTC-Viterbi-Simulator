/*
 * SIGNAL.cpp
 *
 *  Created on: 2012. 4. 22.
 *      Author: Peter
 */

#include "SIGNAL.h"

#define POW2TO12 4096
#define MASK16 0xffff
#define MAX16  32767
#define MIN16 -32768

SIGNAL::SIGNAL() {
}

SIGNAL::SIGNAL(float real, float imag) {
	// TODO Auto-generated constructor stub
	this->real = real;
	this->imag = imag;
}

SIGNAL::~SIGNAL() {
	// TODO Auto-generated destructor stub
}

SIGNAL SIGNAL::operator + (SIGNAL A)
{
	SIGNAL result;

	result.real = this->real + A.real;
	result.imag = this->imag + A.imag;

	return result;
}

SIGNAL SIGNAL::operator - (SIGNAL A)
{
	SIGNAL result;

	result.real = this->real - A.real;
	result.imag = this->imag - A.imag;

	return result;
}

SIGNAL SIGNAL::operator * (SIGNAL A)
{
	SIGNAL result;

	result.real = this->real * A.real - this->imag * A.imag;
	result.imag = this->real * A.imag + this->imag * A.real;

	return result;
}

bool SIGNAL::operator < (SIGNAL A)
{
	if(A.real == 0)
	{
		if(this->imag < A.imag )
			return true;
		else
			return false;
	}
	else if(A.imag == 0)
	{
		if(this->real < A.real )
			return true;
		else
			return false;
	}
	else
		return false;
}

bool SIGNAL::operator <= (SIGNAL A)
{
	if(A.real == 0)
	{
		if(this->imag <= A.imag )
			return true;
		else
			return false;
	}
	else if(A.imag == 0)
	{
		if(this->real <= A.real )
			return true;
		else
			return false;
	}
	else
		return false;
}

bool SIGNAL::operator > (SIGNAL A)
{
	if(A.real == 0)
	{
		if(this->imag > A.imag )
			return true;
		else
			return false;
	}
	else if(A.imag == 0)
	{
		if(this->real > A.real )
			return true;
		else
			return false;
	}
	else
		return false;
}

bool SIGNAL::operator >= (SIGNAL A)
{
	if(A.real == 0)
	{
		if(this->imag >= A.imag )
			return true;
		else
			return false;
	}
	else if(A.imag == 0)
	{
		if(this->real >= A.real )
			return true;
		else
			return false;
	}
	else
		return false;
}

bool SIGNAL::operator == (SIGNAL A)
{
	if(A.real == 0)
	{
		if(this->imag == A.imag )
			return true;
		else
			return false;
	}
	else if(A.imag == 0)
	{
		if(this->real == A.real )
			return true;
		else
			return false;
	}
	else
		return false;
}
