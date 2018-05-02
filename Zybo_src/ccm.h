/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ccm.h
 * Author: Stephan
 *
 * Created on 12 april 2018, 21:35
 */
#include "aes.h"
#ifndef CCM_H
#define CCM_H

void format_NAP(uint8_t *output, uint64_t A, uint8_t *P, uint64_t p, uint8_t t);
void format_CTR(uint8_t *output, uint64_t p);
void format_MAC(uint8_t *T, uint8_t t, aes_key *key, uint8_t *B, uint64_t b);
void ccm_encrypt(uint8_t *C, uint8_t *key, uint64_t A, uint8_t *P, uint64_t p, uint8_t t);
int ccm_decrypt(uint8_t *P, uint8_t *key, uint8_t *C, uint64_t c, uint64_t A, uint8_t t);
#endif /* CCM_H */

