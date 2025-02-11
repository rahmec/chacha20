#include <stdio.h> // I/O handling
#include <stdint.h> // uint handling
#include <string.h> // memory handling

#define ROTL(a,b) (((a) << (b)) | ((a) >> (32-(b))))

const uint32_t C0 = 0x12341234;
const uint32_t C1 = 0x56785678;
const uint32_t C2 = 0x91011910;
const uint32_t C3 = 0xaabbccdd;

typedef uint32_t seed256[8];
typedef uint32_t ctr64[2];
typedef uint32_t nonce64[2];
typedef uint32_t block512[16];

void pad_blocks(seed256 seed, ctr64 ctr, nonce64 nonce, block512 block);
void permute_block(block512 block);
void quarter_round(block512 block, int a, int b, int c, int d);
void print_block(uint32_t* block, char label[], int length);
void chacha20_PRG(block512 output[], seed256 seed, ctr64 ctr, nonce64 nonce,
                  int length);
void print_output(block512 output[], int length);

int main()
{
    seed256 seed = {
        0x11111111, 0x22222222, 0x33333333, 0x44444444,
        0x55555555, 0x66666666, 0x77777777, 0x88888888,
    };

    ctr64 ctr = {
        0x99999999, 0xaaaaaaaa
    };

    nonce64 nonce = {
        0xbbbbbbbb, 0xcccccccc
    };

    int length = 10;
    block512 output[10];

    chacha20_PRG(output,seed, ctr, nonce, length);
    print_output(output,length);

    return 0;
}

/*
 * Function: pad_blocks 
 * --------------------
 *  Padding function
 *
 *  seed: 256-bit seed (8 32-bit blocks s0,...,s7)
 *  ctr: 64-bit counter (2 32-bit blocks c0,c1)
 *  nonce: 64-bit nonce (2 32-bit blocks n0,n1)
 *  block: 512-bit block (16 32-bit blocks b0...b15)
*/
void pad_blocks(seed256 seed, ctr64 ctr, nonce64 nonce, block512 block)
{
    uint32_t paddedBlock[16] = {
       C0,         C1,         C2,         C3, 
       seed[0],    seed[1],    seed[2],    seed[3], 
       seed[4],    seed[5],    seed[6],    seed[7], 
       ctr[0],     ctr[1],     nonce[0],   nonce[1]
    };

    memcpy(block, paddedBlock, sizeof (block512));
}

/*
 * Function: permute_block
 * -----------------------
 *  Permutation function
 *
 *  block: 512-bit block (16 32-bit blocks b0,...,b15)
*/
void permute_block(block512 block)
{
    // Apply to four columns
    quarter_round(block, 0,4,8,12);
    quarter_round(block, 1,5,9,13);
    quarter_round(block, 2,6,10,14);
    quarter_round(block, 3,7,11,15);

    // Apply to four diagonals with wrap
    quarter_round(block, 0,5,10,15);
    quarter_round(block, 1,6,11,12);
    quarter_round(block, 2,7,8,13);
    quarter_round(block, 3,4,9,14);
}

/*
 * Function: quarter_round
 * -----------------------
 *  ChaCha20 permutation round
 *
 *  block: 512-bit block (16 32-bit blocks b0,...,b15)
 *  a,b,c,d: block sub-blocks' indexes
*/
void quarter_round(block512 block, int a, int b, int c, int d)
{
   uint32_t* x_a = &block[a]; 
   uint32_t* x_b = &block[b]; 
   uint32_t* x_c = &block[c]; 
   uint32_t* x_d = &block[d]; 
   *x_a += *x_b; *x_d ^= *x_a; ROTL(*x_d,16);
   *x_c += *x_d; *x_b ^= *x_c; ROTL(*x_b,12);
   *x_a += *x_b; *x_d ^= *x_a; ROTL(*x_d,8);
   *x_c += *x_d; *x_b ^= *x_c; ROTL(*x_b,7);
}

void chacha20_PRG(block512 pr_block[], seed256 seed, ctr64 ctr, nonce64 nonce,
                  int length){
    for(int i=0; i<length; i++){
        block512 padded_block;
        pad_blocks(seed, ctr, nonce, padded_block);

        block512 permuted_block;
        memcpy(permuted_block, padded_block, sizeof (block512));
        //memcpy(permuted_block, block, (sizeof (uint32_t))*16); // other way
        
        // 10 sets of permutations
        for(int j=0; j<10; j++){
            permute_block(permuted_block);
        }

        // Blockwise xor (pairwise word addition mod 32)
        for(int j=0; j<16; j++){
            pr_block[i][j] = padded_block[j] ^ permuted_block[j];
        }
        // CTR update (needs fix)
        ctr[0] = ctr[0]+1;
    }
}

void print_block(uint32_t* block, char label[], int length){
    printf("%s: [", label);
    for(int i=0; i<length; i++){
        printf(" 0x%x ", block[i]);
    }
    printf("]\n");
}
void print_output(block512 output[], int length){
    printf("Output: [");
    for(int i=0; i<length; i++){
        for(int j=0; j<16; j++){
            printf(" 0x%x ", output[i][j]);
        }
        printf("\n");
    }
    printf("]\n");
}

/*
* TODO:
* - CTR update that takes into account 2 blocks
*/
