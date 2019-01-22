/*
 * SIGNAL.h
 *
 *  Created on: 2012. 4. 22.
 *      Author: Peter
 */

#ifndef SIGNAL_H_
#define SIGNAL_H_

#include "WIMAX.h"

class SIGNAL: public WIMAX {
public:
	SIGNAL();
	SIGNAL(float real, float imag);
	virtual ~SIGNAL();

	SIGNAL operator + (SIGNAL A);
	SIGNAL operator - (SIGNAL A);
	SIGNAL operator * (SIGNAL A);

	bool operator < (SIGNAL A);
	bool operator <=(SIGNAL A);
	bool operator >=(SIGNAL A);
	bool operator > (SIGNAL A);
	bool operator == (SIGNAL A);

	float real;
	float imag;

};

#endif /* SIGNAL_H_ */
