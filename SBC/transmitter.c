#include "globals.h"

const short h0_odd[16] = {320, -192, -1, 289, -749, 1546, -3201, 8982, 30212, -6476, 3670, -2465, 1735, -1120, 819, -461};
const short h0_even[16] = {-461,	819,	 -1220,	1735, -2465, 3670, -6476, 30212,	8982, -3201,	1546, -749, 289,	 -1, -192, 320};
const short h2_odd[8] = {650, 255, -1872, 7869, 30860, -6543, 3170, -1624};
const short h2_even[8] = {-1624,	3170	, -6543, 30860, 7869, -1872,	255, 650};

void analysis(short buffer[BUFFERSIZE / 2]);

/* encode */
void transmitter(short buffer[BUFFERSIZE], unsigned short *encodedBuffer)
{
	//split in left and right signal
	short leftSignal[BUFFERSIZE/2];
	short rightSignal[BUFFERSIZE/2];
	for (int bufPos = 0; bufPos < BUFFERSIZE; bufPos += 2) {
		//TODO check if interleaved buffer starts with left sample
		leftSignal[bufPos / 2] = buffer[bufPos];
		rightSignal[bufPos / 2] = buffer[bufPos + 1];
	}
	
	//analysis left
	analysis(leftSignal);
	//analysis right
	analysis(rightSignal);

	//TODO ADPCM left
	//TODO ADPCM right
	//TODO bit shifting into encodedBuffer
}

void analysis(short buffer[BUFFERSIZE / 2])
{
	//TODO
}

void ADPCMencoder4() {}
