#pragma once
#include "globals.h"

void transmitter(short buffer[BUFFERSIZE], unsigned short *encodedBuffer);
void analysis(short *buffer, short *subband1, short *subband2, short *subband3, short *subband4);

//the chunk variable should be updatedregularily by the ADPCM encoder
void ADPCMencoder(short *subband1, short *subband2, short *subband3, short *subband4, struct chunk * encoderChunk)
  
void ADPCMencoderSubband(short *subband, short prediction, short nbBits, short * codebook, short * encodedChunk)
void quantize(short * input, short * codebook, short * partition, short * quantized){
