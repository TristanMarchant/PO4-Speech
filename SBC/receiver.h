#pragma once
#include "globals.h"

struct decoderChunk {
    short prevBufferS10[25];
    short prevBufferS11[25];
    short prevBufferS20[12];
    short prevBufferS21[12];
    short prevBufferS22[12];
    short prevBufferS23[12];
    short stepsize1;
    short stepsize2;
    short stepsize3;
    short stepsize4;
    short deltaPrimeArray1[10];
    short deltaPrimeArray2[10];
    short deltaPrimeArray3[10];
    short deltaPrimeArray4[10];
    short prevoutput1;
    short prevoutput2;
    short prevoutput3;
    short prevoutput4;
};

void receiver(unsigned short encodedBuffer[8], short *buffer, struct decoderChunk * decoderChunkLeft, struct decoderChunk * decoderChunkRight, int test);

void synthesis(short subband1[5], short subband2[5], short subband3[5], short subband4[5], struct decoderChunk * decoderChunk, short result[20], int test);

void ConvolutionStage1dec(short s20[5],short s21[5], short s22[5], short s23[5], long long f20[5], long long f21[5], long long f22[5], long long f23[5], int test);
void ConvolutionStage2dec(short s10[10], short s11[10], long long f10[10], long long f11[10], int test);

void ADPCMdecoder(short subband1[5], short subband2[5], short subband3[5], short subband4[5], struct decoderChunk * decoderChunk);

void ADPCMdecoderSubband(short subbandSignal[5], short mu, short n0_bits, short stepsize, short deltaPrimeArray[10], short PHI, short prevoutput);
