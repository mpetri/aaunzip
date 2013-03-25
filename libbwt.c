
#include "libutil.h"
#include "libbwt.h"


uint8_t* reverse_bwt(uint8_t* in,int32_t n,int32_t I,uint8_t* out)
{
    int32_t i,j,k, occ[256];
    int32_t* LF;

    /* create LF mapping */
    LF = (int32_t*) safe_malloc(n*sizeof(int32_t));

    for (i=0; i<256; i++) occ[i]=0;

    for (i=0; i<n; i++) occ[in[i]]++;

    for (i=1; i<256; i++)  occ[i] += occ[i-1];

    for (i=255; i>0; i--) occ[i] = occ[i-1];

    occ[0]=0;

    for (i=0; i<n; i++) LF[occ[in[i]]++] = i;

    j = I;
    for (i=0; i<n; i++) {
        k = LF[j];
        out[i] = in[k];
        if (k<=I) j=k-1;
        else j=k;
    }

    free(LF);

    return out;
}
