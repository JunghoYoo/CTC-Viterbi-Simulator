/*
 * WIMAX.h
 *
 *  Created on: 2012. 4. 5.
 *      Author: Peter
 */

#ifndef WIMAX_H_
#define WIMAX_H_

#define MAXBURSTSIZE 3000 //9000
#define MAXFECBLOCKSIZE 2048//8192 // 2*N bits
#define MAXFECBLOCKNUM 10

#define CTCSTATENUM  8
#define CTCINPUTNUM  4
#define CTCOUTPUTNUM 4

#define MODORDEROFQPSK 2
#define MODORDEROFQAM16 4
#define MODORDEROFQAM64 6

typedef char BIT;
typedef char BYTE;

// Variable Naming
//
// prefix :
//          b - bit  unit
//          B - byte unit
//          n - integer unit
//          s - struct unit
//          e - enum unit


class WIMAX {
public:
	WIMAX();
	virtual ~WIMAX();

	enum WIMAXMCS {QPSK_1_2, QPSK_3_4,
		QAM16_1_2, QAM16_3_4,
		QAM64_1_2, QAM64_2_3, QAM64_3_4, QAM64_5_6};

	enum WIMAXTDDMODE {DL, UL};

	enum HARQMODE {NO, CC, IR};

	int Floor(double data);
	int Floor(float data);
	int Ceil(double data);
	int Ceil(float data);

	unsigned int BRO (unsigned int y, unsigned char m); // Bitreverse CTC subblock interleave

	BYTE BIT2BYTE(BIT *); // MSB First
	void BYTE2BIT(BYTE, BIT *);// MSB First
	unsigned char Min( int *Data, unsigned char Number);
	unsigned char Min(unsigned int *Data, unsigned char Number);
	unsigned char Max( int *Data, unsigned char Number);
	unsigned char Max(unsigned int *Data, unsigned char Number);

};

#endif /* WIMAX_H_ */
