/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   tools.h
 * Author: Stephan
 *
 * Created on 1 april 2018, 19:29
 */

#ifndef TOOLS_H
#define TOOLS_H

#include <stdint.h>

/* Converts 64 array of 32 bits to 256 array of 8 bits*/
void int32_8_64(uint32_t *input, uint8_t *output);

/* Converts 32 array of 8 bits to 8 array of 32 bits*/
void int8_32_32(uint8_t *input, uint32_t *output);

void mult(uint32_t *x, uint32_t *y, int n, int t, uint32_t *res);
void mod(uint32_t *a, int a_size, uint32_t *N, int N_size, uint32_t *res);
int mp_add(uint32_t *a, uint32_t *b, uint32_t *res, uint32_t size);
int compareLarger(uint32_t *x, uint32_t *y, int size);
void mp_sub(uint32_t *a, uint32_t *b, uint32_t *res, uint32_t size);
void shift_R_1(uint32_t *a, int size, uint32_t *res);
int mp_add_8(uint8_t *a, uint8_t *b, uint8_t *res, int size);
void int64_array8(uint8_t *res, uint64_t input);
int compareZero(uint32_t *x, int size);
int compareLargerEqual(uint32_t *x, uint32_t *y, int size);
int equalsOne(uint32_t *x, int size);
int equal(uint32_t *x, uint32_t *y, int size);
void int16_8(uint16_t *input, uint8_t *output, int length_input);
void int8_16(uint8_t *input, uint16_t *output, int length_input);

/* Generate random number in ]L,R[*/
void rand_interval(uint32_t *L, uint32_t *R, int size, uint32_t *res);
uint32_t rand32b();

#endif /* TOOLS_H */

