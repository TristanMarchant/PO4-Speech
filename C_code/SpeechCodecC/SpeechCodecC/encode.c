#include "globals.h"

/* encode */
void encode(short buffer[BUFFERSIZE])
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
	//TODO bit shifting
}

void analysis(short buffer[BUFFERSIZE / 2])
{
	//TODO
}