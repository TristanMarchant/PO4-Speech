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
	short leftSignal[BUFFERSIZE / 2];
	short rightSignal[BUFFERSIZE / 2];
	short subband_l1[BUFFERSIZE / 8];
	short subband_l2[BUFFERSIZE / 8];
	short subband_l3[BUFFERSIZE / 8];
	short subband_l4[BUFFERSIZE / 8];
	short subband_r1[BUFFERSIZE / 8];
	short subband_r2[BUFFERSIZE / 8];
	short subband_r3[BUFFERSIZE / 8];
	short subband_r4[BUFFERSIZE / 8];
    
	// Split in left and right signal
	for (int bufPos = 0; bufPos < BUFFERSIZE; bufPos += 2) {
		leftSignal[bufPos / 2] = buffer[bufPos];
		rightSignal[bufPos / 2] = buffer[bufPos + 1];
	}
    
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
    for (int i = 0; i < 5; i++) {
        encodedBuffer[i] = shiftAndCast4(subband_l1[i], 12) |
        shiftAndCast4(subband_l2[i], 8) |
        shiftAndCast4(subband_r1[i], 4) |
        shiftAndCast4(subband_r2[i], 0);
    }
    
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
    
  short Even[BUFFERSIZE/4];
  short Odd[BUFFERSIZE/4];
  long long C0[10];
  long long C1[10];
  
  // Split even and odd
  for (int bufPos = 0; bufPos < (BUFFERSIZE/2); bufPos += 2) {
    Odd[bufPos / 2] = buffer[bufPos];
    Even[bufPos / 2] = buffer[bufPos + 1];
  }
    
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
  for (int bufPos = 0; bufPos < 10; bufPos += 2) {
    C0_odd[bufPos / 2] = C0[bufPos];
    C1_odd[bufPos / 2] = C1[bufPos];
    C0_even[bufPos / 2] = C0[bufPos + 1];
    C1_even[bufPos / 2] = C1[bufPos + 1];
  }
    
  // Update the chunk
  for (int i = 0; i<12; i++) {
  	  if (i<7) {
  		  encoderChunk->prevBufferC0Even[i] = encoderChunk -> prevBufferC0Even[i+5];
  		  encoderChunk->prevBufferC0Odd[i] = encoderChunk -> prevBufferC0Odd[i+5];
  		  encoderChunk->prevBufferC1Even[i] = encoderChunk -> prevBufferC1Even[i+5];
		  encoderChunk->prevBufferC1Odd[i] = encoderChunk -> prevBufferC1Odd[i+5];
  	  }
  	  else {
  		  encoderChunk->prevBufferC0Even[i] = C0_even[i-7];
  		  encoderChunk->prevBufferC0Odd[i] = C0_odd[i-7];
  		  encoderChunk->prevBufferC1Even[i] = C1_even[i-7];
		  encoderChunk->prevBufferC1Odd[i] = C1_odd[i-7];
  	  }
    }

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
	  for (int i = 15; i<25; i++) {
	      for (int j=0; j<16;j++) {
			  X0 [i-15] += inputEven[i-j]*h0_even[j];
			  Y0 [i-15] += inputOdd[i-j]*h0_odd[j];
	      }
	      C0[i-15] = (X0[i-15]+Y0[i-15])/pow(2,14);
	      C1[i-15] = (X0[i-15]-Y0[i-15])/pow(2,14);
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
	  for (int i = 7; i<12; i++) {
	      for (int j=0; j<8;j++) {
		    XC0[i-7] += C0Even[i-j]*h2_even[j];
		    YC0[i-7] += C0Odd[i-j]*h2_odd[j];
		    XC1[i-7] += C1Even[i-j]*h2_even[j];
		    YC1[i-7] += C1Odd[i-j]*h2_odd[j];
		  }
	      subband1[i-7] = (XC0[i-7]+YC0[i-7]) / (pow(2, 14));
	      subband2[i-7] = (XC0[i-7]-YC0[i-7]) / (pow(2, 14));
	      subband3[i-7] = (XC1[i-7]+YC1[i-7]) / (pow(2, 14));
	      subband4[i-7] = (XC1[i-7]-YC1[i-7]) / (pow(2, 14));
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
	ADPCMencoderSubband(subband1, mu_1, 4, &encoderChunk->prediction1, codebook4, 15, &encoderChunk->stepsize1, encoderChunk->deltaPrimeArray1, phi_4);
    
	/* Encode subband2 */
	ADPCMencoderSubband(subband2, mu_2, 4, &encoderChunk->prediction2, codebook4, 15, &encoderChunk->stepsize2, encoderChunk->deltaPrimeArray2, phi_4);
    
    short codebook2[4] = codebook_2;
	/* Encode subband3 */
	ADPCMencoderSubband(subband3, mu_3, 2, &encoderChunk->prediction3, codebook2, 3, &encoderChunk->stepsize3, encoderChunk->deltaPrimeArray3, phi_2);
    
	/* Encode subband4 */
	ADPCMencoderSubband(subband4, mu_4, 2, &encoderChunk->prediction4, codebook2, 3, &encoderChunk->stepsize4, encoderChunk->deltaPrimeArray4, phi_2);
}

/*
encode only one given subband
different input parameters are used:
constant for each buffer -> mu, nbBits, codebook, codebookSize, stepsizeOptFP
updated in the chunk -> prediction, stepsize, deltaPrimeArray
the result will be stored at the pointer used for the input
*/
void ADPCMencoderSubband(short * subbandSignal, short mu, short nbBits,
	short *prediction, short * codebook, short codebookSize, short *stepsize,
	short * deltaPrimeArray, short stepsizeOptFP) {
	int delta;
	short deltaPrime;

	//iterate over all samples in this subband signal
	for (int i = 0; i < BUFFERSIZE/8; i++) {
		//calculate delta
		delta = (subbandSignal[i] - *prediction) * 2;

		//quantize delta, store result back into the input array
		subbandSignal[i] = quantize(delta, codebook, codebookSize, *stepsize);

		//calculate delta prime
		deltaPrime = subbandSignal[i] * *stepsize;

		//update the array of deltaPrimes in the chunk
		for (int j = 1; j < nbDelta; j++) {
			deltaPrimeArray[j - 1] = deltaPrimeArray[j];
		}
		deltaPrimeArray[nbDelta - 1] = deltaPrime;

		//update the stepsize
		*stepsize = calculateStepsize(deltaPrimeArray, stepsizeOptFP, nbBits);
		if (*stepsize == 0) {
			*stepsize = 1;
		}

		//calculate the new prediction
		*prediction = mu * (deltaPrime + *prediction) / pow(2, 15);
	}
}

/*
helper function for ADPCMencoderSubband
quantizes the given value and returns the result
*/
short quantize(int value, short* codebook, short codebookSize, short stepsize) { // TESTED WORKS! MAYBE SOMETHING WRONG
	int lowerBound = (codebook[0] + 0.5) * stepsize * 2;
	int upperBound = (codebook[codebookSize] - 1.5) * stepsize * 2;
	int i = 0;
	for (int bound = lowerBound; bound < upperBound; bound += (2*stepsize)) {
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
short calculateStepsize(short* deltaPrimeArray, short stepsizeOptFP, short nbBits) { // TESTED WORKS!
    short phi;
    phi = stepsizeOptFP + pow(2,8) * 0.3 / nbBits;
    
    long long std;
    std = calculateStd(deltaPrimeArray)/pow(2,9)*pow(2,8);
    
	return (phi * std / pow(2, 16));
}

/*
helper function for calculateStepsize
calculate the standard deviation of deltaPrimeArray (size: nbDelta)
does not include a change to floating point
*/
long long calculateStd(short* deltaPrimeArray) {   // TESTED WORKS!
	//TODO ask if long long can be used for this
	long long mean = 0;
	for (int i = 0; i < nbDelta; i++) {
		mean += deltaPrimeArray[i];
	}
	mean = (mean / nbDelta);

	long long std = 0;
	for (int i = 0; i < nbDelta; i++) {
		std += pow(llabs(deltaPrimeArray[i] - mean), 2);
        //printf("STD: %f \n", std); // ALL ZEROS NOT GOOD
	}
	std = pow(2,18) * std / (nbDelta-1);
	std = sqrt(std);

	return std;
}
