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
    
	// retrieve the 4-bit values from the encoded buffer, stored in the first 5 shorts as
	// subband_left1 (4 bits) - subband_left2 (4 bits) - subband_right1 (4 bits) - subband_right2 (4 bits)
	//for (int i = 0; i < (BUFFERSIZE_8); i++) {
	//	subband1_l[i] = twosComplement4Bits((encodedBuffer[i] & 0xF000) >> 12);
	//	subband2_l[i] = twosComplement4Bits((encodedBuffer[i] & 0x0F00) >> 8);
	//	subband1_r[i] = twosComplement4Bits((encodedBuffer[i] & 0x00F0) >> 4);
	//	subband2_r[i] = twosComplement4Bits((encodedBuffer[i] & 0x000F));
	//}
	//loop unrolling
	subband1_l[0] = twosComplement4Bits((encodedBuffer[0] & 0xF000) >> 12);
	subband2_l[0] = twosComplement4Bits((encodedBuffer[0] & 0x0F00) >> 8);
	subband1_r[0] = twosComplement4Bits((encodedBuffer[0] & 0x00F0) >> 4);
	subband2_r[0] = twosComplement4Bits((encodedBuffer[0] & 0x000F));

	subband1_l[1] = twosComplement4Bits((encodedBuffer[1] & 0xF000) >> 12);
	subband2_l[1] = twosComplement4Bits((encodedBuffer[1] & 0x0F00) >> 8);
	subband1_r[1] = twosComplement4Bits((encodedBuffer[1] & 0x00F0) >> 4);
	subband2_r[1] = twosComplement4Bits((encodedBuffer[1] & 0x000F));

	subband1_l[2] = twosComplement4Bits((encodedBuffer[2] & 0xF000) >> 12);
	subband2_l[2] = twosComplement4Bits((encodedBuffer[2] & 0x0F00) >> 8);
	subband1_r[2] = twosComplement4Bits((encodedBuffer[2] & 0x00F0) >> 4);
	subband2_r[2] = twosComplement4Bits((encodedBuffer[2] & 0x000F));

	subband1_l[3] = twosComplement4Bits((encodedBuffer[3] & 0xF000) >> 12);
	subband2_l[3] = twosComplement4Bits((encodedBuffer[3] & 0x0F00) >> 8);
	subband1_r[3] = twosComplement4Bits((encodedBuffer[3] & 0x00F0) >> 4);
	subband2_r[3] = twosComplement4Bits((encodedBuffer[3] & 0x000F));

	subband1_l[4] = twosComplement4Bits((encodedBuffer[4] & 0xF000) >> 12);
	subband2_l[4] = twosComplement4Bits((encodedBuffer[4] & 0x0F00) >> 8);
	subband1_r[4] = twosComplement4Bits((encodedBuffer[4] & 0x00F0) >> 4);
	subband2_r[4] = twosComplement4Bits((encodedBuffer[4] & 0x000F));


	// retrieve the 2-bit values from the encoded buffer, stored in the last 3 shorts
	//for (int i = 0; i < (BUFFERSIZE_8); i++) {
	//	//if i is even 
	//	if ( !(i % 2) ) {
	//		bufPos = 5 + i / 2;
	//		subband3_l[i] = twosComplement2Bits((encodedBuffer[bufPos] & 0xC000) >> 14);
	//		subband4_l[i] = twosComplement2Bits((encodedBuffer[bufPos] & 0x3000) >> 12);
	//		subband3_r[i] = twosComplement2Bits((encodedBuffer[bufPos] & 0x0C00) >> 10);
	//		subband4_r[i] = twosComplement2Bits((encodedBuffer[bufPos] & 0x0300) >> 8);
	//	}
	//	//if i is uneven
	//	else {
	//		bufPos = 5 + (i - 1) / 2;
	//		subband3_l[i] = twosComplement2Bits((encodedBuffer[bufPos] & 0x00C0) >> 6);
	//		subband4_l[i] = twosComplement2Bits((encodedBuffer[bufPos] & 0x0030) >> 4);
	//		subband3_r[i] = twosComplement2Bits((encodedBuffer[bufPos] & 0x000C) >> 2);
	//		subband4_r[i] = twosComplement2Bits((encodedBuffer[bufPos] & 0x0003));
	//	}
	//}
	//loop unrolling
	subband3_l[0] = twosComplement2Bits((encodedBuffer[5] & 0xC000) >> 14);
	subband4_l[0] = twosComplement2Bits((encodedBuffer[5] & 0x3000) >> 12);
	subband3_r[0] = twosComplement2Bits((encodedBuffer[5] & 0x0C00) >> 10);
	subband4_r[0] = twosComplement2Bits((encodedBuffer[5] & 0x0300) >> 8);

	subband3_l[1] = twosComplement2Bits((encodedBuffer[5] & 0x00C0) >> 6);
	subband4_l[1] = twosComplement2Bits((encodedBuffer[5] & 0x0030) >> 4);
	subband3_r[1] = twosComplement2Bits((encodedBuffer[5] & 0x000C) >> 2);
	subband4_r[1] = twosComplement2Bits((encodedBuffer[5] & 0x0003));

	subband3_l[2] = twosComplement2Bits((encodedBuffer[6] & 0xC000) >> 14);
	subband4_l[2] = twosComplement2Bits((encodedBuffer[6] & 0x3000) >> 12);
	subband3_r[2] = twosComplement2Bits((encodedBuffer[6] & 0x0C00) >> 10);
	subband4_r[2] = twosComplement2Bits((encodedBuffer[6] & 0x0300) >> 8);

	subband3_l[3] = twosComplement2Bits((encodedBuffer[6] & 0x00C0) >> 6);
	subband4_l[3] = twosComplement2Bits((encodedBuffer[6] & 0x0030) >> 4);
	subband3_r[3] = twosComplement2Bits((encodedBuffer[6] & 0x000C) >> 2);
	subband4_r[3] = twosComplement2Bits((encodedBuffer[6] & 0x0003));

	subband3_l[4] = twosComplement2Bits((encodedBuffer[7] & 0xC000) >> 14);
	subband4_l[4] = twosComplement2Bits((encodedBuffer[7] & 0x3000) >> 12);
	subband3_r[4] = twosComplement2Bits((encodedBuffer[7] & 0x0C00) >> 10);
	subband4_r[4] = twosComplement2Bits((encodedBuffer[7] & 0x0300) >> 8);

    /* Decode LEFT */
    ADPCMdecoder(subband1_l,subband2_l,subband3_l,subband4_l,decoderChunkLeft);
    /* Decode RIGHT */
    ADPCMdecoder(subband1_r,subband2_r,subband3_r,subband4_r,decoderChunkRight);
	
    short resultLeft[BUFFERSIZE_2];
    short resultRight[BUFFERSIZE_2];
    
	/* Synthesis LEFT */
    synthesis(subband1_l,subband2_l,subband3_l,subband4_l,decoderChunkLeft,resultLeft);
    /* Synthesis RIGHT */
    synthesis(subband1_r,subband2_r,subband3_r,subband4_r,decoderChunkRight,resultRight);
    
    // Interleave LEFT and RIGHT
    for (int i = 0; i < BUFFERSIZE; i+=2) { //TODO loop unrolling or loop termination
        buffer[i] = resultLeft[i>>1];
        buffer[i+1] = resultRight[i>>1];
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

    // Intermediate result from new buffer
	for (int i = 0; i < 5; i++) {  //TODO loop unrolling or loop termination
		s20[i] = subband1[i] + subband2[i];
		s21[i] = subband1[i] - subband2[i];
		s22[i] = subband3[i] + subband4[i];
		s23[i] = subband3[i] - subband4[i];
       
	}
    
	// Update chunk, put the old values in front and the newest in the back
	for (int i = 0; i<12; i++) { //TODO loop unrolling or loop termination
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
	//for (int i = 0; i<10 ; i+=2) {
	//	result1[i] = f21[i/2];
	//	result1[i+1] = f20[i/2];
	//	result2[i] = f23[i/2];
	//	result2[i+1] = f22[i/2];
	//}
	//loop unrolling
	result1[0] = f21[0]; result1[1] = f20[0];
	result2[0] = f23[0]; result2[1] = f22[0];
	result1[2] = f21[1]; result1[3] = f20[1];
	result2[2] = f23[1]; result2[3] = f22[1];
	result1[4] = f21[2]; result1[5] = f20[2];
	result2[4] = f23[2]; result2[5] = f22[2];
	result1[6] = f21[3]; result1[7] = f20[3];
	result2[6] = f23[3]; result2[7] = f22[3];
	result1[8] = f21[4]; result1[9] = f20[4];
	result2[8] = f23[4]; result2[9] = f22[4];
    
	short s10[10] = {0};
	short s11[10] = {0};

    // Intermediate result as the input for the convolution
	for (int i = 0; i < 10; i++) { //TODO loop unrolling or loop termination
		s10[i] = result1[i] + result2[i];
		s11[i] = result1[i] - result2[i];
	}
	

	// Update the chunk
	for (int i = 0; i<25; i++) { //TODO loop unrolling or loop termination
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

	//for (int i = 0; i<20; i+=2) {
	//	result[i] = f11[i/2];
	//	result[i+1] = f10[i/2];
	//}
	//loop unrolling
	result[0] = f11[0];
	result[1] = f10[0];
	result[2] = f11[1];
	result[2] = f10[1];
	result[4] = f11[2];
	result[5] = f10[2];
	result[6] = f11[3];
	result[7] = f10[3];
	result[8] = f11[4];
	result[9] = f10[4];
}

/* Convolution Stage 1 decoder
 Function that performs the convolution in the first stage of synthesis
 Inputs:    - s2x: intermediate result with which the convolution has to be performed
 Outputs:   - f2x: The results from the convolution are stored in these variables
 */
void ConvolutionStage1dec(short * s20,short * s21, short * s22, short * s23, long long * f20, long long * f21, long long * f22, long long * f23) {
	  for (int i = 7; i<12; i++) {
		  for (int j=0; j<8;j++) {
			f21 [i-7] += s21[i-j]*f2_even[j];
			f20 [i-7] += s20[i-j]*f2_odd[j];
			f23 [i-7] += s23[i-j]*f2_even[j];
			f22 [i-7] += s22[i-j]*f2_odd[j];
              
		  }
          f21[i-7] = f21[i-7]>>14;//avoid division
          f20[i-7] = f20[i-7]>>14;//avoid division
          f23[i-7] = f23[i-7]>>14;//avoid division
          f22[i-7] = f22[i-7]>>14;//avoid division
        
	  }
}

/* Convolution Stage 2 decoder
 Function that performs the convolution in the second stage of synthesis
 Inputs:    - s1x: intermediate result with which the convolution has to be performed
 Outputs:   - f1x: The results from the convolution are stored in these variables
 */
void ConvolutionStage2dec(short * s10, short * s11, long long * f10, long long * f11) {
	  for (int i = 15; i<25; i++) {
		  for (int j=0; j<16;j++) {
			  f11[i-15] += s11[i-j]*f0_even[j];
			  f10[i-15] += s10[i-j]*f0_odd[j];
		  }
          f11[i-15] = f11[i-15]>>14; //avoid division
          f10[i-15] = f10[i-15]>>14; //avoid division
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
    ADPCMdecoderSubband(subband1,mu_1,&decoderChunk->stepsize1,decoderChunk->deltaPrimeArray1,phi_4_updated,&decoderChunk->prevoutput1);
    
    /* Decode Subband2 */
    ADPCMdecoderSubband(subband2,mu_2,&decoderChunk->stepsize2,decoderChunk->deltaPrimeArray2,phi_4_updated,&decoderChunk->prevoutput2);
    
    /* Decode Subband3 */
    ADPCMdecoderSubband(subband3,mu_3,&decoderChunk->stepsize3,decoderChunk->deltaPrimeArray3,phi_2_updated,&decoderChunk->prevoutput3);
    
    /* Decode Subband4 */
    ADPCMdecoderSubband(subband4,mu_4,&decoderChunk->stepsize4,decoderChunk->deltaPrimeArray4,phi_2_updated,&decoderChunk->prevoutput4);
    
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
void ADPCMdecoderSubband(short * subbandSignal, short mu, short * stepsize, short * deltaPrimeArray, short phi_updated, short * prevoutput) {
    
    short delta_prime;
    for (int i = 0; i<5; i++) {
        
        // Shift delta prime array to get the 10 most recent samples for the stepsize calculation
        //for (int j = 0; j < 9; j++) {
        //    deltaPrimeArray[j] = deltaPrimeArray[j+1];
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

        // Calculate new delta prime
        delta_prime = subbandSignal[i] * *stepsize;
        deltaPrimeArray[9] = delta_prime;
        
        // Update stepsize
        *stepsize = calculateStepsize(deltaPrimeArray,phi_updated);
        if (*stepsize == 0) {
            *stepsize = 1;
        }
        
        // Calculate output
        subbandSignal[i] = delta_prime + ((mu * *prevoutput) >> 15); //delta_prime + (mu * *prevoutput) / pow(2, 15) changed to delta_prime + ((mu * *prevoutput) >> 15) to avoid division
        
        // Update prevoutput
        *prevoutput = subbandSignal[i];
        
    }
}
