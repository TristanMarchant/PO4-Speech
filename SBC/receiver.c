#include "globals.h"

const int f2_odd[8] = {1300, 509, -3745, 15738, 61720, -13085, 6341, -3248};
const int f2_even[8] = {-3248, 6341, -13085,	61720, 15738, -3745, 509, 1300};
const int f0_odd[16] = {639,	-385, -2, 578, -1499, 3093, -6403, 17965, 60424,	-12952,	7340, -4929,	 3471, -2439, 1638, -922};
const int f0_even[16] = {-922, 1638,	-2439, 3471, -4929, 7340, -12952, 60424, 17965, -6403, 3093, -1499, 578, -2, -385, 639};

/* decode */
void receiver(unsigned short encodedBuffer[8], short *buffer)
{
	//TODO inverse ADPCM left
	//TODO inverse ADPCM right
	//TODO synthesis
}
