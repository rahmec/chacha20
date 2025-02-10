#include <stdio.h> // I/O handling
#include <stdint.h> // uint handling
#include <stdlib.h> // memory handling

#define ROTL(a,b) (((a) << (b)) | ((a) >> (32-(b))))

const uint32_t C0 = 0x00000000;
const uint32_t C1 = 0x00000000;
const uint32_t C2 = 0x00000000;
const uint32_t C3 = 0x00000000;

typedef uint32_t seed256[8];
typedef uint32_t ctr64[2];
typedef uint32_t nonce64[2];
typedef uint32_t block512[16];

uint32_t* pad_blocks(seed256 seed, ctr64 ctr, nonce64 nonce);
void permute_block(block512 *block);
void quarterRound(block512 *block, int a, int b, int c, int d);

int main()
{
    printf("Hello World %d\n", 1);
    return 0;
}

/*
 * Function: pad_blocks 
 * -----------------
 *  Padding function
 *
 *  seed: 256-bit seed (8 32-bit blocks s0,...,s7)
 *  ctr: 64-bit counter (2 32 bit blocks c0,c1)
 *  nonce: 64-bit nonce (2 32 bit blocks n0,n1)
 *
 *  returns: 512-bit block (16 32 bit blocks x0,...,x15)
*/
uint32_t* pad_blocks(seed256 seed, ctr64 ctr, nonce64 nonce)
{
    uint32_t paddedBlock[16] = {
       C0,         C1,         C2,         C3, 
       seed[0],    seed[1],    seed[2],    seed[3], 
       seed[4],    seed[5],    seed[6],    seed[7], 
       ctr[0],     ctr[1],     nonce[0],   nonce[1]
    };

    uint32_t* block = malloc(16*sizeof(uint32_t));
    block = paddedBlock;
    
    return block;
}

/*
 * Function: permute_block
 * -----j-----------
 *  Permutation function
 *
 *  block: 512-bit block (8 16-bit blocks b0,...,b7)
 *
 *  returns: 512-bit block (permutation of the original block)
*/
void permute_block(block512* block)
{
    // Apply to four columns
    quarterRound(block, 0,4,8,12);
    quarterRound(block, 1,5,9,13);
    quarterRound(block, 2,6,10,14);
    quarterRound(block, 3,7,11,15);

    // Apply to four diagonals with wrap
    quarterRound(block, 0,5,10,15);
    quarterRound(block, 1,6,11,12);
    quarterRound(block, 2,7,8,13);
    quarterRound(block, 3,4,9,14);
}

void quarterRound(block512 *block, int a, int b, int c, int d)
{
   uint32_t* x_a = *(block + a); 
   uint32_t* x_b = *(block + b); 
   uint32_t* x_c = *(block + c); 
   uint32_t* x_d = *(block + d);
   *x_a += *x_b; *x_d ^= *x_a; ROTL(*x_d,16);
   *x_c += *x_d; *x_b ^= *x_c; ROTL(*x_b,12);
   *x_a += *x_b; *x_d ^= *x_a; ROTL(*x_d,8);
   *x_c += *x_d; *x_b ^= *x_c; ROTL(*x_b,7);
}


