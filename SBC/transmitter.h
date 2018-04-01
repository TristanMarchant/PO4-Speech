#pragma once
#include "globals.h"

struct encoderChunk {
	//TODO put variables in chunk
	short prevBufferEven[25];
	short prevBufferOdd[25];
	short prevBufferC0Even[7];
	short prevBufferC0Odd[7];
	short prevBufferC1Even[7];
	short prevBufferC1Odd[7];
};





void transmitter(short *buffer, struct encoderChunk * encoderLeftChunk, struct encoderChunk *encoderRightChunk, unsigned short encodedBuffer);
void analysis(short buffer[BUFFERSIZE], short subband1[BUFFERSIZE/2],  short subband2[BUFFERSIZE/2],  short subband3[BUFFERSIZE/2],  short subband4[BUFFERSIZE/2], struct encoderChunk * encoderChunk);

//the chunk variable should be updated regularily by the ADPCM encoder
void ADPCMencoder(short *subband1, short *subband2, short *subband3, short *subband4, struct encoderChunk * encoderChunk);

void ADPCMencoderSubband(short *subband, short prediction, short nbBits, short * codebook, short * encoderChunk);
void quantize(short * input, short * codebook, short * partition, short * quantized);

void ConvolutionStage1(short * inputEven, short * inputOdd, short * filterEven, short * filterOdd, long long * C0, long long * C1);
