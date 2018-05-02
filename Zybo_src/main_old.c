#include "common.h"

//#include "hw_accelerator.h"

int main_old()
{
    init_platform();
    init_performance_counters(1);

    xil_printf("Begin\n\r");

START_TIMING
    	xil_printf("Hello World!\n\r");
STOP_TIMING

//    init_HW_access();
//    example_HW_accelerator();

	xil_printf("End\n\r");

    cleanup_platform();

    return 0;
}
