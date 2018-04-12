#pragma once
#include "globals.h"
#include "receiver.h"
struct encoderChunk {
	short prevBufferEven[25];
	short prevBufferOdd[25];
	short prevBufferC0Even[12];
	short prevBufferC0Odd[12];
	short prevBufferC1Even[12];
	short prevBufferC1Odd[12];
};

void transmitter(short *buffer, struct encoderChunk * encoderLeftChunk, struct encoderChunk *encoderRightChunk, unsigned short encodedBuffer[8], int test, struct decoderChunk * decoderChunkLeft, struct decoderChunk * decoderChunkRight);
void analysis(short buffer[BUFFERSIZE], short subband1[BUFFERSIZE/8],  short subband2[BUFFERSIZE/8],  short subband3[BUFFERSIZE/8],  short subband4[BUFFERSIZE/8], struct encoderChunk * encoderChunk, int test);

////the chunk variable should be updated regularily by the ADPCM encoder
//void ADPCMencoder(short *subband1, short *subband2, short *subband3, short *subband4, struct encoderChunk * encoderChunk);
//
//void ADPCMencoderSubband(short *subband, short prediction, short nbBits, short * codebook, short * encoderChunk);
//void quantize(short * input, short * codebook, short * partition, short * quantized);

void ConvolutionStage1(short inputEven[25], short inputOdd[25], long long  C0[10], long long C1[10], int test);
void ConvolutionStage2(short C0Even[7], short C0Odd[7], short C1Even[7], short C1Odd[7], short subband1[BUFFERSIZE/8],  short subband2[BUFFERSIZE/8],  short subband3[BUFFERSIZE/8],  short subband4[BUFFERSIZE/8], int test);
