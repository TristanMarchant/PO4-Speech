/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <stdint.h>
#include <stdio.h>
#include "tools.h"
#include "aes.h"

uint8_t q = 8;
uint8_t N[15] = {0};
uint8_t One_8[15] = {1,0};

void format_NAP(uint8_t *output, uint64_t A, uint8_t *P, uint64_t p, uint8_t t) {
    int i, j;
    uint8_t a = 0;
    uint8_t Q[8] = {0};
    uint8_t A_array[8] = {0};
    uint8_t n = 15-q;
    
    /* Block B0 */
    output[0] = 0;
    if (A != 0) {
        output[0] += 0x40;
        a = 8;
    }
    output[0] += ((t-2)/2) << 3;
    output[0] += (q-1);
    
    
    for (i = 1 ; i <= n; i++) {
        output[i] = N[i-1];
    }
    
    int64_array8(Q,p);
    for (i = 1 + n ; i <= q + n; i++) {
        output[i] = Q[i-1-n];
    }
    
    /* Associated Data, i.e. Tag */
    if (a != 0) {
        int64_array8(A_array,A);
        output[16] = 0;
        output[17] = a;
        for (i = 18 ; i < 32; i++) {
            if (i < 18 + a) { 
                output[i] = A_array[i-18];
            } else {
                output[i] = 0;
            }
        }
    }
    
    /* Plaintext */
    if (a == 0) {
        j = 16; 
    } else {
        j = 32;
    }
    for (i = j; i < (j+p+(16 - p%16)); i++) {
        if (i < j+p) {
            output[i] = P[i-j];
        } else {
            output[i] = 0;
        }
    }
}

void format_CTR_S(uint8_t *output, uint64_t p, aes_key *key) {
    int i, j;
    uint8_t Y[16] = {0};
    uint8_t Y_temp[16] = {0};
    uint8_t n = 15-q;
    uint8_t CTR[15] = {0};
    
    Y_temp[0] = q-1;
    for (j = 1 ; j <= n; j++) {
        Y_temp[j] = N[j-1];
    }
    
    for (i=0; i <= (p + (16 - p%16)); i+=16) {
        mp_add_8(CTR, One_8, CTR, 15);
        for (j = (n+1) ; j <= (n+q); j++) {
            Y_temp[j] = CTR[j-1-n];
        }
        
        aes_encrypt(key, Y_temp, Y);
        
        for (j=0; j<16; j++) {
            output[i+j] = Y[j];
        }       
    }
}

void format_MAC(uint8_t *T, uint8_t t, aes_key *key, uint8_t *B, uint64_t b) {
    uint8_t Y[16] = {0};
    uint8_t Y_temp[16] = {0};
    //uint8_t Y[100] = {0};
    //uint8_t Y_temp[100] = {0};
    int i, j;
    //printf("b: %llu \n",b); // Dit is het probleem denk ik: b = 48 en je schrijft Y_temp[47] wat out of range is
    for (i=0; i<16; i++) {
        Y_temp[i] = B[i];
    }
    aes_encrypt(key, Y_temp, Y);
    
    for (i=16; i<b; i+=16) {
        for (j=0; j<16; j++) {
            Y_temp[j] = B[i+j] ^ Y[j];
        }
        aes_encrypt(key, Y_temp, Y);
    }
    
    for (i=0; i<t; i++) {
        T[i] = Y[i];
    }
    //printf("HEY \n"); // Works so it gets to the end of the function and when going back to ccm_encrypt it does the abort trap thing
}

void ccm_encrypt(uint8_t *C, uint8_t *key, uint64_t A, uint8_t *P, uint64_t p, uint8_t t) {
    aes_key aes_key;
    uint8_t B[2000] = {0};
    uint8_t S[2000] = {0};
    uint8_t T[16] = {0};
    uint64_t b = 0;
    int i;
    aes_set_encrypt_key(&aes_key, key, 128);
    
    mp_add_8(N, One_8, N, 15);
    
    format_NAP(B, A, P, p, t);
    
    b = 16 + (p + (16 - p%16)); 
    if (A != 0) {
        b += 16;
    }

    format_MAC(T, t, &aes_key, B, b); // Abort Trap error happens in format_MAC function
    //printf("HEY \n");
    format_CTR_S(S, p, &aes_key);
    
    for (i=0; i<p; i++) {
        C[i] = P[i] ^ S[i+16];
    }
    for (i=0; i<t; i++) {
        C[p+i] = T[i] ^ S[i];
    }
}

int ccm_decrypt(uint8_t *P, uint8_t *key, uint8_t *C, uint64_t c, uint64_t A, uint8_t t) {
    aes_key aes_key_encrypt, aes_key_decrypt;
    uint8_t B[2000] = {0};
    uint8_t S[2000] = {0};
    uint8_t T[16] = {0};
    uint8_t T_calc[16] = {0};
    uint64_t b = 0;
    uint64_t p = c - (uint64_t) t;
    int i;
    aes_set_encrypt_key(&aes_key_encrypt, key, 128);
    aes_set_decrypt_key(&aes_key_decrypt, key, 128);
        
    if (c <= t) {
        return 0;
    }
    format_CTR_S(S, p, &aes_key_encrypt);
    
    for (i=0; i<p; i++) {
        P[i] = C[i] ^ S[i+16];
    }
    for (i=0; i<t; i++) {
        T[i] = C[p+i] ^ S[i];
    }
    
    format_NAP(B, A, P, p, t);
    b = 16 + (p + (16 - p%16)); 
    if (A != 0) {
        b += 16;
    }
    format_MAC(T_calc, t, &aes_key_encrypt, B, b);
    
    for (i=0; i<16; i++) {
        if (T[i] != T_calc[i]) {
            return 0;
            
        }
    }
    return 1;
}
