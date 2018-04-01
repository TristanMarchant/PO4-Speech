#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "wavpcm_io.h"
#include "globals.h"
#include "transmitter.h"
#include "receiver.h"
//#include "transmitterTest.h"

//bool transmitterTest();

/* This is the function that is called when the program starts. */
int main (int argc, char *argv[])
{
	//disp(transmitterTest());
  /* Variable declarations. */
  struct wavpcm_input input;
  struct wavpcm_output output;
  short buffer[BUFFERSIZE]; 
  short reconstructedBuffer[BUFFERSIZE];
  int bufPos, bufIndex, read;
  unsigned short encodedBuffer[8];

  // Our struct initialization
  struct encoderChunk encoderChunkLeft;
  memset(encoderChunkLeft.prevBufferEven,0,25*sizeof(short));
  memset(encoderChunkLeft.prevBufferOdd,0,25*sizeof(short));
  memset(encoderChunkLeft.prevBufferC0Even,0,12*sizeof(short));
  memset(encoderChunkLeft.prevBufferC0Odd,0,12*sizeof(short));
  memset(encoderChunkLeft.prevBufferC1Even,0,12*sizeof(short));
  memset(encoderChunkLeft.prevBufferC1Odd,0,12*sizeof(short));
  struct encoderChunk encoderChunkRight;
  memset(encoderChunkRight.prevBufferEven,0,25*sizeof(short));
  memset(encoderChunkRight.prevBufferOdd,0,25*sizeof(short));
  memset(encoderChunkRight.prevBufferC0Even,0,12*sizeof(short));
  memset(encoderChunkRight.prevBufferC0Odd,0,12*sizeof(short));
  memset(encoderChunkRight.prevBufferC1Even,0,12*sizeof(short));
  memset(encoderChunkRight.prevBufferC1Odd,0,12*sizeof(short));


  memset(&input, 0, sizeof(struct wavpcm_input));
  input.resource=INPUTWAVFILE;
  memset(&output, 0, sizeof(struct wavpcm_output));
  output.resource=OUTPUTWAVFILE;

  /* First open input file and parse header, */
  wavpcm_input_open (&input);
  /* and then use this same header configuration for the output file */
  wavpcm_output_copy_settings(&input, &output);
  wavpcm_output_open(&output);
  
  /*bufPos expressed in temporal samples*/
  for (bufPos=0; bufPos<input.samplesAvailable; bufPos+=(BUFFERSIZE/2)) {
    /* Try to read BUFFERSIZE samples (16 bits, pairwise identical if input is mono, interleaved if input is stereo)  */
    /* into buffer, with read the actual amount read (expressed in bytes! =  (2*read)/(channels * bitDepth/8) array elements)*/
    read = wavpcm_input_read (&input, buffer);

    /* transform buffer (ENCODER) */
	transmitter(buffer, &encoderChunkLeft, &encoderChunkRight, encodedBuffer);

    /* if required, dump compressed output */

    /* inverse transform buffer (DECODER)*/
	//receiver(encodedBuffer, buffer);
    for (bufIndex=0; bufIndex<BUFFERSIZE; bufIndex++)
      reconstructedBuffer[bufIndex]=buffer[bufIndex];

    /* dump reconstructed output */
    wavpcm_output_write (&output, reconstructedBuffer, read);
  }

  /* finalize output (write header) and close */
  wavpcm_output_close (&output);  

  /* Return successful exit code. */
  return 0;
}
