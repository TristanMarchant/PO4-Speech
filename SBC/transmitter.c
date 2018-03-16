#include "globals.h"

void analysis(short *buffer, short *subband1, short *subband2, short *subband3, short *subband4);
void ADPCMencoder();


/* encode */
void transmitter(short *buffer, unsigned short *encodedBuffer)
{
	//split in left and right signal
	short leftSignal[BUFFERSIZE/2];
	short rightSignal[BUFFERSIZE/2];
	for (int bufPos = 0; bufPos < BUFFERSIZE; bufPos += 2) {
		//TODO check if interleaved buffer starts with left sample
		leftSignal[bufPos / 2] = buffer[bufPos];
		rightSignal[bufPos / 2] = buffer[bufPos + 1];
	}
	
	/*LEFT*/
	//analysis left
	analysis(leftSignal);
	//TODO ADPCM left
	ADPCMencoder();

	/*RIGHT*/
	//analysis right
	analysis(rightSignal);
	//TODO ADPCM right
	ADPCMencoder();

	//TODO bit shifting into encodedBuffer
}

/* creates 4 subband signals */
void analysis(short *buffer, short *subband1, short *subband2, short *subband3, short *subband4)
{
	//TODO

}

/* encodes one subband signal 
inputs: subband signal to encode, nb of quantisation bits to use
*/
void ADPCMencoder() 
{
	//TODO
}