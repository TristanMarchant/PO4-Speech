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
    short deltaPrimeArray1[nbDelta];
    short deltaPrimeArray2[nbDelta];
    short deltaPrimeArray3[nbDelta];
    short deltaPrimeArray4[nbDelta];
    short prevoutput1;
    short prevoutput2;
    short prevoutput3;
    short prevoutput4;
};

void receiver(unsigned short encodedBuffer[8], short *buffer, struct decoderChunk * decoderChunkLeft, struct decoderChunk * decoderChunkRight);

void synthesis(short subband1[BUFFERSIZE/8], short subband2[BUFFERSIZE/8], short subband3[BUFFERSIZE/8], short subband4[BUFFERSIZE/8], struct decoderChunk * decoderChunk, short result[BUFFERSIZE/2]);

void ConvolutionStage1dec(short s20[BUFFERSIZE/8],short s21[BUFFERSIZE/8], short s22[BUFFERSIZE/8], short s23[BUFFERSIZE/8], long long f20[BUFFERSIZE/8], long long f21[BUFFERSIZE/8], long long f22[BUFFERSIZE/8], long long f23[BUFFERSIZE/8]);
void ConvolutionStage2dec(short s10[BUFFERSIZE/4], short s11[BUFFERSIZE/4], long long f10[BUFFERSIZE/4], long long f11[BUFFERSIZE/4]);

void ADPCMdecoder(short subband1[BUFFERSIZE/8], short subband2[BUFFERSIZE/8], short subband3[BUFFERSIZE/8], short subband4[BUFFERSIZE/8], struct decoderChunk * decoderChunk);

void ADPCMdecoderSubband(short subbandSignal[BUFFERSIZE/8], short mu, short * stepsize, short deltaPrimeArray[nbDelta], short phi_updated, short * prevoutput);
short twosComplement2Bits(unsigned short value);
short twosComplement4Bits(unsigned short value);
