
#include "libutil.h"
#include "libhuff.h"

#include <string.h>

void
calc_code_values(huff_t* h)
{
    uint32_t l,i,j,n;
    uint8_t code_of_len[ALPHABET_SIZE] = {0};

    /* compute min max and code_of_len */
    h->min=255;
    h->max=0;
    for (i=0; i<ALPHABET_SIZE; i++) {
        j=h->len[i];
        if (j>0) {
            code_of_len[j]++;
            if (j>h->max) h->max = j;
            if (j<h->min) h->min = j;
        }
    }

    /* init maxrank */
    n = 0;
    for (i=h->min; i<=h->max; i++) {
        n += code_of_len[i];
        h->max_rank[i] = n-1;
    }

    /* init limit: limit[i] is the largest code of a symbol of length i */
    h->limit[h->min]=code_of_len[h->min]-1;
    for (i=h->min+1; i<=h->max; i++) {
        h->limit[i] = ((h->limit[i-1]+1)<<1)+code_of_len[i]-1;
    }

    /* compute codes */
    for (i=0; i<ALPHABET_SIZE; i++) {
        l=h->len[i];
        h->val[i] = h->limit[l]-(--code_of_len[l]);
    }
}

uint8_t
decode_symbol(huff_t* h,bit_file_t* f)
{
    uint32_t i,offset,codeword;

    i = 0;
    codeword = 0;
    while (i < h->min) {
        codeword = (codeword<<1) + BitFileGetBit(f);
        i++;
    }

    while (codeword > h->limit[i]) {
        codeword = (codeword<<1) + BitFileGetBit(f);
        i++;
    }

    offset = h->max_rank[i] - (h->limit[i]-codeword);
    return h->syms[offset];
}

#define CHECK_BIT(var,pos) !!((var) & (1 << (pos)))

uint8_t*
decode_huffman(bit_file_t* f,int32_t* osize)
{
    uint8_t* text;
    uint32_t i;
    uint32_t n;

    huff_t hdata;

    memset(&hdata,0,sizeof(huff_t));

    /* read tree */
    /* read symbols */
    hdata.nsyms = BitFileGetChar(f);
    hdata.nsyms++; /* we store n-1 in the encoder */
    for (i=0; i<hdata.nsyms; i++) {
        hdata.syms[i] = BitFileGetChar(f);
    }
    /* read code lengths */
    for (i=0; i<hdata.nsyms; i++) {
        hdata.len[hdata.syms[i]] = BitFileGetChar(f);
    }

    /* generate code words */
    calc_code_values(&hdata);

    /* get number of chars */
    BitFileGetBitsInt(f,&n,32,sizeof(n));

    /* malloc */
    text = safe_malloc(n*sizeof(uint8_t));

    /* decode text */
    for (i=0; i<n; i++) {
        text[i] = decode_symbol(&hdata,f);
    }
    *osize = n;

    return text;
}

