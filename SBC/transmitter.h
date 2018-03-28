#pragma once
#include "globals.h"

struct encoderChunk {
	//TODO put variables in chunk
	short prevBufferLeft[32];
	short prevBufferRight[32];
};


void transmitter(short *buffer, unsigned short *encodedBuffer, struct encoderChunk *encoderChunk);
void analysis(short *buffer, short *subband1, short *subband2, short *subband3, short *subband4);

//the chunk variable should be updatedregularily by the ADPCM encoder
void ADPCMencoder(short *subband1, short *subband2, short *subband3, short *subband4, struct encoderChunk * encoderChunk);

void ADPCMencoderSubband(short *subband, short prediction, short nbBits, short * codebook, short * encoderChunk);
void quantize(short * input, short * codebook, short * partition, short * quantized);
