#include "globals.h"
#include "fadeOut.h"

const short h0_odd[16] = {320, -192, -1, 289, -749, 1546, -3201, 8982, 30212, -6476, 3670, -2465, 1735, -1120, 819, -461};
const short h0_even[16] = {-461,	819,	 -1220,	1735, -2465, 3670, -6476, 30212,	8982, -3201,	1546, -749, 289,	 -1, -192, 320};
const short h2_odd[8] = {650, 255, -1872, 7869, 30860, -6543, 3170, -1624};
const short h2_even[8] = {-1624,	3170	, -6543, 30860, 7869, -1872,	255, 650};



void analysis(short *buffer, short *subband1, short *subband2, short *subband3, short *subband4);
void ADPCMencoder();


/* encode */
void transmitter(short *buffer, unsigned short *encodedBuffer, struct Tx_chunk *chunk)
{
	//split in left and right signal
	short leftSignal[BUFFERSIZE/2];
	short rightSignal[BUFFERSIZE/2];
	for (int bufPos = 0; bufPos < BUFFERSIZE; bufPos += 2) {
		//TODO check if interleaved buffer starts with left sample
		leftSignal[bufPos / 2] = buffer[bufPos];
		rightSignal[bufPos / 2] = buffer[bufPos + 1];
	}
	
	/*LEFT*/
	//analysis left
	analysis(leftSignal);
	//TODO ADPCM left
	ADPCMencoder();

	/*RIGHT*/
	//analysis right
	analysis(rightSignal);
	//TODO ADPCM right
	ADPCMencoder();

	//TODO bit shifting into encodedBuffer
}

/* creates 4 subband signals */
// recurrent function 
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
void ADPCMencoder(short *subband1, short *subband2, short *subband3, short *subband4)
{
	short initPrediction;
	short initStepsize;
	short mu;
	short prevInput;
	// per subband extract from chunk variable: 
	// last sample, last stepsize from previous buffer

	// for each subband, call the ADPCMencoderSubband function
	initPrediction = mu * prevInput;
	ADPCMencoderSubband(subband1, initPrediction, initStepsize, 4);
}

/* encodes only one subband
input: pointer to the start of an array containing the subband signal
       the first prediction value, will be updated
	   the first stepsize value, will be updated
	   the number of quantisation bits to use
*/
void ADPCMencoderSubband(short *subband, short prediction, short stepsize, short nbBits)
{
	short delta;
	// encode all samples
	for (int i = 0; i < BUFFERSIZE / 2; i++) {
		delta = (subband[i] - prediction)*2;
		//TODO quantize delta
		//TODO calculate delta prime
		//TODO update the stepsize
		//TODO update the prediction
	}
}

