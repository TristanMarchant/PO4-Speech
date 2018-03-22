#include "globals.h"
#include "fadeOut.h"

const short h0_odd[16] = {320, -192, -1, 289, -749, 1546, -3201, 8982, 30212, -6476, 3670, -2465, 1735, -1120, 819, -461};
const short h0_even[16] = {-461, 819, -1220, 1735, -2465, 3670, -6476, 30212, 8982, -3201, 1546, -749, 289, -1, -192, 320};
const short h2_odd[8] = {650, 255, -1872, 7869, 30860, -6543, 3170, -1624};
const short h2_even[8] = {-1624,3170, -6543, 30860, 7869, -1872,255, 650};

/* transform the input signal in buffer to an encoded signal, stored in encodedBuffer */
void transmitter(short *buffer, unsigned short *encodedBuffer)
{
	short leftSignal[BUFFERSIZE/2];
	short rightSignal[BUFFERSIZE/2];
	short subband_l1[BUFFERSIZE / 2];
	short subband_l2[BUFFERSIZE / 2];
	short subband_l3[BUFFERSIZE / 2];
	short subband_l4[BUFFERSIZE / 2];
	short subband_r1[BUFFERSIZE / 2];
	short subband_r2[BUFFERSIZE / 2];
	short subband_r3[BUFFERSIZE / 2];
	short subband_r4[BUFFERSIZE / 2];

	//split in left and right signal
	for (int bufPos = 0; bufPos < BUFFERSIZE; bufPos += 2) {
		//TODO check if interleaved buffer starts with left sample
		leftSignal[bufPos / 2] = buffer[bufPos];
		rightSignal[bufPos / 2] = buffer[bufPos + 1];
	}
	
	/*LEFT*/
	//analysis left
	analysis(leftSignal, subband_l1, subband_l2, subband_l3, subband_l4);
	//ADPCM left
	ADPCMencoder(subband_l1, subband_l2, subband_l3, subband_l4);

	/*RIGHT*/
	//analysis right
	analysis(rightSignal, subband_r1, subband_r2, subband_r3, subband_r4);
	//ADPCM right
	ADPCMencoder(subband_r1, subband_r2, subband_r3, subband_r4);

	//TODO bit shifting into encodedBuffer
}

/* creates 4 subband signals */
void analysis(short *buffer, short *subband1, short *subband2, short *subband3, short *subband4)
{
  //1.Analysis for L/R signal using h0 filter coeffcients
  
  int nconv_1 = BUFFERSIZE/4 + len(h0_odd) -1; //len(h0_odd) = 16
  
  short LR_even[BUFFERSIZE/4];
  short LR_odd[BUFFERSIZE/4];
  short C0[nconv_1]; //int or short ?
  short C1[nconv_1];
  
  for (int bufPos = 0; bufPos < BUFFERSIZE/2; bufPos += 2) {
    LR_even[bufPos / 2] = buffer[bufPos];
    LR_odd[bufPos / 2] = buffer[bufPos + 1];
  }
  
  short X0[nconv_1];
  short Y0[nconv_1];
  for (int i = 0; i<nconv_1; i++)
    {
      X0[i] = 0;
      Y0[i] = 0;
      for (int j=0; j<16;j++)
	{
	  X0 [i] += LR_even[i-j]*h0_even[j];
	  Y0 [i] += LR_odd[i-j]*h0_odd[j];
         }
      C0[i] = X0[i]+Y0[i];
      C1[i] = X0[i]-Y0[i];
    }

//2.Analysis for C0 and C1 signal using h2 filter coefficients
  //used variables for analyzing C0 and C1
  int nconv_2 = nconv_1/2 + len(h2_odd) -1; //len(h2_odd) = 8
  
  short C0_even[nconv_1/2];
  short C0_odd[nconv_1/2];
  short C1_even[nconv_1/2];
  short C1_odd[nconv_1/2];
  
  for (int bufPos = 0; bufPos < nconv_1; bufPos += 2) {
    C0_even[bufPos / 2] = C0[bufPos];
    C1_even[bufPos / 2] = C1[bufPos];
    C0_odd[bufPos / 2] = C0[bufPos + 1];
    C1_odd[bufPos / 2] = C1[bufPos + 1];
  }
  //intermediate buffers
  short XC0[nconv_2]; 
  short YC0[nconv_2]; 
  short XC1[nconv_2]; 
  short YC1[nconv_2]; 

  for (int i = 0; i<nconv_2; i++)
    {
      X1[i] = 0;
      Y2[i] = 0;
      for (int j=0; j<8;j++)
	{
	  if (i >= j)
	    {
	    XC0 [i] += C0_even[i-j]*h2_even[j];
	    YC0 [i] += C0_odd[i-j]*h2_odd[j];
	    XC1 [i] += C1_even[i-j]*h2_even[j];
	    YC1 [i] += C1_odd[i-j]*h2_odd[j];
	    }
	}
      subband1[i] = XC0[i]+YC0[i];
      subband2[i] = XC0[i]-YC0[i];
      subband3[i] = XC1[i]+YC1[i];
      subband4[i] = XC1[i]-YC1[i];
    }

}

/* encodes one subband signal 
inputs: subband signal to encode, nb of quantisation bits to use
*/
void ADPCMencoder(short *subband1, short *subband2, short *subband3, short *subband4, struct chunk * encoderChunk)
{
	short initPrediction;
	//TODO : define a function that initializes the encoderChunk (initial stepsize, previous sample ...)
	initialize(encoderChunk);

	// for each subband, call the ADPCMencoderSubband function
	//subband1
	initPrediction = mu_1 * subband1[0];
	ADPCMencoderSubband(subband1, initPrediction, 4, codebook_4, encoderChunk) ;
	initialize(encoderChunk);
	
	//subband2
	initPrediction = mu_2 * subband2[0];
	ADPCMencoderSubband(subband2, initPrediction, 4, codebook_4, encoderChunk);
	initialize(encoderChunk);
	
	//subband3
	initPrediction = mu_3 * subband3[0];
	ADPCMencoderSubband(subband3, initPrediction, 2, codebook_2, encoderChunk);
	initialize(encoderChunk);
	
        //subband4
	initPrediction = mu_4 * subband4[0];
	ADPCMencoderSubband(subband4, initPrediction, 2, codebook_2, encoderChunk);
	
}

/* encodes only one subband
input: pointer to the start of an array containing the subband signal
       the first prediction value, will be updated
       the first stepsize value, will be updated
       the number of quantization bits to use
       /Oula
       choice of codebook depends on the subbands, input of the encoder
       inputs should be revisited, can't have so man inputs if the chunk variable carries the necessary coefficients
*/
void ADPCMencoderSubband(short *subband, short prediction, short nbBits, short * codebook,  struct chunk * encoderChunk)//short * encoded_subband)
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
	for (int i = 1; i < BUFFERSIZE / 2; i++) {
	  stepsize = encoderChunk->current_stepsize;
	  delta = (subband[i] - prediction)*2;

	  //defining partition
	  min_edge = stepsize*(1-2**(nbBits));
	  max_edge = stepsize*(-3+2**(nbBits));
	  n_partition = (max_edge-min_edge)/(2*stepsize)+1;
	  for(int j =0; j < n_partition; j++){
	    partition[j]= min_edge + j*stepsize*2
	  }
	  quantize(delta, partition, codebook, quantized_delta)
		
	  //calculate delta prime
	    delta_approx = stepsize*quantized_delta[0] //only one same is present in delta quantized as delta is of size 1
	  
		//TODO update the stepsize
		//TODO update the prediction
	}
}

/*function that quantizes a sample given a codebook and a partition*/
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
}
