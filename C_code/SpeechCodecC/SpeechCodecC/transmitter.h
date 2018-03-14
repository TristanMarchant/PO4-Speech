#pragma once
#include "globals.h"

void transmitter(short buffer[BUFFERSIZE], unsigned short *encodedBuffer);
void analysis(short buffer[BUFFERSIZE / 2]);