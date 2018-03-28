
/* represents buffer array elements: equivalent to BUFFERSIZE/2 temporal sample positions */
#define BUFFERSIZE 40

#define INPUTWAVFILE  "input.wav"
#define OUTPUTWAVFILE "output.wav"

#define nbTaps1 32
#define nbTaps2 16

#define mu_1 21774
#define mu_2 7402
#define mu_3 5164
#define mu_4 -3139

#define bpsb {4,4,2,2}

#define codebook_4 {-8,-7,-6,-5,-4,-3,-2,-1,0,1,2,3,4,5,6,7};
#define codebook_2 {-2,-1,0,1};

#define phi_4 = 105;
#define phi_2 = 293;

#define VERBOSE