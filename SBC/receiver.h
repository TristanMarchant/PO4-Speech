#pragma once
#include "globals.h"

void receiver(unsigned short encodedBuffer[8], short *buffer);

struct decoderChunk {
	short prevBufferS10[25];
	short prevBufferS11[25];
	short prevBufferS20[12];
	short prevBufferS21[12];
	short prevBufferS22[12];
	short prevBufferS23[12];
};

void synthesis(short subband1[5], short subband2[5], short subband3[5], short subband4[5], struct decoderChunk * decoderChunk, short result[20], int test);

void ConvolutionStage1dec(short s20[5],short s21[5], short s22[5], short s23[5], long long f20[5], long long f21[5], long long f22[5], long long f23[5], int test);
void ConvolutionStage2dec(short s10[10], short s11[10], long long f10[10], long long f11[10], int test);
