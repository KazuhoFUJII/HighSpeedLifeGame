#include <stdint.h>
#include <limits.h>

//     ^ Y
//     |
// X<--
#define NX 64  // num of bits in field on X direction. multiple of 8
#define NY 64  // num of bits in field on Y direction. multiple of 2
#define NC (NX/CHAR_BIT/sizeof(uint8_t)+1)

#define NUM_THREAD 4

//config
#define EXEC_STEP   100 
#define PRINT_PBM     0 // 1: on; 0: off
#define DEAD_OR_ALIVE 1 // 1: on; 0: off

#define SEED 0.364021
