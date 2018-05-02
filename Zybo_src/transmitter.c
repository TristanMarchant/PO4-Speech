#include "globals.h"
#include "transmitter.h"
#include "math.h"
#include "string.h"
#include "receiver.h"
#include <stdio.h>
#include <stdlib.h>

const short h0_even[16] = {-115,    205,    -305,    434,    -616,    918,    -1619,    7553,    2246,    -800,    387,    -187,    72,    0,    -48,    80};
const short h0_odd[16] = {80,    -48,    0,    72,    -187,    387,    -800,    2246,    7553,    -1619,    918,    -616,    434,    -305,    205,    -115};
const short h2_even[8] = {-406,    793,    -1636,    7715,    1967,    -468,    64,    163};
const short h2_odd[8] = {163,    64,    -468,    1967,    7715,    -1636,    793,    -406};

/* TRANSMITTER
 Transform the input signal in buffer to encoded subband signals, stored in encodedBuffer
 Inputs:    - buffer: the input buffer
            - encoderChunkLeft: structure that remembers values from previous buffers (left signal)
            - encoderChunkLeft: structure that remembers values from previous buffers (right signal)
 Outputs:   - encodedBuffer: the encoded subband signals are stored in this array
*/
void transmitter(short * buffer, struct encoderChunk * encoderChunkLeft, struct encoderChunk * encoderChunkRight, unsigned short * encodedBuffer)
{
	short leftSignal[BUFFERSIZE_2]; //avoid division by defining constants
	short rightSignal[BUFFERSIZE_2];
	short subband_l1[BUFFERSIZE_8];
	short subband_l2[BUFFERSIZE_8];
	short subband_l3[BUFFERSIZE_8];
	short subband_l4[BUFFERSIZE_8];
	short subband_r1[BUFFERSIZE_8];
	short subband_r2[BUFFERSIZE_8];
	short subband_r3[BUFFERSIZE_8];
	short subband_r4[BUFFERSIZE_8];
    
	// Split in left and right signal
	//for (int bufPos = 0; bufPos < BUFFERSIZE; bufPos += 2) {
	//	leftSignal[bufPos>>1] = buffer[bufPos]; //avoid division by changing /2 to >>1
	//	rightSignal[bufPos>>1] = buffer[bufPos + 1]; //TODO avoid divisionby changing /2 to >>1
	//}
	//loop unrolling
	leftSignal[0] = buffer[0]; rightSignal[0] = buffer[1];
	leftSignal[1] = buffer[2]; rightSignal[1] = buffer[3];
	leftSignal[2] = buffer[4]; rightSignal[2] = buffer[5];
	leftSignal[3] = buffer[6]; rightSignal[3] = buffer[7];
	leftSignal[4] = buffer[8]; rightSignal[4] = buffer[9];
	leftSignal[5] = buffer[10]; rightSignal[5] = buffer[11];
	leftSignal[6] = buffer[12]; rightSignal[6] = buffer[13];
	leftSignal[7] = buffer[14]; rightSignal[7] = buffer[15];
	leftSignal[8] = buffer[16]; rightSignal[8] = buffer[17];
	leftSignal[9] = buffer[18]; rightSignal[9] = buffer[19];
	leftSignal[10] = buffer[20]; rightSignal[10] = buffer[21];
	leftSignal[11] = buffer[22]; rightSignal[11] = buffer[23];
	leftSignal[12] = buffer[24]; rightSignal[12] = buffer[25];
	leftSignal[13] = buffer[26]; rightSignal[13] = buffer[27];
	leftSignal[14] = buffer[28]; rightSignal[14] = buffer[29];
	leftSignal[15] = buffer[30]; rightSignal[15] = buffer[31];
	leftSignal[16] = buffer[32]; rightSignal[16] = buffer[33];
	leftSignal[17] = buffer[34]; rightSignal[17] = buffer[35];
	leftSignal[18] = buffer[36]; rightSignal[18] = buffer[37];
	leftSignal[19] = buffer[38]; rightSignal[19] = buffer[39];
    
	/*ANALYSIS*/
	
    // LEFT
	analysis(leftSignal, subband_l1, subband_l2, subband_l3, subband_l4, encoderChunkLeft);
	// RIGHT
    analysis(rightSignal, subband_r1, subband_r2, subband_r3, subband_r4, encoderChunkRight);
	
    /*ENCODE*/
    
    // LEFT
    ADPCMencoder(subband_l1, subband_l2, subband_l3, subband_l4, encoderChunkLeft);
    // RIGHT
    ADPCMencoder(subband_r1, subband_r2, subband_r3, subband_r4, encoderChunkRight);
    
    /*BIT PACKING*/
    
    // in the first 5 compactly packed shorts, store per short 4 quantised samples from
    // subband_left1 (4 bits) - subband_left2 (4 bits) - subband_right1 (4 bits) - subband_right2 (4 bits)
    //for (int i = 0; i < 5; i++) { //TODO loop-unrolling
    //    encodedBuffer[i] = shiftAndCast4(subband_l1[i], 12) |
    //    shiftAndCast4(subband_l2[i], 8) |
    //    shiftAndCast4(subband_r1[i], 4) |
    //    shiftAndCast4(subband_r2[i], 0);
    //}
	//loop unrolling
	encodedBuffer[0] = shiftAndCast4(subband_l1[0], 12) |
		shiftAndCast4(subband_l2[0], 8) |
		shiftAndCast4(subband_r1[0], 4) |
		shiftAndCast4(subband_r2[0], 0);
	encodedBuffer[1] = shiftAndCast4(subband_l1[1], 12) |
		shiftAndCast4(subband_l2[1], 8) |
		shiftAndCast4(subband_r1[1], 4) |
		shiftAndCast4(subband_r2[1], 0);
	encodedBuffer[2] = shiftAndCast4(subband_l1[2], 12) |
		shiftAndCast4(subband_l2[2], 8) |
		shiftAndCast4(subband_r1[2], 4) |
		shiftAndCast4(subband_r2[2], 0);
	encodedBuffer[3] = shiftAndCast4(subband_l1[3], 12) |
		shiftAndCast4(subband_l2[3], 8) |
		shiftAndCast4(subband_r1[3], 4) |
		shiftAndCast4(subband_r2[3], 0);
	encodedBuffer[4] = shiftAndCast4(subband_l1[4], 12) |
		shiftAndCast4(subband_l2[4], 8) |
		shiftAndCast4(subband_r1[4], 4) |
		shiftAndCast4(subband_r2[4], 0);

    
    // in the next 3 compactly packed shorts, store data from the upper subbands
    encodedBuffer[5] = shiftAndCast2(subband_l3[0], 14) | shiftAndCast2(subband_l4[0], 12) | shiftAndCast2(subband_r3[0], 10) | shiftAndCast2(subband_r4[0], 8)
    | shiftAndCast2(subband_l3[1], 6) | shiftAndCast2(subband_l4[1], 4) | shiftAndCast2(subband_r3[1], 2) | shiftAndCast2(subband_r4[1], 0);
    encodedBuffer[6] = shiftAndCast2(subband_l3[2], 14) | shiftAndCast2(subband_l4[2], 12) | shiftAndCast2(subband_r3[2], 10) | shiftAndCast2(subband_r4[2], 8)
    | shiftAndCast2(subband_l3[3], 6) | shiftAndCast2(subband_l4[3], 4) | shiftAndCast2(subband_r3[3], 2) | shiftAndCast2(subband_r4[3], 0);
    encodedBuffer[7] = shiftAndCast2(subband_l3[4], 14) | shiftAndCast2(subband_l4[4], 12) | shiftAndCast2(subband_r3[4], 10) | shiftAndCast2(subband_r4[4], 8);
    
}

/*
Helper function for bit packing
*/
unsigned short shiftAndCast2(short value, short nbBits) {
	unsigned short result = value << 14;
	return (result >> (14 - nbBits));
}

/*
Helper function for bit packing
*/
unsigned short shiftAndCast4(short value, short nbBits) {
	unsigned short result = value << 12;
	return (result >> (12 - nbBits));
}

/* ANALYSIS
 Function that creates 4 subband signals
 Inputs:    - The chunk variable for previous results
            - buffer to split in subbands
 Outputs:   - the 4 subband signals
 */
void analysis(short *buffer, short *subband1, short *subband2, short *subband3, short *subband4, struct encoderChunk *encoderChunk)
{
  /* 1st Stage Analysis using h0 filter coefficients */
    
  short Even[BUFFERSIZE_4]; //avoid division by using BUFFERSIZE>>2 instead of BUFFERSIZE/4
  short Odd[BUFFERSIZE_4];
  long long C0[10];
  long long C1[10];
  
  // Split even and odd
  //for (int bufPos = 0; bufPos < BUFFERSIZE_2; bufPos += 2) {
  //  Odd[bufPos>>1] = buffer[bufPos]; //avoid division by using bufPos>>1 instead of bufPos/2
  //  Even[bufPos>>1] = buffer[bufPos + 1];
  //}
  //loop unrolling
  Odd[0] = buffer[0]; Even[0] = buffer[1];
  Odd[1] = buffer[2]; Even[1] = buffer[3];
  Odd[2] = buffer[4]; Even[2] = buffer[5];
  Odd[3] = buffer[6]; Even[3] = buffer[7];
  Odd[4] = buffer[8]; Even[4] = buffer[9];
  Odd[5] = buffer[10]; Even[5] = buffer[11];
  Odd[6] = buffer[12]; Even[6] = buffer[13];
  Odd[7] = buffer[14]; Even[7] = buffer[15];
  Odd[8] = buffer[16]; Even[8] = buffer[17];
  Odd[9] = buffer[18]; Even[9] = buffer[19];
    
  // Update the chunk
  for (int i = 0; i<25; i++) {
	  if (i<15) {
		  encoderChunk->prevBufferEven[i] = encoderChunk -> prevBufferEven[i+10];
		  encoderChunk->prevBufferOdd[i] = encoderChunk -> prevBufferOdd[i+10];
	  }
	  else {
		  encoderChunk->prevBufferEven[i] = Even[i-15];
		  encoderChunk->prevBufferOdd[i] = Odd[i-15];
	  }
  }
   
  // Execute first stage convolution
  ConvolutionStage1(encoderChunk->prevBufferEven,encoderChunk->prevBufferOdd,C0,C1);
    
  /* 2nd Stage Analysis using h2 filter coefficients */

  short C0_even[5];
  short C0_odd[5];
  short C1_even[5];
  short C1_odd[5];
  
  // Split even and odd
  //for (int bufPos = 0; bufPos < 10; bufPos += 2) { //TODO loop-unrolling
  //  C0_odd[bufPos>>1] = C0[bufPos];//TODO avoid division by changing bufPos/2 to bufPos>>1
  //  C1_odd[bufPos>>1] = C1[bufPos];
  //  C0_even[bufPos>>1] = C0[bufPos + 1];
  //  C1_even[bufPos>>1] = C1[bufPos + 1];
  //}
  //loop unrolling
  C0_odd[0] = C0[0];
  C1_odd[0] = C1[0];
  C0_even[0] = C0[1];
  C1_even[0] = C1[1];
  C0_odd[1] = C0[2];
  C1_odd[1] = C1[2];
  C0_even[1] = C0[3];
  C1_even[1] = C1[3];
  C0_odd[2] = C0[4];
  C1_odd[2] = C1[4];
  C0_even[2] = C0[5];
  C1_even[2] = C1[5];
  C0_odd[3] = C0[6];
  C1_odd[3] = C1[6];
  C0_even[3] = C0[7];
  C1_even[3] = C1[7];
  C0_odd[4] = C0[8];
  C1_odd[4] = C1[8];
  C0_even[4] = C0[9];
  C1_even[4] = C1[9];


    
  // Update the chunk
  //for (int i = 0; i<12; i++) {
  //	  if (i<7) {
  //		  encoderChunk->prevBufferC0Even[i] = encoderChunk -> prevBufferC0Even[i+5];
  //		  encoderChunk->prevBufferC0Odd[i] = encoderChunk -> prevBufferC0Odd[i+5];
  //		  encoderChunk->prevBufferC1Even[i] = encoderChunk -> prevBufferC1Even[i+5];
		//  encoderChunk->prevBufferC1Odd[i] = encoderChunk -> prevBufferC1Odd[i+5];
  //	  }
  //	  else {
  //		  encoderChunk->prevBufferC0Even[i] = C0_even[i-7];
  //		  encoderChunk->prevBufferC0Odd[i] = C0_odd[i-7];
  //		  encoderChunk->prevBufferC1Even[i] = C1_even[i-7];
		//  encoderChunk->prevBufferC1Odd[i] = C1_odd[i-7];
  //	  }
  //  }
  //loop unrolling
  encoderChunk->prevBufferC0Even[0] = encoderChunk->prevBufferC0Even[5];
  encoderChunk->prevBufferC0Odd[0] = encoderChunk->prevBufferC0Odd[5];
  encoderChunk->prevBufferC1Even[0] = encoderChunk->prevBufferC1Even[5];
  encoderChunk->prevBufferC1Odd[0] = encoderChunk->prevBufferC1Odd[5];

  encoderChunk->prevBufferC0Even[1] = encoderChunk->prevBufferC0Even[6];
  encoderChunk->prevBufferC0Odd[1] = encoderChunk->prevBufferC0Odd[6];
  encoderChunk->prevBufferC1Even[1] = encoderChunk->prevBufferC1Even[6];
  encoderChunk->prevBufferC1Odd[1] = encoderChunk->prevBufferC1Odd[6];

  encoderChunk->prevBufferC0Even[2] = encoderChunk->prevBufferC0Even[7];
  encoderChunk->prevBufferC0Odd[2] = encoderChunk->prevBufferC0Odd[7];
  encoderChunk->prevBufferC1Even[2] = encoderChunk->prevBufferC1Even[7];
  encoderChunk->prevBufferC1Odd[2] = encoderChunk->prevBufferC1Odd[7];

  encoderChunk->prevBufferC0Even[3] = encoderChunk->prevBufferC0Even[8];
  encoderChunk->prevBufferC0Odd[3] = encoderChunk->prevBufferC0Odd[8];
  encoderChunk->prevBufferC1Even[3] = encoderChunk->prevBufferC1Even[8];
  encoderChunk->prevBufferC1Odd[3] = encoderChunk->prevBufferC1Odd[8];

  encoderChunk->prevBufferC0Even[4] = encoderChunk->prevBufferC0Even[9];
  encoderChunk->prevBufferC0Odd[4] = encoderChunk->prevBufferC0Odd[9];
  encoderChunk->prevBufferC1Even[4] = encoderChunk->prevBufferC1Even[9];
  encoderChunk->prevBufferC1Odd[4] = encoderChunk->prevBufferC1Odd[9];

  encoderChunk->prevBufferC0Even[5] = encoderChunk->prevBufferC0Even[10];
  encoderChunk->prevBufferC0Odd[5] = encoderChunk->prevBufferC0Odd[10];
  encoderChunk->prevBufferC1Even[5] = encoderChunk->prevBufferC1Even[10];
  encoderChunk->prevBufferC1Odd[5] = encoderChunk->prevBufferC1Odd[10];

  encoderChunk->prevBufferC0Even[6] = encoderChunk->prevBufferC0Even[11];
  encoderChunk->prevBufferC0Odd[6] = encoderChunk->prevBufferC0Odd[11];
  encoderChunk->prevBufferC1Even[6] = encoderChunk->prevBufferC1Even[11];
  encoderChunk->prevBufferC1Odd[6] = encoderChunk->prevBufferC1Odd[11];

  encoderChunk->prevBufferC0Even[7] = C0_even[0];
  encoderChunk->prevBufferC0Odd[7] = C0_odd[0];
  encoderChunk->prevBufferC1Even[7] = C1_even[0];
  encoderChunk->prevBufferC1Odd[7] = C1_odd[0];

  encoderChunk->prevBufferC0Even[8] = C0_even[1];
  encoderChunk->prevBufferC0Odd[8] = C0_odd[1];
  encoderChunk->prevBufferC1Even[8] = C1_even[1];
  encoderChunk->prevBufferC1Odd[8] = C1_odd[1];

  encoderChunk->prevBufferC0Even[9] = C0_even[2];
  encoderChunk->prevBufferC0Odd[9] = C0_odd[2];
  encoderChunk->prevBufferC1Even[9] = C1_even[2];
  encoderChunk->prevBufferC1Odd[9] = C1_odd[2];

  encoderChunk->prevBufferC0Even[10] = C0_even[3];
  encoderChunk->prevBufferC0Odd[10] = C0_odd[3];
  encoderChunk->prevBufferC1Even[10] = C1_even[3];
  encoderChunk->prevBufferC1Odd[10] = C1_odd[3];

  encoderChunk->prevBufferC0Even[11] = C0_even[4];
  encoderChunk->prevBufferC0Odd[11] = C0_odd[4];
  encoderChunk->prevBufferC1Even[11] = C1_even[4];
  encoderChunk->prevBufferC1Odd[11] = C1_odd[4];

  // Execute stage 2 convolution
  ConvolutionStage2(encoderChunk->prevBufferC0Even,encoderChunk->prevBufferC0Odd,encoderChunk->prevBufferC1Even,encoderChunk->prevBufferC1Odd,subband1,subband2,subband3,subband4);
}
/*
 Function that performs the first stage convolution
 Inputs:    - arguments to do the convolution with
 outputs:   - the 2 results
 */
void ConvolutionStage1(short * inputEven, short * inputOdd, long long * C0, long long * C1) {
   
      long long X0[10] = {0};
	  long long Y0[10] = {0};
	  //loop interchange:
	  //It is better to have the loop with the smaller number of iterations as the outer loop
	  //and the one with the highest iteration count as the innermost loop.
	  for (int i = 15; i<25; i++) {
	      for (int j=0; j<16;j++) {
			  X0 [i-15] += inputEven[i-j]*h0_even[j];
			  Y0 [i-15] += inputOdd[i-j]*h0_odd[j];
	      }

	      C0[i-15] = (X0[i - 15] + Y0[i - 15]) >> 14; //(X0[i-15]+Y0[i-15])/pow(2,14) changed to (X0[i - 15] + Y0[i - 15]) >> 14, to avoid division
	      C1[i-15] = (X0[i - 15] - Y0[i - 15]) >> 14; //(X0[i - 15] - Y0[i - 15]) >> 14 changed to (X0[i - 15] - Y0[i - 15]) >> 14, to avoid division
	   }
}

/*
 Function that performs the second stage convolution
 Inputs:    - arguments to do the convolution with
 outputs:   - the 4 results
 */
void ConvolutionStage2(short * C0Even, short * C0Odd, short * C1Even, short * C1Odd, short * subband1, short * subband2, short * subband3, short * subband4) {
    
    long long XC0[5] = {0};
	long long YC0[5] = {0};
	long long XC1[5] = {0};
	long long YC1[5] = {0};
	//loop interchange:
	//It is better to have the loop with the smaller number of iterations as the outer loop
	//and the one with the highest iteration count as the innermost loop.
	  for (int i = 7; i<12; i++) {
	      for (int j=0; j<8;j++) {
		    XC0[i-7] += C0Even[i-j]*h2_even[j];
		    YC0[i-7] += C0Odd[i-j]*h2_odd[j];
		    XC1[i-7] += C1Even[i-j]*h2_even[j];
		    YC1[i-7] += C1Odd[i-j]*h2_odd[j];
		  }

	      subband1[i-7] = (XC0[i - 7] + YC0[i - 7]) >> 14; //(XC0[i - 7] + YC0[i - 7]) / (pow(2, 14)) changed to (XC0[i - 7] + YC0[i - 7]) >> 14, to avoid division
	      subband2[i-7] = (XC0[i - 7] - YC0[i - 7]) >> 14; //(XC0[i - 7] - YC0[i - 7]) / (pow(2, 14)) changed to (XC0[i - 7] - YC0[i - 7]) >> 14, to avoid division
	      subband3[i-7] = (XC1[i - 7] + YC1[i - 7]) >> 14; //(XC1[i - 7] + YC1[i - 7]) / (pow(2, 14)) changed to (XC1[i - 7] + YC1[i - 7]) >> 14, to avoid division
	      subband4[i-7] = (XC1[i - 7] - YC1[i - 7]) >> 14; //(XC1[i - 7] - YC1[i - 7]) / (pow(2, 14)) changed to (XC1[i - 7] - YC1[i - 7]) >> 14, to avoid division
	   }
}

/* ADPCMENCODER
encodes all subband signals using ADPCM
the result will be stored at the pointers used for the input
a chunk is needed to contain values of the previous buffer
*/
void ADPCMencoder(short *subband1, short *subband2, short *subband3, short *subband4, struct encoderChunk * encoderChunk) {
	short codebook4[16] = codebook_4;
    
	/* Encode subband1 */
	ADPCMencoderSubband(subband1, mu_1, &encoderChunk->prediction1, codebook4, 15, &encoderChunk->stepsize1, encoderChunk->deltaPrimeArray1, phi_4_updated);
    
	/* Encode subband2 */
	ADPCMencoderSubband(subband2, mu_2, &encoderChunk->prediction2, codebook4, 15, &encoderChunk->stepsize2, encoderChunk->deltaPrimeArray2, phi_4_updated);
    
    short codebook2[4] = codebook_2;
	/* Encode subband3 */
	ADPCMencoderSubband(subband3, mu_3, &encoderChunk->prediction3, codebook2, 3, &encoderChunk->stepsize3, encoderChunk->deltaPrimeArray3, phi_2_updated);
    
	/* Encode subband4 */
	ADPCMencoderSubband(subband4, mu_4, &encoderChunk->prediction4, codebook2, 3, &encoderChunk->stepsize4, encoderChunk->deltaPrimeArray4, phi_2_updated);
}

/*
encode only one given subband
different input parameters are used:
constant for each buffer -> mu, codebook, codebookSize, stepsizeOptFP
updated in the chunk -> prediction, stepsize, deltaPrimeArray
the result will be stored at the pointer used for the input
*/
void ADPCMencoderSubband(short * subbandSignal, short mu,
	short *prediction, short * codebook, short codebookSize, short *stepsize,
	short * deltaPrimeArray, short phi_updated) {
	int delta;
	short deltaPrime;

	//iterate over all samples in this subband signal
	for (int i = 0; i < BUFFERSIZE_8; i++) { //TODO [low] loop unrolling
		//calculate delta
		delta = (subbandSignal[i] - *prediction) << 1; //change *2 to <<1 to avoid multiplication

		//quantize delta, store result back into the input array
		subbandSignal[i] = quantize(delta, codebook, codebookSize, *stepsize);

		//calculate delta prime
		deltaPrime = subbandSignal[i] * *stepsize;

		//update the array of deltaPrimes in the chunk
		//for (int j = 1; j < nbDelta; j++) {
		//	deltaPrimeArray[j - 1] = deltaPrimeArray[j];
		//}
		//loop unrolling
		deltaPrimeArray[0] = deltaPrimeArray[1];
		deltaPrimeArray[1] = deltaPrimeArray[2];
		deltaPrimeArray[2] = deltaPrimeArray[3];
		deltaPrimeArray[3] = deltaPrimeArray[4];
		deltaPrimeArray[4] = deltaPrimeArray[5];
		deltaPrimeArray[5] = deltaPrimeArray[6];
		deltaPrimeArray[6] = deltaPrimeArray[7];
		deltaPrimeArray[7] = deltaPrimeArray[8];
		deltaPrimeArray[8] = deltaPrimeArray[9];
		deltaPrimeArray[9] = deltaPrime;

		//update the stepsize
		*stepsize = calculateStepsize(deltaPrimeArray, phi_updated);
		if (*stepsize == 0) {
			*stepsize = 1;
		}

		//calculate the new prediction
		*prediction = (mu * (deltaPrime + *prediction)) >> 15; //mu * (deltaPrime + *prediction) / pow(2, 15) changed to (mu * (deltaPrime + *prediction)) >> 15
	}
}

/*
helper function for ADPCMencoderSubband
quantizes the given value and returns the result
*/
short quantize(int value, short* codebook, short codebookSize, short stepsize) { // TESTED WORKS! MAYBE SOMETHING WRONG
	int lowerBound = ((codebook[0]<<1) + 1) * stepsize; //(codebook[0] + 0.5) * stepsize * 2 changed to ((codebook[0]<<1) + 1) * stepsize, to remove double 
	int upperBound = ((codebook[codebookSize]<<1) - 3) * stepsize; //(codebook[codebookSize] - 1.5) * stepsize * 2 changed to ((codebook[codebookSize]<<1) - 3) * stepsize, to remove double

	int i = 0;
	int temp = stepsize << 1;
	for (int bound = lowerBound; bound < upperBound; bound += temp) {
		if (value <= bound) {
			return codebook[i];
		}
        i++;
	}
	return codebook[i-1];
}

/*
helper function for ADPCMencoderSubband
calculates the new stepsize based on the previous values of delta prime (stored in deltaPrimeArray)
and on phi (cfr. matlab implementation)
*/
short calculateStepsize(short* deltaPrimeArray, short phi_updated) {
	//use only phi_updated values and remove nbBits parameter
	long long std;
	std = calculateStd(deltaPrimeArray) >> 1; //calculateStd(deltaPrimeArray)/pow(2,9)*pow(2,8) changed to calculateStd(deltaPrimeArray) >> 1

	return  (phi_updated * std) >> 16; //(phi_updated * std / pow(2, 16)) changed to (phi_updated * std) >> 16, to avoid division
}


/*
helper function for calculateStepsize
calculate the standard deviation of deltaPrimeArray (size: nbDelta)
does not include a change to floating point
*/
long long calculateStd(short* deltaPrimeArray) {
	long mean = 0;
	for (int i = 0; i < nbDelta; i++) { //TODO [low] loop unrolling -> too much dependencies
		mean += deltaPrimeArray[i];
	}
	mean = (mean / nbDelta); //TODO [low] avoid division -> change nbDelta to 8 or 16, to be able to shift (might affect performance)

	unsigned long long std = 0;
	unsigned long long temp;
	//loop termination: it is better to have integer loop counters which end at 0
	// 9 = nbDelta - 1;
	for (int i = 9; i > 0; i--) { //TODO [low] loop unrolling -> too much dependencies
		temp = labs(deltaPrimeArray[i] - mean);
		std += temp * temp; //pow(llabs(deltaPrimeArray[i] - mean), 2) changed to llabs(deltaPrimeArray[i] - mean) * llabs(deltaPrimeArray[i] - mean) * llabs(deltaPrimeArray[i] - mean) * llabs(deltaPrimeArray[i] - mean) to avoid pow
	}

	//TODO [low] avoid division -> change nbDelta to 9, to be able to shift (might affect performance)
	std = (std << 18) / 9; //pow(2, 18) * std / (nbDelta - 1) changed to (std<<18) / 9, to avoid multiplication

	//std = sqrt(std); //avoid sqrt as it uses a double
	std = calculateSqrt(std);

	return std;
}


//calculate the square root of the given value using bisection
//TODO faster algorithm (very usefull as code is used very often)
unsigned long long calculateSqrt(unsigned long long value) {
	unsigned long long hi = value;
	unsigned long long lo = 0;
	unsigned long long mid = (hi>>1) + (lo>>1);
	unsigned long long mid2 = mid * mid;
	while ((lo < hi - 1) && (mid2 != value)) {
		if (mid2 < value) {
			lo = mid;
		}
		else {
			hi = mid;
		}
		mid = (hi + lo) >> 1;
		mid2 = mid * mid;
	}
	return mid;
}
