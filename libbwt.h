/*
 * File:   libbwt.h
 * Author: matt
 *
 * Created on 1 December 2010, 10:37 PM
 */

#ifndef LIBBWT_H
#define	LIBBWT_H

#ifdef	__cplusplus
extern "C" {
#endif

    uint8_t* reverse_bwt(uint8_t* input,int32_t n,int32_t I,uint8_t* out);

#ifdef	__cplusplus
}
#endif

#endif	/* LIBBWT_H */

