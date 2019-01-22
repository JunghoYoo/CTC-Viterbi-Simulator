/*
 * AWGN.h
 *
 *  Created on: 2012. 10. 4.
 *      Author: SEC
 */

#ifndef AWGN_H_
#define AWGN_H_

#include "WIMAX.h"
#include "SIGNAL.h"
#include "Fixed.h"


class AWGN {
public:
	AWGN();
	virtual ~AWGN();

	void Add(SIGNAL * in, SIGNAL * out, unsigned long size);

	float m_SNRdB;
private:
	float Randn(void);
	float m_SNRlinear;
};

#endif /* AWGN_H_ */
