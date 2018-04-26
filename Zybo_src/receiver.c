#include "globals.h"
#include "receiver.h"
#include "transmitter.h"
#include "math.h"
#include <stdio.h>

const short f2_even[8] = {-812,    1585,    -3271,    15430,    3935,    -936,    127,    325};
const short f2_odd[8] = {325,    127,    -936,    3935,    15430,    -3271,    1585,    -812};
const short f0_even[16] = {-231,    409,    -610,    868,    -1232,    1835,    -3238,    15106,    4491,    -1601,    773,    -375,    145,    0,    -96,    160};
const short f0_odd[16] = {160,    -96,    0,    145,    -375,    773,    -1601,    4491,    15106,    -3238,    1835,    -1232,    868,    -610,    409,    -231};

/* Receiver
 Function That performs the full receiver function: Decoding followed by synthesis for both the left and the right singal
 Inputs:    - encodedBuffer: Array where the the encoded subband signals are stored in (bit unpacking has to be performed onto)
            - decoderChunkLeft: Chunk variable used for storing necessary previous buffer results for the left signal
            - decoderChunkRight: Chunk variable used for storing necessary previous buffer results for the right signal
 Ouputs:    - buffer: the result is stored in this array
 */
void receiver(unsigned short* encodedBuffer, short *buffer, struct decoderChunk * decoderChunkLeft, struct decoderChunk * decoderChunkRight)
{
    short subband1_l[5];
    short subband1_r[5];
    short subband2_l[5];
    short subband2_r[5];
    short subband3_l[5];
    short subband3_r[5];
    short subband4_l[5];
    short subband4_r[5];
	short bufPos;
    
	/* Bit unpacking */
	int i;
	// retrieve the 4-bit values from the encoded buffer, stored in the first 5 shorts as
	// subband_left1 (4 bits) - subband_left2 (4 bits) - subband_right1 (4 bits) - subband_right2 (4 bits)
	for (i = 0; i < (BUFFERSIZE/8); i++) {
		subband1_l[i] = twosComplement4Bits((encodedBuffer[i] & 0xF000) >> 12);
		subband2_l[i] = twosComplement4Bits((encodedBuffer[i] & 0x0F00) >> 8);
		subband1_r[i] = twosComplement4Bits((encodedBuffer[i] & 0x00F0) >> 4);
		subband2_r[i] = twosComplement4Bits((encodedBuffer[i] & 0x000F));
	}

	// retrieve the 2-bit values from the encoded buffer, stored in the last 3 shorts
	for (i = 0; i < (BUFFERSIZE/8); i++) {
		//if i is even 
		if ( !(i % 2) ) {
			bufPos = 5 + i / 2;
			subband3_l[i] = twosComplement2Bits((encodedBuffer[bufPos] & 0xC000) >> 14);
			subband4_l[i] = twosComplement2Bits((encodedBuffer[bufPos] & 0x3000) >> 12);
			subband3_r[i] = twosComplement2Bits((encodedBuffer[bufPos] & 0x0C00) >> 10);
			subband4_r[i] = twosComplement2Bits((encodedBuffer[bufPos] & 0x0300) >> 8);
		}
		//if i is uneven
		else {
			bufPos = 5 + (i - 1) / 2;
			subband3_l[i] = twosComplement2Bits((encodedBuffer[bufPos] & 0x00C0) >> 6);
			subband4_l[i] = twosComplement2Bits((encodedBuffer[bufPos] & 0x0030) >> 4);
			subband3_r[i] = twosComplement2Bits((encodedBuffer[bufPos] & 0x000C) >> 2);
			subband4_r[i] = twosComplement2Bits((encodedBuffer[bufPos] & 0x0003));
		}
	}
   
    /* Decode LEFT */
    ADPCMdecoder(subband1_l,subband2_l,subband3_l,subband4_l,decoderChunkLeft);
    /* Decode RIGHT */
    ADPCMdecoder(subband1_r,subband2_r,subband3_r,subband4_r,decoderChunkRight);
	
    short resultLeft[BUFFERSIZE/2];
    short resultRight[BUFFERSIZE/2];
    
	/* Synthesis LEFT */
    synthesis(subband1_l,subband2_l,subband3_l,subband4_l,decoderChunkLeft,resultLeft);
    /* Synthesis RIGHT */
    synthesis(subband1_r,subband2_r,subband3_r,subband4_r,decoderChunkRight,resultRight);
    
    // Interleave LEFT and RIGHT
    for (i = 0; i < BUFFERSIZE; i+=2) {
        buffer[i] = resultLeft[i/2];
        buffer[i+1] = resultRight[i/2];
    }
}

/* twosComplement2Bits
 Function that gets the 2s complement representation right
 Input:    - value to get to the 2s complement from
*/
short twosComplement2Bits(unsigned short value) {
	//for a negative value, add 1's and keep the last 2 bits
	if (value & 0x2) {
		return (value | 0xFFFC);
	}
	//for a positive value, do nothing
	return value;
}

/* twosComplement4Bits
 Function that gets the 2s complement representation right
 Input:    - value to get to the 2s complement from
*/
short twosComplement4Bits(unsigned short value) {
	//for a negative value, add 1's and keep the last 4 bits
	if (value & 0x8) {
		return (value | 0xFFF0);
	}
	//for a positive value, do nothing
	return value;
}

/* SYNTHESIS
 Function that performs the synthesis from 4 subband signals to one signal
 Inputs:    - the 4 different subband signals
            - the decoderChunk used to remember values from previous buffers
 Outputs:   - result is the output of the filterbank
 */
void synthesis(short * subband1, short * subband2, short * subband3, short * subband4, struct decoderChunk * decoderChunk, short * result){

    /* Synthesis of the first subband signal with the second and the third with the fourth */
    
	short s20[5] = {0};
	short s21[5] = {0};
	short s22[5] = {0};
	short s23[5] = {0};

	int i;

    // Intermediate result from new buffer
	for (i = 0; i < 5; i++) {
		s20[i] = subband1[i] + subband2[i];
		s21[i] = subband1[i] - subband2[i];
		s22[i] = subband3[i] + subband4[i];
		s23[i] = subband3[i] - subband4[i];
       
	}
    
	// Update chunk, put the old values in front and the newest in the back
	for (i = 0; i<12; i++) {
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
	ConvolutionStage1dec(decoderChunk->prevBufferS20,decoderChunk->prevBufferS21,decoderChunk->prevBufferS22,decoderChunk->prevBufferS23,f20,f21,f22,f23);

    /* Synthesis of the results from stage 1 */
    
	short result1[10] = {0};
	short result2[10] = {0};

    // Interleave results from stage 1
	for (i = 0; i<10 ; i+=2) {
		result1[i] = f21[i/2];
		result1[i+1] = f20[i/2];
		result2[i] = f23[i/2];
		result2[i+1] = f22[i/2];
	}
    
	short s10[10] = {0};
	short s11[10] = {0};

    // Intermediate result as the input for the convolution
	for (i = 0; i < 10; i++) { // WAUW 5 ipv 10
		s10[i] = result1[i] + result2[i];
		s11[i] = result1[i] - result2[i];
	}
	

	// Update the chunk
	for (i = 0; i<25; i++) {
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

    // Execute convolution stage 2
	ConvolutionStage2dec(decoderChunk->prevBufferS10,decoderChunk->prevBufferS11,f10,f11);

	for (i = 0; i<20; i+=2) {
		result[i] = f11[i/2];
		result[i+1] = f10[i/2];
	}
}

/* Convolution Stage 1 decoder
 Function that performs the convolution in the first stage of synthesis
 Inputs:    - s2x: intermediate result with which the convolution has to be performed
 Outputs:   - f2x: The results from the convolution are stored in these variables
 */
void ConvolutionStage1dec(short * s20,short * s21, short * s22, short * s23, long long * f20, long long * f21, long long * f22, long long * f23) {

	int i,j;

	for (i = 7; i<12; i++) {
		  for (j=0; j<8;j++) {
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

/* Convolution Stage 2 decoder
 Function that performs the convolution in the second stage of synthesis
 Inputs:    - s1x: intermediate result with which the convolution has to be performed
 Outputs:   - f1x: The results from the convolution are stored in these variables
 */
void ConvolutionStage2dec(short * s10, short * s11, long long * f10, long long * f11) {

	int i,j;

	for (i = 15; i<25; i++) {
		  for (j=0; j<16;j++) {
			  f11[i-15] += s11[i-j]*f0_even[j];
			  f10[i-15] += s10[i-j]*f0_odd[j];
		  }
          f11[i-15] = f11[i-15]/pow(2,14);
          f10[i-15] = f10[i-15]/pow(2,14);
         
	  }
}

/* ADPCMdecoder
 Function that calls the individual subband decoder sequentially for the 4 different subbands
 Inputs:    - the 4 subbands that have to be decoded
            - the chunk for the values of previous buffers
 Outputs:   - the 4 decoded subbands are stored in the subband inputs
 */
void ADPCMdecoder(short * subband1, short * subband2, short * subband3, short * subband4, struct decoderChunk * decoderChunk) {
    
    /* Decode Subband1 */
    ADPCMdecoderSubband(subband1,mu_1,4,&decoderChunk->stepsize1,decoderChunk->deltaPrimeArray1,phi_4,&decoderChunk->prevoutput1);
    
    /* Decode Subband2 */
    ADPCMdecoderSubband(subband2,mu_2,4,&decoderChunk->stepsize2,decoderChunk->deltaPrimeArray2,phi_4,&decoderChunk->prevoutput2);
    
    /* Decode Subband3 */
    ADPCMdecoderSubband(subband3,mu_3,2,&decoderChunk->stepsize3,decoderChunk->deltaPrimeArray3,phi_2,&decoderChunk->prevoutput3);
    
    /* Decode Subband4 */
    ADPCMdecoderSubband(subband4,mu_4,2,&decoderChunk->stepsize4,decoderChunk->deltaPrimeArray4,phi_2,&decoderChunk->prevoutput4);
    
}

/* ADPCMdecoderSubband
 Function that decodes one particular subband
 Inputs:    - the subband singal to be decoded
            - the mu used in the first order prediction for the specific subband
            - the number of quantization bits used for this subband
            - the pointer to the previous stepsize
            - the delta prime array in the chunk variable used for the stepsize calculation
            - the Phi used in this subband
            - the previous output needed for decoding
 Outputs:   - the decoded subband is saved in the original subband input
 */
void ADPCMdecoderSubband(short * subbandSignal, short mu, short n0_bits, short * stepsize, short * deltaPrimeArray, short PHI, short * prevoutput) {
    
    short delta_prime;
    int i,j;

    for (i = 0; i<5; i++) {
        
        // Shift delta prime array to get the 10 most recent samples for the stepsize calculation
        for (j = 0; j < 9; j++) {
            deltaPrimeArray[j] = deltaPrimeArray[j+1];
        }
        
        // Calculate new delta prime
        delta_prime = subbandSignal[i] * *stepsize;
        deltaPrimeArray[9] = delta_prime;
        
        // Update stepsize
        *stepsize = calculateStepsize(deltaPrimeArray,PHI,n0_bits);
        if (*stepsize == 0) {
            *stepsize = 1;
        }
        
        // Calculate output
        subbandSignal[i] = delta_prime + (mu * *prevoutput)/pow(2,15);
        
        // Update prevoutput
        *prevoutput = subbandSignal[i];
        
    }
}
