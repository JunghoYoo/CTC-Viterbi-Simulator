/*
 * Demodulator.h
 *
 *  Created on: 2012. 12. 15.
 *      Author: Peter
 */

#ifndef DEMODULATOR_H_
#define DEMODULATOR_H_

#include "WIMAX.h"
#include "SIGNAL.h"

class Demodulator: WIMAX  {
public:
	Demodulator();
	virtual ~Demodulator();

	void Demodulate(WIMAXMCS mcs);

	SIGNAL 	m_datain[MAXFECBLOCKSIZE];
	BYTE 	m_dataout[MAXFECBLOCKSIZE];

	unsigned int m_SIGNALSize;
	unsigned int m_LLRSize;

private:
	void QPSKDemodulate (SIGNAL , BYTE *);
	void QAM16Demodulate(SIGNAL , BYTE *);
	void QAM64Demodulate(SIGNAL , BYTE *);
};

#endif /* DEMODULATOR_H_ */
