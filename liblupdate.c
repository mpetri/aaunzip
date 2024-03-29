
#include "libutil.h"
#include "liblist.h"
#include "liblupdate.h"

list_t* lupdate_createlist()
{
    uint32_t i;
    list_t* lst;

    lst = list_create();

    for (i=0; i<ALPHABET_SIZE; i++) {
        list_insert(lst,i);
    }

    return lst;
}

uint8_t*
lupdate_simple(uint8_t* in,uint32_t size,uint8_t* out)
{
    uint32_t i;

    for (i=0; i<size; i++) {
        out[i] = in[i];
    }

    return out;
}

uint8_t*
lupdate_movetofront(uint8_t* in,uint32_t size,uint8_t* out)
{
    uint32_t i,j;
    list_t* lst;
    lnode_t* cur;

    lst = lupdate_createlist();

    for (i=0; i<size; i++) {

        j = in[i];

        cur = list_get(lst,j);

        list_movetofront(lst,cur);

        out[i] = (uint8_t) cur->data;
    }

    return out;
}

uint8_t*
lupdate_freqcount(uint8_t* in,uint32_t size,uint8_t* output)
{
    uint32_t j,i;
    list_t* lst;
    lnode_t* found,*tmp;

    lst = lupdate_createlist();

    for (i=0; i<size; i++) {
        j = in[i];

        found = list_get(lst,j);

        output[i] = (uint8_t) found->data;

        /* reorder */
        found->freq++;
        tmp = found->prev;
        while (tmp != NULL && found->freq > tmp->freq) {
            tmp = tmp->prev;
        }

        if (tmp == NULL) {
            list_movetofront(lst,found);
        } else {
            list_moveafter(found,tmp);
        }
    }

    return output;
}

/*
 * weighted frequency count comparison function
 */
float
calc_wfc(int32_t t,int32_t p)
{
    if (t==1) return 1;
    if (t > 1 && t <= 64) return (1.0f/((float)t*(float)p));
    if (t > 64 && t <= 256) return (1.0f/(2.0f*((float)t*(float)p)));
    if (t > 256 && t <= 1024) return (1.0f/(4.0f*((float)t*(float)p)));
    if (t > 1024 && t <= 2048) return (1.0f/(8.0f*((float)t*(float)p)));
    return 0;
}

/*
 * weighted frequency count comparison function
 */
int
wfc_cmp(const void* t1,const void* t2)
{
    lnode_t* n1 = (lnode_t*)t1;
    lnode_t* n2 = (lnode_t*)t2;

    if (n1->wfreq < n2->wfreq)
        return -1;
    else if (n1->wfreq > n2->wfreq)
        return 1;
    else
        return 0;
}

uint8_t*
lupdate_wfc(uint8_t* in,uint32_t size,uint8_t* output)
{
    uint32_t j,i;
    list_t* lst;
    lnode_t* found,*tmp;
    int32_t k,start,c;

    lst = lupdate_createlist();

    int32_t ca = 0;
    for (i=0; i<size; i++) {
        j = in[i];
        ca += (int32_t) j;

        found = list_get(lst,j);
        output[i] = (uint8_t) found->data;

        /* reset wfreq values */
        tmp = lst->head;
        while (tmp != NULL) {
            tmp->wfreq = 0;
            tmp = tmp->next;
        }

        /* calculate new wfreq values */
        start = MAX((int32_t)i-2048,0);
        for (k=start; k<(int32_t)i; k++) {
            found = list_find(lst,output[k],&c);
            found->wfreq = found->wfreq + calc_wfc(i-k,4);
        }

        /* sort list based on wfreq values */
        list_sort(lst,wfc_cmp);
    }

    fprintf(stderr,"COST = %d\n",ca);

    list_free(lst);

    return output;

}

uint8_t*
lupdate_timestamp(uint8_t* in,uint32_t size,uint8_t* output)
{
    uint32_t j;
    int i;
    int ts;
    lnode_t* found;
    lnode_t* tmp;
    list_t* lst;

    lst = lupdate_createlist();

    ts = 0;
    for (j=0; j<size; j++) {
        i = in[j];

        found = list_get(lst,i);
        output[j] = (uint8_t) found->data;

        if (found->ts1 != -1) {
            tmp = lst->head;
            while (tmp != NULL && tmp != found) {
                if (tmp->ts1 < found->ts1 ||
                    (tmp->ts1 > found->ts1 && found->ts1 > tmp->ts2)) {
                    /* move in front of ts */
                    tmp = tmp->prev;
                    if (tmp == NULL) {
                        /* move to the front of the list */
                        list_movetofront(lst,found);
                    } else {
                        list_moveafter(found,tmp);
                    }
                    break;
                }
                tmp = tmp->next;
            }
        }

        /* update ts */
        found->ts2 = found->ts1;
        found->ts1 = ts;

        ts++;
    }
    return output;
}

