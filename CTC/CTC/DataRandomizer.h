/*
 * DataRandomizer.h
 *
 *  Created on: 2012. 4. 7.
 *      Author: Peter
 */

#ifndef DATARANDOMIZER_H_
#define DATARANDOMIZER_H_

#include "WIMAX.h"

class FECEncoder;

class DataRandomizer : WIMAX {
public:
	DataRandomizer(FECEncoder *FECEncoder);
	virtual ~DataRandomizer();

	void Padding(void);
	void DataRandomize(void);
	void GetCRC16(void);
	void Fragment(void);

public:
	FECEncoder * m_FECEncoder;
};

#endif /* DATARANDOMIZER_H_ */
