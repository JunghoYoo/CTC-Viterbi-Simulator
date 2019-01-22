/*
 * Modulator.h
 *
 *  Created on: 2012. 4. 22.
 *      Author: Peter
 */

#ifndef MODULATOR_H_
#define MODULATOR_H_

#include "WIMAX.h"
#include "SIGNAL.h"

class Modulator: WIMAX {
public:
	Modulator();
	virtual ~Modulator();

	void Modulate(WIMAXMCS mcs);

	BIT 	m_datain[MAXFECBLOCKSIZE];
	SIGNAL 	m_dataout[MAXFECBLOCKSIZE];

	unsigned int m_BITSize;
	unsigned int m_SIGNALSize;

private:
	const SIGNAL BPSKModulate (BIT *);
	const SIGNAL QPSKModulate (BIT *);
	const SIGNAL QAM16Modulate(BIT *);
	const SIGNAL QAM64Modulate(BIT *);

};

#endif /* MODULATOR_H_ */
