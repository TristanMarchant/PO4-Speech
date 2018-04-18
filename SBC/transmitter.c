#include "globals.h"
#include "transmitter.h"
#include "math.h"
#include "string.h"
#include "receiver.h"
#include <stdio.h>
#include <stdlib.h>
/*const short h0_odd[16] = {320, -192, -1, 289, -749, 1546, -3201, 8982, 30212, -6476, 3670, -2465, 1735, -1120, 819, -461};
const short h0_even[16] = {-461, 819, -1220, 1735, -2465, 3670, -6476, 30212, 8982, -3201, 1546, -749, 289, -1, -192, 320};
const short h2_odd[8] = {650, 255, -1872, 7869, 30860, -6543, 3170, -1624};
const short h2_even[8] = {-1624,3170, -6543, 30860, 7869, -1872,255, 650};
*/
const short h0_even[16] = {-115,    205,    -305,    434,    -616,    918,    -1619,    7553,    2246,    -800,    387,    -187,    72,    0,    -48,    80};
const short h0_odd[16] = {80,    -48,    0,    72,    -187,    387,    -800,    2246,    7553,    -1619,    918,    -616,    434,    -305,    205,    -115};
const short h2_even[8] = {-406,    793,    -1636,    7715,    1967,    -468,    64,    163};
const short h2_odd[8] = {163,    64,    -468,    1967,    7715,    -1636,    793,    -406};

/* transform the input signal in buffer to an encoded signal, stored in encodedBuffer */
void transmitter(short * buffer, struct encoderChunk * encoderChunkLeft, struct encoderChunk * encoderChunkRight, unsigned short * encodedBuffer, int test, struct decoderChunk * decoderChunkLeft, struct decoderChunk * decoderChunkRight)
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
    
	//split in left and right signal
	for (int bufPos = 0; bufPos < BUFFERSIZE; bufPos += 2) {
		leftSignal[bufPos / 2] = buffer[bufPos];
		rightSignal[bufPos / 2] = buffer[bufPos + 1];
	}
    
	/*LEFT*/
	//analysis left
	//analysis(leftSignal, subband_l1, subband_l2, subband_l3, subband_l4, &encoderChunkLeft);
	analysis(leftSignal, subband_l1, subband_l2, subband_l3, subband_l4, encoderChunkLeft,test);
	//ADPCM left
	ADPCMencoder(subband_l1, subband_l2, subband_l3, subband_l4, encoderChunkLeft);

	/*RIGHT*/
	//analysis right
	analysis(rightSignal, subband_r1, subband_r2, subband_r3, subband_r4, encoderChunkRight,test);
	//ADPCM right
	ADPCMencoder(subband_r1, subband_r2, subband_r3, subband_r4, &encoderChunkLeft);

	/*BIT PACKING*/
	//in the first 5 compactly packed shorts, store per short 4 quantised samples from
	// subband_left1 (4 bits) - subband_left2 (4 bits) - subband_right1 (4 bits) - subband_right2 (4 bits)
	for (int i = 0; i < 5; i++) {
		encodedBuffer[i] = shiftAndCast4(subband_l1[i], 12) |
						   shiftAndCast4(subband_l2[i], 8) |
						   shiftAndCast4(subband_r1[i], 4) |
						   shiftAndCast4(subband_r2[i], 0);
	}

	//in the next 3 compactly packed shorts, store data from the upper subbands
	encodedBuffer[5] = shiftAndCast2(subband_l3[0], 14) | shiftAndCast2(subband_l4[0], 12) | shiftAndCast2(subband_r3[0], 10) | shiftAndCast2(subband_r4[0], 8)
					 | shiftAndCast2(subband_l3[1], 6) | shiftAndCast2(subband_l4[1], 4) | shiftAndCast2(subband_r3[1], 2) | shiftAndCast2(subband_r4[1], 0);
	encodedBuffer[6] = shiftAndCast2(subband_l3[2], 14) | shiftAndCast2(subband_l4[2], 12) | shiftAndCast2(subband_r3[2], 10) | shiftAndCast2(subband_r4[2], 8)
					 | shiftAndCast2(subband_l3[3], 6) | shiftAndCast2(subband_l4[3], 4) | shiftAndCast2(subband_r3[3], 2) | shiftAndCast2(subband_r4[3], 0);
	encodedBuffer[7] = shiftAndCast2(subband_l3[4], 14) | shiftAndCast2(subband_l4[4], 12) | shiftAndCast2(subband_r3[4], 10) | shiftAndCast2(subband_r4[4], 8);

    /*if (test==1) {
        for (int i =0; i<5; i++) {
            printf("sb1: %d \n",subband_l1[i]);
        }
     
     }*/

    
    
	//// TESTING SYNTHESIS
	////short resultLeft[20] = {0};
	////short resultRight[20] = {0};
	///*struct decoderChunk decoderChunkLeft; // WTF MONGOOL
 //   memset(&decoderChunkLeft,0,sizeof(struct decoderChunk));
	//struct decoderChunk decoderChunkRight;
 //   memset(&decoderChunkRight,0,sizeof(struct decoderChunk));*/
 //   
 //   /*
	//synthesis(subband_l1,subband_l2,subband_l3,subband_l4,decoderChunkLeft,resultLeft,test);
	//synthesis(subband_r1,subband_r2,subband_r3,subband_r4,decoderChunkRight,resultRight,test);

	//for (int i = 0; i < 40; i+=2) {
	//	buffer[i] = resultLeft[i/2];
	//	buffer[i+1] = resultRight[i/2];
	//}*/
 //   // TEST FULL ENCODE
 //   ADPCMencoder(subband_l1, subband_l2, subband_l3, subband_l4, encoderChunkLeft, test);
 //   /*if (test==1) {
 //        for (int i = 0; i<5; i++) {
 //        printf("sb3: %d \n",subband_l3[i]);
 //        }
 //   }*/
 //   ADPCMencoder(subband_r1, subband_r2, subband_r3, subband_r4, encoderChunkRight,test);
 //       // TESTING ENCODE PARTS (WORKS)
 //   //short deltaPrimeArray[10] = {5,19,23,540,9,5,120,-4,0,300};
 //   //short haha;
 //   //short delta = 211;
 //   //short codebook[16] = {-8,-7,-6,-5,-4,-3,-2,-1,0,1,2,3,4,5,6,7};
 //   //haha = calculateStd(deltaPrimeArray);
 //   /*if (test == 1) {
 //       printf("hallo: %f \n", haha); // WORKS
 //   }*/
 //   
 //   /*if (test == 1) {
 //       haha = calculateStepsize(deltaPrimeArray,phi_4,4); // WORKS
 //       printf("hallo: %d \n", haha);
 //   }*/
 //   /*if (test ==1) {
 //       haha = quantize(delta,codebook,15,30); // WORKS
 //       printf("hallo: %d \n", haha);
 //   }*/
 //   // Decode LEFT
 //   ADPCMdecoder(subband_l1,subband_l2,subband_l3,subband_l4,decoderChunkLeft);
 //   // Decode RIGHT
 //   ADPCMdecoder(subband_r1,subband_r2,subband_r3,subband_r4,decoderChunkRight);
 //   
 //   short resultLeft[20];
 //   short resultRight[20];
 //   // Synthesis LEFT
 //   synthesis(subband_l1,subband_l2,subband_l3,subband_l4,decoderChunkLeft,resultLeft,test);
 //   // Synthesis RIGHT
 //   synthesis(subband_r1,subband_r2,subband_r3,subband_r4,decoderChunkRight,resultRight,test);
 //   for (int i = 0; i < 40; i+=2) {
 //       buffer[i] = resultLeft[i/2];
 //       buffer[i+1] = resultRight[i/2];
 //   }
    // EVERYTHING WORKS WITHOUT BITPACKING 
}

/*
Helper function for bit packing
*/
unsigned short shiftAndCast2(short value, short nbBits) {
	unsigned short result = value << 14;
	return (result >> (14-nbBits));
}

/*
Helper function for bit packing
*/
unsigned short shiftAndCast4(short value, short nbBits) {
	unsigned short result = value << 12;
	return (result >> (12 - nbBits));
}

/* creates 4 subband signals */
void analysis(short *buffer, short *subband1, short *subband2, short *subband3, short *subband4, struct encoderChunk *encoderChunk, int test)
{
  // 1st Stage Analysis using h0 filter coefficients
  short Even[BUFFERSIZE/4];
  short Odd[BUFFERSIZE/4];
  long long C0[10];
  long long C1[10];
  
  // Split even and odd
  for (int bufPos = 0; bufPos < (BUFFERSIZE/2); bufPos += 2) {
    Odd[bufPos / 2] = buffer[bufPos];
    Even[bufPos / 2] = buffer[bufPos + 1];
  }
    
  // Update the chunk (ik denk dat dit werkt)
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
   
    
    /*if (test ==1) {
        for (int i = 0; i<25; i++) {
            printf("prevBufEven: %d \n",encoderChunk->prevBufferEven[i]);
        }
    }*/
    /*if (test ==1) {
        for (int i = 0; i<25; i++) {
            printf("prevBufOdd: %d \n",encoderChunk->prevBufferOdd[i]);
        }
    }*/
  // Execute first stage convolution
  ConvolutionStage1(encoderChunk->prevBufferEven,encoderChunk->prevBufferOdd,C0,C1, test);
    /*if (test ==1) {
        for (int i = 0; i<10; i++) {
            printf("C0: %lld \n",C0[i]);
        }
    }
    if (test ==1) {
        for (int i = 0; i<10; i++) {
            printf("C1: %lld \n",C1[i]);
        }
    }*/
  // 2nd Stage Analysis using h2 filter coefficients

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
  ConvolutionStage2(encoderChunk->prevBufferC0Even,encoderChunk->prevBufferC0Odd,encoderChunk->prevBufferC1Even,encoderChunk->prevBufferC1Odd,subband1,subband2,subband3,subband4, test);
    
    
}

void ConvolutionStage1(short * inputEven, short * inputOdd, long long * C0, long long * C1, int test) {
   
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

void ConvolutionStage2(short * C0Even, short * C0Odd, short * C1Even, short * C1Odd, short * subband1, short * subband2, short * subband3, short * subband4, int test) {
    
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

/*
encodes all subband signals using ADPCM
the result will be stored at the pointers used for the input
a chunk is needed to contain values of the previous buffer
*/
void ADPCMencoder(short *subband1, short *subband2, short *subband3, short *subband4, struct encoderChunk * encoderChunk) {
	short codebook4[16] = codebook_4;
	//encode subband 1

	ADPCMencoderSubband(subband1, mu_1, 4, &encoderChunk->prediction1, codebook4, 15, &encoderChunk->stepsize1, encoderChunk->deltaPrimeArray1, phi_4);
	//encode subband 2
	ADPCMencoderSubband(subband2, mu_2, 4, &encoderChunk->prediction2, codebook4, 15, &encoderChunk->stepsize2, encoderChunk->deltaPrimeArray2, phi_4);
    short codebook2[4] = codebook_2;
	//encode subband 3
	ADPCMencoderSubband(subband3, mu_3, 2, &encoderChunk->prediction3, codebook2, 3, &encoderChunk->stepsize3, encoderChunk->deltaPrimeArray3, phi_2);
	//encode subband 4
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
	short delta;
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
	//chunk variable is automatically updated because we used a pointer
	//to stepsize and prediction

	//output: encoded values are immediatly stored in the input subbandSignal variable
}

/*
helper function for ADPCMencoderSubband
quantizes the given value and returns the result
*/

short quantize(short value, short* codebook, short codebookSize, short stepsize) { // TESTED WORKS!
	short lowerBound = (codebook[0] + 0.5) * stepsize * 2;
	short upperBound = (codebook[codebookSize] - 1.5) * stepsize * 2;
	short i = 0;
	for (short bound = lowerBound; bound <= upperBound; bound += (2*stepsize)) {
		if (value <= bound) {
			return codebook[i];
		}
        i++;
	}
	return codebook[i];
}

/*
helper function for ADPCMencoderSubband
calculates the new stepsize based on the previous values of delta prime (stored in deltaPrimeArray)
and on phi (cfr. matlab implementation)
*/
short calculateStepsize(short* deltaPrimeArray, short stepsizeOptFP, short nbBits) { // TESTED WORKS!
    short phi;
    phi = stepsizeOptFP + pow(2,8) * 0.3 / nbBits;
    
	//TODO not sure about the std calculation
	//short std = pow(2, 15) * calculateStd(deltaPrimeArray);
    long long std;
    std = calculateStd(deltaPrimeArray)*pow(2,8);
    
	return (phi * std / pow(2, 16));
}

/*
helper function for calculateStepsize
calculate the standard deviation of deltaPrimeArray (size: nbDelta)
does not include a change to floating point
*/
float calculateStd(short* deltaPrimeArray) {   // TESTED WORKS!
	//TODO ask if long long can be used for this
	long long mean = 0;
	for (int i = 0; i < nbDelta; i++) {
		mean += deltaPrimeArray[i];
	}
	mean = mean / nbDelta;

	float std = 0;
	for (int i = 0; i < nbDelta; i++) {
		std += pow(llabs(deltaPrimeArray[i] - mean), 2);
	}
	std = std / (nbDelta-1);
	std = sqrt(std);

	return std;
}


/*
helper function for calculateStepsize
calculate the standard deviation of deltaPrimeArray (size: nbDelta)
already includes a change to floating point
*/
short calculateStdFP(short* deltaPrimeArray) {
	//TODO ask if long long can be used for this
	long long mean = 0;
	for (int i = 0; i < nbDelta; i++) {
		mean = mean + deltaPrimeArray[i];
	}
	mean = mean / nbDelta;

	long long std = 0;
	for (int i = 0; i < nbDelta; i++) {
		long long diff = pow(2, 15)*llabs(deltaPrimeArray[i] - mean);
		std = std + pow(diff, 2);
	}
	std = std / nbDelta;
	std = sqrt(std);

	return std;
}


/* encodes one subband signal 
inputs: subband signal to encode, nb of quantisation bits to use
*/
/*void ADPCMencoder(short *subband1, short *subband2, short *subband3, short *subband4, struct encoderChunkLeft * encoderChunkLeft)
{
	short initPrediction;
	//TODO : define a function that initializes the encoderChunkLeft (initial stepsize, previous sample ...)
	initialize(encoderChunkLeft);

	// for each subband, call the ADPCMencoderSubband function
	//subband1
	initPrediction = mu_1 * subband1[0];
	ADPCMencoderSubband(subband1, initPrediction, 4, codebook_4, encoderChunkLeft) ;
	initialize(encoderChunkLeft);
	
	//subband2
	initPrediction = mu_2 * subband2[0];
	ADPCMencoderSubband(subband2, initPrediction, 4, codebook_4, encoderChunkLeft);
	initialize(encoderChunkLeft);
	
	//subband3
	initPrediction = mu_3 * subband3[0];
	ADPCMencoderSubband(subband3, initPrediction, 2, codebook_2, encoderChunkLeft);
	initialize(encoderChunkLeft);
	
        //subband4
	initPrediction = mu_4 * subband4[0];
	ADPCMencoderSubband(subband4, initPrediction, 2, codebook_2, encoderChunkLeft);
	
}

 encodes only one subband
input: pointer to the start of an array containing the subband signal
       the first prediction value, will be updated
       the first stepsize value, will be updated
       the number of quantization bits to use
       /Oula
       choice of codebook depends on the subbands, input of the encoder
       inputs should be revisited, can't have so man inputs if the chunk variable carries the necessary coefficients

void ADPCMencoderSubband(short *subband, short prediction, short nbBits, short *codebook,  struct encoderChunkLeft * encoderChunkLeft)//short * encoded_subband)
{
	short delta;
	short delta_approx;
	short stepsize;
	short quantized_delta;
	short min_edge;
	short max_edge;
	int n_partition;
	short partition[n_partition];
	// encode all samples
	for (int i = 0; i < BUFFERSIZE / 2; i++) {
	  //stepsize = encoderChunkLeft->current_stepsize;
	  delta = (subband[i] - prediction)*2;

	  //defining partition
	  min_edge = stepsize*(1-2^(nbBits));
	  max_edge = stepsize*(-3+2^(nbBits));
	  n_partition = (max_edge-min_edge)/(2*stepsize)+1;
	  for(int j =0; j < n_partition; j++){
		  partition[j] = min_edge + j * stepsize * 2;
	  }
	  quantize(delta, partition, codebook, quantized_delta);
		
	  //calculate delta prime
	  delta_approx = stepsize * quantized_delta[0]; //only one same is present in delta quantized as delta is of size 1
	  
		//TODO update the stepsize
		//TODO update the prediction
	}
}

function that quantizes a sample given a codebook and a partition
void quantize(short * input, short * codebook, short * partition, short * quantized){
  int n = sizeof(input)/sizeof(short);
  int m = sizeof(partition)/sizeof(short);
  int index;
  for(int j=0;j<n;j++){
    if (input[j] <= partition[0])
      index = 0;
    int k = 1;
    while(k<m){
       if (input[j] > partition[k-1] && input[j] <= partition[k])
	  index = k;
       k++;
    }
    if (input[j] > partition[m-1])
      index = m;
    
    quantized[j] = codebook[index];
  }
}*/

