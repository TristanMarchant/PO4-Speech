
#include "common.h"
#include "interface.h"

#include "hw_accelerator.h"

// Note that these tree CMDs are same as
// they are defined in montgomery_wrapper.v
#define CMD_READ    0
#define CMD_COMPUTE 1
#define CMD_WRITE   2

void init_HW_access(void)
{
	interface_init();

	// Test the port-based communication
	test_dma_transfer();
}

void example_HW_accelerator(void)
{
	//// --- Create and initialize a 1024-bit src array
	//       as 32 x 32-bit words.
	//       src[ 0] is the least significant word
	//       src[31] is the most  significant word

	unsigned int src[DMA_TRANSFER_NUMBER_OF_WORDS]={
		0x00000000, 0x00000000, 0x00000000, 0x00000000,
		0x00000000, 0x00000000, 0x00000000, 0x00000000,
		0x00000000, 0x00000000, 0x00000000, 0x00000000,
		0x00000000, 0x00000000, 0x00000000, 0x00000000,
		0x00000000, 0x00000000, 0x00000000, 0x00000000,
		0x00000000, 0x00000000, 0x00000000, 0x00000000,
		0x00000000, 0x00000000, 0x00000000, 0x00000000,
		0x89abcdef, 0x01234567, 0x00000000, 0x00000000};

	//// --- Perform the send operation

	// Start by writing CMD_READ to port1 (command port)
	xil_printf("PORT1=%x\n\r",CMD_READ);

START_TIMING
	accelerator_port[0] = CMD_READ;

	// Transfer the src array to BRAM
	bram_dma_transfer(dma_config,src,DMA_TRANSFER_NUMBER_OF_WORDS);

	// Wait for done of CMD_READ is done
	// by waiting for port2 (acknowledgement port)
	port2_wait_for_done();
STOP_TIMING

	//// --- Print BRAM contents

	// For checking if send is successful
	print_bram_contents();


	//// --- Perform the compute operation

	// Start by writing CMD_COMPUTE to port1 (command port)
	xil_printf("PORT1=%x\n\r",CMD_COMPUTE);

START_TIMING
	accelerator_port[0] = CMD_COMPUTE;

	// Wait for done of CMD_COMPUTE is done
	// by waiting for port2 (acknowledgement port)
	port2_wait_for_done();
STOP_TIMING

	//// --- Perform the read operation

	// Start by writing CMD_WRITE to port1 (command port)
	xil_printf("PORT1=%x\n\r",CMD_WRITE);

START_TIMING
	accelerator_port[0] = CMD_WRITE;

	// Wait for done of CMD_WRITE is done
	port2_wait_for_done(); //Wait until Port2=1
STOP_TIMING

	//// --- Print BRAM contents

	// For receiving the read output of the computation
	print_bram_contents();
}
