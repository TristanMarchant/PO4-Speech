#pragma once
#include "globals.h"

void transmitter(short buffer[BUFFERSIZE], unsigned short *encodedBuffer);
void analysis(short *buffer, short *subband1, short *subband2, short *subband3, short *subband4);
void ADPCMencoder();
