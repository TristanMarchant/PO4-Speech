#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "wavpcm_io.h"
#include "globals.h"
#include "transmitter.h"
#include "receiver.h"
#include "ccm.h"
#include "tools.h"
#include "common.h"

/* This is the function that is called when the program starts. */
int main (int argc, char *argv[])
{
	init_platform();
	init_performance_counters(1);

    /* Variable declarations. */
    struct wavpcm_input input;
    struct wavpcm_output output;
    short buffer[BUFFERSIZE];
    short reconstructedBuffer[BUFFERSIZE];
    int bufPos, bufIndex, read;
    unsigned short encodedBuffer[8];
    
    
    /* Struct initialization */
    struct encoderChunk encoderChunkLeft;
    memset(&encoderChunkLeft,0,sizeof(struct encoderChunk));
    
    struct encoderChunk encoderChunkRight;
    memset(&encoderChunkRight,0,sizeof(struct encoderChunk));
    
    struct decoderChunk decoderChunkLeft;
    memset(&decoderChunkLeft,0,sizeof(struct decoderChunk));
    decoderChunkLeft.stepsize1 = 1;
    decoderChunkLeft.stepsize2 = 1;
    decoderChunkLeft.stepsize3 = 1;
    decoderChunkLeft.stepsize4 = 1;
    
    struct decoderChunk decoderChunkRight;
    memset(&decoderChunkRight,0,sizeof(struct decoderChunk));
    decoderChunkRight.stepsize1 = 1;
    decoderChunkRight.stepsize2 = 1;
    decoderChunkRight.stepsize3 = 1;
    decoderChunkRight.stepsize4 = 1;
    
    memset(&input, 0, sizeof(struct wavpcm_input));
    input.resource=INPUTWAVFILE;
    memset(&output, 0, sizeof(struct wavpcm_output));
    output.resource=OUTPUTWAVFILE;
    
    /* Interface with encryption */
    unsigned short data[DATASIZE];
    unsigned char cipherBuffer[76]; //  28 = 16 + 12
    unsigned char KeyA[16] = {0x74,0xf7,0xc9,0x44,0xf9,0x1a,0x61,0xbf,0xd5,0x8c,0x67,0x8d,0x4c,0x7d,0x28,0xb8};
    unsigned char KeyB[16] = {0x74,0xf7,0xc9,0x44,0xf9,0x1a,0x61,0xbf,0xd5,0x8c,0x67,0x8d,0x4c,0x7d,0x28,0xb8};
    int secure;
    unsigned char plaintext[64];
    unsigned char plaintext_dec[64];
    uint64_t Ad = 0;
    
    /* First open input file and parse header, */
    wavpcm_input_open (&input);
    /* and then use this same header configuration for the output file */
    wavpcm_output_copy_settings(&input, &output);
    wavpcm_output_open(&output);
    /*bufPos expressed in temporal samples*/
    for (bufPos=0; bufPos<input.samplesAvailable; bufPos+=(BUFFERSIZE*2)) {
        /* Try to read BUFFERSIZE samples (16 bits, pairwise identical if input is mono, interleaved if input is stereo)  */
        /* into buffer, with read the actual amount read (expressed in bytes! =  (2*read)/(channels * bitDepth/8) array elements)*/
        int l = 0;
        int bufPos2,index,k;
        for (bufPos2 = 0; bufPos2 < (BUFFERSIZE*2); bufPos2+=(BUFFERSIZE/2)) {
            read = wavpcm_input_read (&input, buffer);
            transmitter(buffer, &encoderChunkLeft, &encoderChunkRight, encodedBuffer);
            for (index = 0; index < 8; index++) {
                data[index+l] = encodedBuffer[index];
            }
            l += 8;
        }
        l = 0;
        
        /* INTERFACE CRYPTO */
        int16_8(data,plaintext,32);
        ccm_encrypt(cipherBuffer,KeyA,Ad,plaintext,64,12);
        secure = ccm_decrypt(plaintext_dec,KeyB,cipherBuffer,76,Ad,12);
        int8_16(plaintext_dec,data,64);
        
        for (k = 0; k < 4; k++) {
            for (index = 0; index < 8; index++) {
                encodedBuffer[index] = data[index+l];
            }
            l+=8;
            receiver(encodedBuffer, buffer, &decoderChunkLeft, &decoderChunkRight);
            for (bufIndex=0; bufIndex<BUFFERSIZE; bufIndex++){
                reconstructedBuffer[bufIndex]=buffer[bufIndex];
            }
            wavpcm_output_write (&output, reconstructedBuffer, read);
        }
        
        /* transform buffer (ENCODER) */
        
        
        /* if required, dump compressed output */
        
        /* INTERFACE CRYPTO */
        int16_8(encodedBuffer,plaintext,8);
        ccm_encrypt(cipherBuffer,KeyA,Ad,plaintext,16,8);
        secure = ccm_decrypt(plaintext_dec,KeyB,cipherBuffer,28,Ad,8);
        int8_16(plaintext_dec,encodedBuffer,16);
        
        /* inverse transform buffer (DECODER)*/
        
        
       
        
        /* dump reconstructed output */
       
        
    }
    printf("HEY");
    /* finalize output (write header) and close */
    wavpcm_output_close (&output);
    
    /* Return successful exit code. */
    cleanup_platform();

    return 0;
}
