#include <stdint.h>
#include <limits.h>

//     ^ Y
//     |
// X<--
#define NX 8192  // num of bits in field on X direction. multiple of 8
#define NY NX  // num of bits in field on Y direction. multiple of 2

typedef uint16_t scan_t;
#define SCAN_BLOCK (sizeof(scan_t)*8/2/2) 

#define NC (NX/8/sizeof(uint8_t)+sizeof(scan_t)/sizeof(uint8_t)/2)
#define ND (NX/8/sizeof(uint8_t))

#define NUM_THREAD 4

//config
#define EXEC_STEP   100000 
#define PRINT_PBM     0 // 0: off; 1: print blk|wht; 2: print dead|alv 
#define DEAD_OR_ALIVE 1 // 0: off; 1: on

#define SEED 0.364021
