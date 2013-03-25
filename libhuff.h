/*
 * File:   libhuff.h
 * Author: matt
 *
 * Created on 1 December 2010, 5:38 PM
 */

#ifndef LIBHUFF_H
#define	LIBHUFF_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "libutil.h"

    typedef struct {
        uint8_t min;
        uint8_t max;
        uint32_t nsyms;
        uint8_t syms[ALPHABET_SIZE];
        uint8_t len[ALPHABET_SIZE];
        uint32_t val[ALPHABET_SIZE];
        uint32_t limit[ALPHABET_SIZE];
        uint32_t max_rank[ALPHABET_SIZE];
    } huff_t;

    uint8_t* decode_huffman(bit_file_t* f,int32_t* n);

#ifdef	__cplusplus
}
#endif

#endif	/* LIBHUFF_H */

