/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#define _POSIX_C_SOURCE 199309L

#include <stdint.h>
#include <stdio.h>
#include "tools.h"
#include <math.h>
#include <time.h>
#include <stdlib.h>


void int32_8_64(uint32_t *input, uint8_t *output){
    int i;
    for(i=0;i<64;i++){
        output[i*4 + 0] = input[i] >> 0;
        output[i*4 + 1] = input[i] >> 8;
        output[i*4 + 2] = input[i] >> 16;
        output[i*4 + 3] = input[i] >> 24;
    }
}


void int8_32_32(uint8_t *input, uint32_t *output){
    int i;
    uint32_t temp;
    for(i=0;i<8;i++){
        temp = ((uint32_t) input[4*i + 0]) << 24;
        temp += ((uint32_t) input[4*i + 1]) << 16;
        temp += ((uint32_t) input[4*i + 2]) << 8;
        temp += ((uint32_t) input[4*i + 3]) << 0;       
        output[i] = temp;
    }
}

void int16_8(uint16_t *input, uint8_t *output, int length_input) {
    int i;
    for(i=0;i<length_input;i++){
        output[i*2] = input[i] >> 8;
        output[i*2 + 1] = input[i];
    }
}

void int8_16(uint8_t *input, uint16_t *output, int length_input) {
    int i;
    uint16_t temp;
    for(i=0;i<(length_input/2);i++){
        temp = ((uint16_t) input[2*i]) << 8;
        temp += ((uint16_t) input[2*i+1]);
        output[i] = temp;
    }
}

void mult(uint32_t *x, uint32_t *y, int n, int t, uint32_t *res) {
    uint32_t W[100];
    uint64_t uv;
    uint32_t c;
    int i = 0;
    int j = 0;
    n -= 1;
    t -= 1;
    for (i=0;i<100;i++) {
            W[i] = 0;
    }
    
    for (i=0; i<=t; i++){
        c = 0;
        for (j=0; j<=n; j++){
            uv = (uint64_t) W[i+j] + (uint64_t) x[j] * (uint64_t) y[i] + (uint64_t) c;
            W[i+j] = (uint32_t) uv;
            uv = uv >> 32;
            c = (uint32_t) uv;
        }
        W[i+n+1] = c;
    }

    for (i=0; i<=(n+t+1); i++) {
        res[i] = W[i];
    }
}

void mod(uint32_t *a, int a_size, uint32_t *N, int N_size, uint32_t *res) {    
    uint32_t A_copy[200], A_res[200], N_Shift[200], N_copy[200];
    int i;
    
    for (i=0; i<200; i++) {
        A_res[i] = 0;
        if (i < a_size) {
            A_copy[i] = a[i];
        } else {
            A_copy[i] = 0;
        }
        
        if (i < N_size) {
            N_copy[i] = N[i];
        } else {
            N_copy[i] = 0;
        }
                
        if ((a_size - (N_size + i)) > 0) {
            N_Shift[i] = 0;
        } else if((N_size + i) - a_size < N_size) { 
            N_Shift[i] = N[(N_size + i) - a_size];
        } else {
            N_Shift[i] = 0;
        }
    }
    
    while (compareLarger(A_copy, N_copy, a_size)) {
        if (compareLarger(A_copy, N_Shift, a_size)) {
            mp_sub(A_copy, N_Shift, A_res, a_size);
            for (i=0; i<a_size; i++) {
                A_copy[i] = A_res[i];
            }
        }
        shift_R_1(N_Shift, a_size, N_Shift);        
    }
    
    for (i=0; i<N_size; i++) {
        res[i] = A_copy[i];
    }
}

int compareLarger(uint32_t *x, uint32_t *y, int size){
    int i = 1;
    
    while(i<(size+1)){
        if(x[size-i]==y[size-i]){
            i = i + 1;
        }
        else if(x[size-i]>y[size-i]){
            return 1;
        }
        else{
            return 0;
        }
    }
    return 0;    
}

void mp_sub(uint32_t *a, uint32_t *b, uint32_t *res, uint32_t size) {
	int64_t c = 0;
	int i;
	for (i=0; i < size; i++) {
		int64_t temp = (int64_t) a[i] - (int64_t) b[i] + c;
		res[i] = (uint32_t) temp;
		if (temp >= 0) {
                    c = 0;}
		else {
                    c = -1;}
	}
}

int mp_add(uint32_t *a, uint32_t *b, uint32_t *res, uint32_t size)
{
	uint64_t c = 0;
	int i;
	for (i=0; i < size; i++) {
            uint64_t temp = (uint64_t) a[i] + (uint64_t) b[i] + c;
	    res[i] = (uint32_t) temp;
	    if (temp > (pow(2,32) - 1)) {
	    c = 1;}
	    else {
	    c = 0;}
	}
	res[size] = (uint32_t) c;
	return c;
}

int mp_add_8(uint8_t *a, uint8_t *b, uint8_t *res, int size)
{
	uint16_t c = 0;
	int i;
	for (i=0; i < size; i++) {
            uint16_t temp = (uint16_t) a[i] + (uint16_t) b[i] + c;
	    res[i] = (uint8_t) temp;
	    if (temp > (pow(2,8) - 1)) {
	    c = 1;}
	    else {
	    c = 0;}
	}
	res[size] = (uint8_t) c;
	return c;
}

void shift_R_1(uint32_t *a, int size, uint32_t *res) {
    int i;
    
    for (i=0; i<size; i++) {
        res[i] = a[i] >> 1;
        if (i<size-1 && a[i+1] & 1) {
            res[i] += 0x80000000;
        }       
    }
}

/* Generate random number in ]L,R[*/
void rand_interval(uint32_t *L, uint32_t *R, int size, uint32_t *res) {
    uint32_t W[200];
    int i = 0;
 
    for (i=0; i<200; i++) {
        W[i] = 0;
    }
    
    i = 0;
    while (i < size) {
        W[size-1-i] = rand32b();       
        if (compareLarger(R,W,size) & compareLarger(W,L,size)) {
            i += 1;
        }
    }
    
    for (i=0; i<size; i++) {
        res[i] = W[i];
    }
}

uint32_t rand32b() {
    uint32_t W = 0;
    struct timespec t;
    int j;
    srand((unsigned) t.tv_nsec);
    for (j=0; j<32; j++) {
        W += (rand() % 2) << j;
    }
    return W;
    
}

void int64_array8(uint8_t *res, uint64_t input) {
    int i;
    
    for (i=0; i<8; i++) {
        res[7-i] = input >> (8*i); 
    }
}

int compareZero(uint32_t *x, int size){
    
    int i = 0;

    for(i=0; i<size; i++) {
        if(x[i]!=0){
            return 0;
        }
    }
    return 1;
}

int compareLargerEqual(uint32_t *x, uint32_t *y, int size){
    int i = 1;
    
    while(i<(size+1)){
        if(x[size-i]==y[size-i]){
            i = i + 1;
        }
        else if(x[size-i]>y[size-i]){
            return 1;
        }
        else{
            return 0;
        }
    }
    return 1;
    
    
}

int equalsOne(uint32_t *x, int size){
    int i;
    
    for(i=1; i<size; i++){
        if(x[i]!=0){
            return 0;
        }
    }
    
    if(x[0]==1){
        return 1;
    }
    else{
        return 0;
    }
    
}

int equal(uint32_t *x, uint32_t *y, int size){
    int i;
    
    for(i=0; i<size; i++)
        if(x[i]!=y[i]){
            return 0;
        }
    return 1;
    
}

