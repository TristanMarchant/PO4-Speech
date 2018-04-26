#ifndef _INTERFACE_H_
#define _INTERFACE_H_

//Divide by four to get the 32-bit offsets
#define MM2S_DMACR_OFFSET   (0/4)
#define MM2S_SA_OFFSET      (0x18/4)
#define MM2S_LENGTH_OFFSET  (0x28/4)

#define DMA_TRANSFER_NUMBER_OF_WORDS 32

unsigned int * dma_config;
unsigned int * accelerator_port;
unsigned int * accelerator_data;

void interface_init();
void bram_dma_transfer(unsigned int* dma_config, unsigned int * data_addr, unsigned int data_len);
void test_dma_transfer();
void port2_wait_for_done();
void print_bram_contents();

#endif
