#include "globals.h"
#include "transmitter.h"
#include "math.h"
#include "string.h"

const short h0_odd[16] = {320, -192, -1, 289, -749, 1546, -3201, 8982, 30212, -6476, 3670, -2465, 1735, -1120, 819, -461};
const short h0_even[16] = {-461, 819, -1220, 1735, -2465, 3670, -6476, 30212, 8982, -3201, 1546, -749, 289, -1, -192, 320};
const short h2_odd[8] = {650, 255, -1872, 7869, 30860, -6543, 3170, -1624};
const short h2_even[8] = {-1624,3170, -6543, 30860, 7869, -1872,255, 650};

/* transform the input signal in buffer to an encoded signal, stored in encodedBuffer */
void transmitter(short *buffer, struct encoderChunk *encoderChunkLeft, struct encoderChunk *encoderChunkRight, unsigned short encodedBuffer)
{
	short leftSignal[BUFFERSIZE/2];
	short rightSignal[BUFFERSIZE/2];
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
		//TODO check if interleaved buffer starts with left sample
		leftSignal[bufPos / 2] = buffer[bufPos];
		rightSignal[bufPos / 2] = buffer[bufPos + 1];
	}
	
	/*LEFT*/
	//analysis left
	//analysis(leftSignal, subband_l1, subband_l2, subband_l3, subband_l4, &encoderChunkLeft);
	analysis(leftSignal, buffer, subband_l2, subband_l3, subband_l4, &encoderChunkLeft);
	//ADPCM left
	//ADPCMencoder(subband_l1, subband_l2, subband_l3, subband_l4, &encoderChunkLeft);

	/*RIGHT*/
	//analysis right
	analysis(rightSignal, subband_r1, subband_r2, subband_r3, subband_r4, &encoderChunkRight);
	//ADPCM right
	//ADPCMencoder(subband_r1, subband_r2, subband_r3, subband_r4, &encoderChunkLeft);

	//TODO bit 'packing'
}

/* creates 4 subband signals */
void analysis(short *buffer, short *subband1, short *subband2, short *subband3, short *subband4, struct encoderChunk *encoderChunk)
{
  // 1st Stage Analysis using h0 filter coefficients
  
  short Even[BUFFERSIZE/4];
  short Odd[BUFFERSIZE/4];
  long long C0[10];
  long long C1[10];
  
  // Split even and odd
  for (int bufPos = 0; bufPos < (BUFFERSIZE/2); bufPos += 2) {
    Even[bufPos / 2] = buffer[bufPos];
    Odd[bufPos / 2] = buffer[bufPos + 1];
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
  ConvolutionStage1(encoderChunk->prevBufferEven,encoderChunk->prevBufferOdd,h0_even,h0_odd,C0,C1);

  // 2nd Stage Analysis using h2 filter coefficients

  short C0_even[5];
  short C0_odd[5];
  short C1_even[5];
  short C1_odd[5];
  
  // Split even and odd
  for (int bufPos = 0; bufPos < 10; bufPos += 2) {
    C0_even[bufPos / 2] = C0[bufPos];
    C1_even[bufPos / 2] = C1[bufPos];
    C0_odd[bufPos / 2] = C0[bufPos + 1];
    C1_odd[bufPos / 2] = C1[bufPos + 1];
  }

  short XC0[5];
  short YC0[5];
  short XC1[5];
  short YC1[5];


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
  		  encoderChunk->prevBufferC0Even[i] = C1_even[i-7];
		  encoderChunk->prevBufferC0Odd[i] = C1_odd[i-7];
  	  }
    }


  // Convolution stage 2
  int k = 0;
  for (int i = 7; i<12; i++)
    {
      XC0[i] = 0;
      XC1[i] = 0;
	  YC0[i] = 0;
	  YC1[i] = 0;
      for (int j=0; j<8;j++)
	{
	    XC0 [k] += encoderChunk->prevBufferC0Even[i-j]*h2_even[j];
	    YC0 [k] += encoderChunk->prevBufferC0Odd[i-j]*h2_odd[j];
	    XC1 [k] += encoderChunk->prevBufferC1Even[i-j]*h2_even[j];
	    YC1 [k] += encoderChunk->prevBufferC1Odd[i-j]*h2_odd[j];
	    }

      subband1[k] = (XC0[k]+YC0[k]) / (pow(2, 16));
      subband2[k] = (XC0[k]-YC0[k]) / (pow(2, 16));
      subband3[k] = (XC1[k]+YC1[k]) / (pow(2, 16));
      subband4[k] = (XC1[k]-YC1[k]) / (pow(2, 16));
      k += 1;
    }

}

void ConvolutionStage1(short * inputEven, short * inputOdd, short * filterEven, short * filterOdd, long long * C0, long long * C1) {
	  int k = 0;
	  long long X0[10] = {0};
	  long long Y0[10] = {0};

	  for (int i = 15; i<25; i++)

	    {
		  X0[i-15] = 0;
		  Y0[i-15] = 0;
	      for (int j=0; j<16;j++)
			{
			  X0 [k] += inputEven[i-j]*filterEven[j];
			  Y0 [k] += inputOdd[i-j]*filterOdd[j];
				 }
	      k += 1;

	    }
	   for (int j = 0; j<10; j++) {
		   C0[j] = (X0[j]+Y0[j])/pow(2,16);
		   C1[j] = (X0[j]-Y0[j])/pow(2,16);
	   }
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

