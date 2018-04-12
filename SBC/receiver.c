#include "globals.h"
#include "receiver.h"
#include "transmitter.h"
#include "math.h"
#include <stdio.h>
/*const int f2_odd[8] = {1300, 509, -3745, 15738, 61720, -13085, 6341, -3248};
const int f2_even[8] = {-3248, 6341, -13085,	61720, 15738, -3745, 509, 1300};
const int f0_odd[16] = {639,	-385, -2, 578, -1499, 3093, -6403, 17965, 60424,	-12952,	7340, -4929,	 3471, -2439, 1638, -922};
const int f0_even[16] = {-922, 1638,	-2439, 3471, -4929, 7340, -12952, 60424, 17965, -6403, 3093, -1499, 578, -2, -385, 639};
*/
const short f2_even[8] = {-812,    1585,    -3271,    15430,    3935,    -936,    127,    325};
const short f2_odd[8] = {325,    127,    -936,    3935,    15430,    -3271,    1585,    -812};
const short f0_even[16] = {-231,    409,    -610,    868,    -1232,    1835,    -3238,    15106,    4491,    -1601,    773,    -375,    145,    0,    -96,    160};
const short f0_odd[16] = {160,    -96,    0,    145,    -375,    773,    -1601,    4491,    15106,    -3238,    1835,    -1232,    868,    -610,    409,    -231};
/* decode */
void receiver(unsigned short encodedBuffer[8], short *buffer)
{
	//TODO inverse ADPCM left
	//TODO inverse ADPCM right
	//TODO synthesis
}
void synthesis(short * subband1, short * subband2, short * subband3, short * subband4, struct decoderChunk * decoderChunk, short * result, int test){

	short s20[5] = {0};
	short s21[5] = {0};
	short s22[5] = {0};
	short s23[5] = {0};

	for (int i = 0; i < 5; i++) {
		s20[i] = subband1[i] + subband2[i];
		s21[i] = subband1[i] - subband2[i];
		s22[i] = subband3[i] + subband4[i];
		s23[i] = subband3[i] - subband4[i];
       
	}
	// Update chunk
	for (int i = 0; i<12; i++) {
	  if (i<7) {
		  decoderChunk->prevBufferS20[i] = decoderChunk -> prevBufferS20[i+5];
		  decoderChunk->prevBufferS21[i] = decoderChunk -> prevBufferS21[i+5];
		  decoderChunk->prevBufferS22[i] = decoderChunk -> prevBufferS22[i+5];
		  decoderChunk->prevBufferS23[i] = decoderChunk -> prevBufferS23[i+5];
          
	  }
	  else {
		  decoderChunk->prevBufferS20[i] = s20[i-7];
		  decoderChunk->prevBufferS21[i] = s21[i-7];
		  decoderChunk->prevBufferS22[i] = s22[i-7];
		  decoderChunk->prevBufferS23[i] = s23[i-7];
	  }
	}
    
	long long f20[5] = {0};
	long long f21[5] = {0};
	long long f22[5] = {0};
	long long f23[5] = {0};

	// Execute convolution stage 1
	ConvolutionStage1dec(decoderChunk->prevBufferS20,decoderChunk->prevBufferS21,decoderChunk->prevBufferS22,decoderChunk->prevBufferS23,f20,f21,f22,f23, test);

	short result1[10] = {0};
	short result2[10] = {0};

	for (int i = 0; i<10 ; i+=2) {
		result1[i] = f21[i/2];
		result1[i+1] = f20[i/2];
		result2[i] = f23[i/2];
		result2[i+1] = f22[i/2];
        /*if (test ==1) {
            printf("result1 %d \n", result1[i]);
            printf("result1 %d \n", result1[i+1]);
        }*/
	}
    
	short s10[10] = {0};
	short s11[10] = {0};

	for (int i = 0; i < 10; i++) { // WAUW 5 ipv 10
		s10[i] = result1[i] + result2[i];
		s11[i] = result1[i] - result2[i];
	}
	//Convstage2

	//Update the chunk
	for (int i = 0; i<25; i++) {
		if (i<15) {
			decoderChunk->prevBufferS10[i] = decoderChunk -> prevBufferS10[i+10];
			decoderChunk->prevBufferS11[i] = decoderChunk -> prevBufferS11[i+10];
		}
		else {
			decoderChunk->prevBufferS10[i] = s10[i-15];
			decoderChunk->prevBufferS11[i] = s11[i-15];
		}
	}

	long long f10[10] = {0};
	long long f11[10] = {0};

	ConvolutionStage2dec(decoderChunk->prevBufferS10,decoderChunk->prevBufferS11,f10,f11, test);

	for (int i = 0; i<20; i+=2) {
		result[i] = f11[i/2];
		result[i+1] = f10[i/2];
	}
    

}

void ConvolutionStage1dec(short * s20,short * s21, short * s22, short * s23, long long * f20, long long * f21, long long * f22, long long * f23, int test) {
	  for (int i = 7; i<12; i++) {
		  for (int j=0; j<8;j++) {
			f21 [i-7] += s21[i-j]*f2_even[j];
			f20 [i-7] += s20[i-j]*f2_odd[j];
			f23 [i-7] += s23[i-j]*f2_even[j];
			f22 [i-7] += s22[i-j]*f2_odd[j];
              
		  }
          f21[i-7] = f21[i-7]/pow(2,14);
          f20[i-7] = f20[i-7]/pow(2,14);
          f23[i-7] = f23[i-7]/pow(2,14);
          f22[i-7] = f22[i-7]/pow(2,14);
        
	  }
}

void ConvolutionStage2dec(short * s10, short * s11, long long * f10, long long * f11, int test) {
	  for (int i = 15; i<25; i++) {
		  for (int j=0; j<16;j++) {
			  f11[i-15] += s11[i-j]*f0_even[j];
			  f10[i-15] += s10[i-j]*f0_odd[j];
		  }
          f11[i-15] = f11[i-15]/pow(2,14);
          f10[i-15] = f10[i-15]/pow(2,14);
         
	  }
}
