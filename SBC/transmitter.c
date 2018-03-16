#include "globals.h"


const short h0_odd[16] = {320, -192, -1, 289, -749, 1546, -3201, 8982, 30212, -6476, 3670, -2465, 1735, -1120, 819, -461};
const short h0_even[16] = {-461,	819,	 -1220,	1735, -2465, 3670, -6476, 30212,	8982, -3201,	1546, -749, 289,	 -1, -192, 320};
const short h2_odd[8] = {650, 255, -1872, 7869, 30860, -6543, 3170, -1624};
const short h2_even[8] = {-1624,	3170	, -6543, 30860, 7869, -1872,	255, 650};


void analysis(short *buffer, short *subband1, short *subband2, short *subband3, short *subband4);
void ADPCMencoder(short *subbandSignal, short nbOfQuantBits);

/* transform the input signal in buffer to an encoded signal, stored in encodedBuffer */
void transmitter(short *buffer, unsigned short *encodedBuffer)
{
	//split in left and right signal
	short leftSignal[BUFFERSIZE/2];
	short rightSignal[BUFFERSIZE/2];
	short subband1[BUFFERSIZE / 2];
	short subband2[BUFFERSIZE / 2];
	short subband3[BUFFERSIZE / 2];
	short subband4[BUFFERSIZE / 2];
	for (int bufPos = 0; bufPos < BUFFERSIZE; bufPos += 2) {
		//TODO check if interleaved buffer starts with left sample
		leftSignal[bufPos / 2] = buffer[bufPos];
		rightSignal[bufPos / 2] = buffer[bufPos + 1];
	}
	
	/*LEFT*/
	//analysis left
	analysis(leftSignal, subband1, subband2, subband3, subband4);
	//ADPCM left
	ADPCMencoder(subband1, subband2, subband3, subband4);

	/*RIGHT*/
	//analysis right
	analysis(rightSignal, subband1, subband2, subband3, subband4);
	//ADPCM right
	ADPCMencoder(subband1, subband2, subband3, subband4);

	//TODO bit shifting into encodedBuffer
}

/* creates 4 subband signals */
void analysis(short *buffer, short *subband1, short *subband2, short *subband3, short *subband4)
{
	//TODO

}

/* encodes all subband signals 
inputs: subband signals to encode
outputs: 
*/
void ADPCMencoder(short *subband1, short *subband2, short *subband3, short *subband4)
{
	short initPrediction;
	short initStepsize;
	short mu;
	short prevInput;
	// per subband extract from chunk variable: 
	// last sample, last stepsize from previous buffer

	// for each subband, call the ADPCMencoderSubband function
	initPrediction = mu * prevInput;
	ADPCMencoderSubband(subband1, initPrediction, initStepsize, 4);
}

/* encodes only one subband
input: pointer to the start of an array containing the subband signal
       the first prediction value, will be updated
	   the first stepsize value, will be updated
	   the number of quantisation bits to use
*/
void ADPCMencoderSubband(short *subband, short prediction, short stepsize, short nbBits)
{
	short delta;
	// encode all samples
	for (int i = 0; i < BUFFERSIZE / 2; i++) {
		delta = (subband[i] - prediction)*2;
		//TODO quantize delta
		//TODO calculate delta prime
		//TODO update the stepsize
		//TODO update the prediction
	}
}
