/********************************************************************
 ** Emulation functions for function calls automatically generated by
 ** llvm isel expand.
 ********************************************************************/

#include <stdint.h>

typedef uint32_t uint32;
typedef int32_t   int32;

/******************************************************************
 **
 ** Emulation functions from \newlib\libc\machine\h8500\divsi3.c
 **
 *****************************************************************/

#define divnorm(num, den, sign)                 \
{                                               \
  if (num < 0)                                  \
    {                                           \
      num = -num;                               \
      sign = 1;                                 \
    }                                           \
  else                                          \
    {                                           \
      sign = 0;                                 \
    }                                           \
                                                \
  if (den < 0)                                  \
    {                                           \
      den = - den;                              \
      sign = 1 - sign;                          \
    }                                           \
}

uint32
divmodsi4(int32 modwanted, uint32 num, uint32 den)
{
    int32 bit = 1;
    int32 res = 0;
    int32 prevden;
    while (den < num && bit && !(den & (1L<<31)))
    {
        den <<=1;
        bit <<=1;
    }
    while (bit)
    {
        if (num >= den)
        {
            num -= den;
            res |= bit;
        }
        bit >>=1;
        den >>=1;
    }
    if (modwanted) return num;
    return res;
}


#define exitdiv(sign, res) if (sign) { res = - res;} return res;

int32
__modsi3 (int32 numerator, int32 denominator)
{
    int32 sign = 0;
    int32 dividend;
    int32 modul;
    

    if (numerator < 0)
    {
        numerator = -numerator;
        sign = 1;
    }
    if (denominator < 0)
    {
        denominator = -denominator;
    }
    
    modul =  divmodsi4 (1, numerator, denominator);
    if (sign)
        return - modul;
    return modul;
}


int32
__divsi3 (int32 numerator, int32 denominator)
{
    int32 sign;
    int32 dividend;
    int32 modul;
    divnorm (numerator, denominator, sign);
    
    dividend = divmodsi4 (0,  numerator, denominator);
    exitdiv (sign, dividend);
}

int32
__umodsi3 (uint32 numerator, uint32 denominator)
{
    int32 dividend;
    int32 modul;
    modul= divmodsi4 (1,  numerator, denominator);
    return modul;
}

int32
__udivsi3 (uint32 numerator, uint32 denominator)
{
    int32 sign;
    int32 dividend;
    int32 modul;
    dividend =   divmodsi4 (0, numerator, denominator);
    return dividend;
}

int32 
__mulsi3(uint32 a, uint32 b) {
    int32 res = 0;
    while (a) {
        if (a & 1) {
            res += b;
        }
        b <<= 1;
        a >>=1;
    }
    return res;
}

#if 0
/////////////////////////////////////////////////
//
// add memcpy, memset and memmove functions back 
// if they were optimized out by accident
//
/////////////////////////////////////////

/* Part of the ht://Dig package   <http://www.htdig.org/> */
/* Copyright (c) 1999-2004 The ht://Dig Group */
/* For copyright details, see the file COPYING in your distribution */
/* or the GNU Library General Public License (LGPL) version 2 or later */
/* <http://www.gnu.org/copyleft/lgpl.html> */

/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 1996, 1997, 1998, 1999
 *	Sleepycat Software.  All rights reserved.
 */
/*
 * Copyright (c) 1990, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <string.h>

/*
 * sizeof(word) MUST BE A POWER OF TWO
 * SO THAT wmask BELOW IS ALL ONES
 */
typedef	int word;		/* "word" used for optimal copy speed */

#undef	wsize
#define	wsize	sizeof(word)
#undef	wmask
#define	wmask	(wsize - 1)

/*
 * Copy a block of memory, handling overlap.
 * This is the routine that actually implements
 * (the portable versions of) bcopy, memcpy, and memmove.
 */

/*
 * PUBLIC: #ifndef HAVE_MEMCPY
 * PUBLIC: void *memcpy __P((void *, const void *, size_t));
 * PUBLIC: #endif
 */
void *
memcpy(dst0, src0, length)
	void *dst0;
	const void *src0;
	register size_t length;
{
	register char *dst = dst0;
	register const char *src = src0;
	register size_t t;

	if (length == 0 || dst == src)		/* nothing to do */
        return (dst0);

	/*
	 * Macros: loop-t-times; and loop-t-times, t>0
	 */
#undef	TLOOP
#define	TLOOP(s) if (t) TLOOP1(s)
#undef	TLOOP1
#define	TLOOP1(s) do { s; } while (--t)

	if ((unsigned long)dst < (unsigned long)src) {
		/*
		 * Copy forward.
		 */
		t = (int)src;	/* only need low bits */
		if ((t | (int)dst) & wmask) {
			/*
			 * Try to align operands.  This cannot be done
			 * unless the low bits match.
			 */
			if ((t ^ (int)dst) & wmask || length < wsize)
				t = length;
			else
				t = wsize - (t & wmask);
			length -= t;
			TLOOP1(*dst++ = *src++);
		}
		/*
		 * Copy whole words, then mop up any trailing bytes.
		 */
		t = length / wsize;
		TLOOP(*(word *)dst = *(word *)src; src += wsize; dst += wsize);
		t = length & wmask;
		TLOOP(*dst++ = *src++);
	} else {
		/*
		 * Copy backwards.  Otherwise essentially the same.
		 * Alignment works as before, except that it takes
		 * (t&wmask) bytes to align, not wsize-(t&wmask).
		 */
		src += length;
		dst += length;
		t = (int)src;
		if ((t | (int)dst) & wmask) {
			if ((t ^ (int)dst) & wmask || length <= wsize)
				t = length;
			else
				t &= wmask;
			length -= t;
			TLOOP1(*--dst = *--src);
		}
		t = length / wsize;
		TLOOP(src -= wsize; dst -= wsize; *(word *)dst = *(word *)src);
		t = length & wmask;
		TLOOP(*--dst = *--src);
	}

	return (dst0);
}

/*
 * sizeof(word) MUST BE A POWER OF TWO
 * SO THAT wmask BELOW IS ALL ONES
 */

/*
 * Copy a block of memory, handling overlap.
 * This is the routine that actually implements
 * (the portable versions of) bcopy, memcpy, and memmove.
 */
/*
 * PUBLIC: #ifndef HAVE_MEMMOVE
 * PUBLIC: void *memmove __P((void *, const void *, size_t));
 * PUBLIC: #endif
 */
void *
memmove(dst0, src0, length)
	void *dst0;
	const void *src0;
	register size_t length;
{
	register char *dst = dst0;
	register const char *src = src0;
	register size_t t;

	if (length == 0 || dst == src)		/* nothing to do */
        return (dst0);

	/*
	 * Macros: loop-t-times; and loop-t-times, t>0
	 */
#undef	TLOOP
#define	TLOOP(s) if (t) TLOOP1(s)
#undef	TLOOP1
#define	TLOOP1(s) do { s; } while (--t)

	if ((unsigned long)dst < (unsigned long)src) {
		/*
		 * Copy forward.
		 */
		t = (int)src;	/* only need low bits */
		if ((t | (int)dst) & wmask) {
			/*
			 * Try to align operands.  This cannot be done
			 * unless the low bits match.
			 */
			if ((t ^ (int)dst) & wmask || length < wsize)
				t = length;
			else
				t = wsize - (t & wmask);
			length -= t;
			TLOOP1(*dst++ = *src++);
		}
		/*
		 * Copy whole words, then mop up any trailing bytes.
		 */
		t = length / wsize;
		TLOOP(*(word *)dst = *(word *)src; src += wsize; dst += wsize);
		t = length & wmask;
		TLOOP(*dst++ = *src++);
	} else {
		/*
		 * Copy backwards.  Otherwise essentially the same.
		 * Alignment works as before, except that it takes
		 * (t&wmask) bytes to align, not wsize-(t&wmask).
		 */
		src += length;
		dst += length;
		t = (int)src;
		if ((t | (int)dst) & wmask) {
			if ((t ^ (int)dst) & wmask || length <= wsize)
				t = length;
			else
				t &= wmask;
			length -= t;
			TLOOP1(*--dst = *--src);
		}
		t = length / wsize;
		TLOOP(src -= wsize; dst -= wsize; *(word *)dst = *(word *)src);
		t = length & wmask;
		TLOOP(*--dst = *--src);
	}

	return (dst0);
}

/** 
 * This is from newlib...
 */

#define LBLOCKSIZE (sizeof(long))
#define UNALIGNED(X)   ((long)X & (LBLOCKSIZE - 1))
#define TOO_SMALL(LEN) ((LEN) < LBLOCKSIZE)

void *
memset(m, c, n)
	void *m;
	int c;
	size_t n;
{
    char *s = (char *) m;
    
    int i;
    unsigned long buffer;
    unsigned long *aligned_addr;
    unsigned int d = c & 0xff;	/* To avoid sign extension, copy C to an
                                   unsigned variable.  */

    while (UNALIGNED (s))
    {
        if (n--)
            *s++ = (char) c;
        else
            return m;
    }
    
    if (!TOO_SMALL (n))
    {
        /* If we get this far, we know that n is large and s is word-aligned. */
        aligned_addr = (unsigned long *) s;
        
        /* Store D into each char sized location in BUFFER so that
           we can set large blocks quickly.  */
        buffer = (d << 8) | d;
        buffer |= (buffer << 16);
        for (i = 32; i < LBLOCKSIZE * 8; i <<= 1)
            buffer = (buffer << i) | buffer;
        
        /* Unroll the loop.  */
        while (n >= LBLOCKSIZE*4)
        {
            *aligned_addr++ = buffer;
            *aligned_addr++ = buffer;
            *aligned_addr++ = buffer;
            *aligned_addr++ = buffer;
            n -= 4*LBLOCKSIZE;
        }
        
        while (n >= LBLOCKSIZE)
        {
            *aligned_addr++ = buffer;
            n -= LBLOCKSIZE;
        }
        /* Pick up the remainder with a bytewise loop.  */
        s = (char*)aligned_addr;
    }
    while (n--)
        *s++ = (char) c;
    
    return m;
}
#endif
/**************************************************************************
 **
 ** Disabled 64bit support functions.
 **
 *****************************************************************************/
#if 0

#include <stdint.h>

struct HiLo64 {        
    uint32_t hi;
    uint32_t lo;
};

typedef union {
    struct HiLo64 s;
    uint64_t val;
} HiLoUnion64;

/**
 * Function for preventing gcc to generate 64bit 
 * adds and subs to code..
 */
void _emulate_i64_shl_i64_i64_gcc_hack(
    uint32_t a_hi, uint32_t a_lo,
    uint32_t shift_count,
    uint32_t *r_hi, uint32_t *r_lo) {
    
    uint32_t temp;

    /*
      | h1 h2 h3 h4 | l1 l2 l3 l4 | << 1
      
      hi   = | h2 h3 h4  0 |
      lo   = | l2 l3 l4  0 |
      temp = |  0  0  0 l1 |
      
      ret.hi = hi|temp 
      ret.lo = lo
    */
    
    temp = a_lo >> (32 - shift_count);       
    *r_hi = a_hi << shift_count;
    *r_lo = a_lo << shift_count;
    *r_hi = *r_hi|temp;
}
 
int64_t _emulate_i64_shl_i64_i64(int64_t p1, int64_t p2) {
    HiLoUnion64 u1 = {.val = p1};
    HiLoUnion64 u2 = {.val = p2}; 
    HiLoUnion64 ret;       
    
    _emulate_i64_shl_i64_i64_gcc_hack(u1.s.hi, u1.s.lo,
                                      u2.s.lo,
                                      &ret.s.hi, &ret.s.lo);

    return ret.val;
}

void _emulate_i64_ashr_i64_i64_gcc_hack(
    uint32_t a_hi, uint32_t a_lo,
    uint32_t shift_count,
    uint32_t *r_hi, uint32_t *r_lo) {
    
    uint32_t temp;

    /*
      | h1 h2 h3 h4 | l1 l2 l3 l4 | << 1
      
      hi   = ashr -> |  0 h1 h2 h3 |
      lo   = lshr -> |  0 l1 l2 l3 |
      temp =         | h1  0  0  0 |
      
      ret.hi = hi|temp 
      ret.lo = lo
    */
    
    temp = a_lo << (32 - shift_count);       
    *r_hi =  ((int32_t)a_hi) >> shift_count;
    *r_lo = ((uint32_t)a_lo) >> shift_count;
    *r_hi = *r_hi|temp;
}
 
int64_t _emulate_i64_ashr_i64_i64(int64_t p1, int64_t p2) {
    HiLoUnion64 u1 = {.val = p1};
    HiLoUnion64 u2 = {.val = p2}; 
    HiLoUnion64 ret;       
    
    _emulate_i64_ashr_i64_i64_gcc_hack(u1.s.hi, u1.s.lo,
                                       u2.s.lo,
                                       &ret.s.hi, &ret.s.lo);

    return ret.val;
}

void _emulate_i64_lshr_i64_i64_gcc_hack(
    uint32_t a_hi, uint32_t a_lo,
    uint32_t shift_count,
    uint32_t *r_hi, uint32_t *r_lo) {
    
    uint32_t temp;

    /*
      | h1 h2 h3 h4 | l1 l2 l3 l4 | << 1
      
      hi   = ashr -> |  0 h1 h2 h3 |
      lo   = lshr -> |  0 l1 l2 l3 |
      temp =         | h1  0  0  0 |
      
      ret.hi = hi|temp 
      ret.lo = lo
    */
    
    temp = a_lo << (32 - shift_count);       
//  *r_hi = ((uint32_t)a_hi) >> shift_count;
    *r_lo = ((uint32_t)a_lo) >> shift_count;
    *r_hi = *r_hi|temp;
}
 
uint64_t _emulate_i64_lshr_i64_i64(uint64_t p1, int64_t p2) {
  HiLoUnion64 u1 = {.val = p1}; 
  HiLoUnion64 u2 = {.val = p2}; 
  HiLoUnion64 ret;        
    
  _emulate_i64_ashr_i64_i64_gcc_hack(u1.s.hi, u1.s.lo, 
                                     u2.s.lo, 
                                     &ret.s.hi, &ret.s.lo); 

  return ret.val; 
}

/**
 * Function for preventing gcc to generate 64bit 
 * adds and subs to code..
 */
void _emulate_i64_add_i64_i64_gcc_hack(
    uint32_t a_hi, uint32_t a_lo,
    uint32_t b_hi, uint32_t b_lo,
    uint32_t *r_hi, uint32_t *r_lo) {

    *r_hi = a_hi + b_hi;
    *r_lo = a_lo + b_lo;

    if ((*r_lo < a_lo) ||
        (*r_lo < b_lo)) *r_hi = *r_hi + 1;

}
 
int64_t _emulate_i64_add_i64_i64(int64_t p1, int64_t p2) {
    HiLoUnion64 u1 = {.val = p1};
    HiLoUnion64 u2 = {.val = p2};
    HiLoUnion64 ret;       
    
    _emulate_i64_add_i64_i64_gcc_hack(u1.s.hi, u1.s.lo,
                                      u2.s.hi, u2.s.lo,
                                      &ret.s.hi, &ret.s.lo);

    return ret.val;
}

/**
 * Function for preventing gcc to generate 64 adds and subs to code..
 */
void _emulate_i64_sub_i64_i64_gcc_hack(
    uint32_t a_hi, uint32_t a_lo,
    uint32_t b_hi, uint32_t b_lo,
    uint32_t *r_hi, uint32_t *r_lo) {

    *r_hi = a_hi - b_hi;
    *r_lo = a_lo - b_lo;
    if (a_lo < b_lo) *r_hi = *r_hi - 1;    
}
 
int64_t _emulate_i64_sub_i64_i64(int64_t p1, int64_t p2) {
    HiLoUnion64 u1 = {.val = p1};
    HiLoUnion64 u2 = {.val = p2};
    HiLoUnion64 ret;       
    
    _emulate_i64_sub_i64_i64_gcc_hack(u1.s.hi, u1.s.lo,
                                      u2.s.hi, u2.s.lo,
                                      &ret.s.hi, &ret.s.lo);

    return ret.val;
}

int64_t _emulate_i64_mul_i64_i64(int64_t p1, int64_t p2) {    
    HiLoUnion64 u1 = {.val = p1};
    HiLoUnion64 u2 = {.val = p2};
    HiLoUnion64 ret;    

    /* Algorithm... used for getting hi part of 32*32 = 64bit multiply
       and for 64*64 = 64bit result.

                      a1  a2
                      b1  b2
                   ----------
                    (    a2b2)
              (     |a1b2)
              (     |a2b1)
       +  (    a1b1)|
       -------------|------------

       Note: a2b1 + a1b2 + (a2b2>>16) can be more than 32 bits!

    */
    
    uint32_t a2b2 = (u1.s.lo&0xffff) * (u2.s.lo&0xffff);
    uint32_t a1b2 = (u1.s.lo>>16)    * (u2.s.lo&0xffff);
    uint32_t a2b1 = (u1.s.lo&0xffff) * (u2.s.lo>>16);
    uint32_t a1b1 = (u1.s.lo>>16)    * (u2.s.lo>>16);
    
    uint32_t mul32hipart =
        ((((uint64_t)((a2b2>>16) + a1b2)) + a2b1)>>16) + a1b1;

    ret.s.hi = 
        u1.s.hi*u2.s.lo + 
        u1.s.lo*u2.s.hi +
        mul32hipart;
    
    ret.s.lo = u1.s.lo*u2.s.lo;

    /* Verify code 
       printf("p1 = %llx p2 = %llx hipart: %lx correct: %lx\n", 
       p1, p2, mul32hipart, 
       (uint32_t)(((p1&0xffffffff)*(p2&0xffffffff))>>32));
       
       
       int64_t sum = p1 * p2;    
       printf("p1 = %llx p2 = %llx result: %llx correct: %llx\n", 
       p1, p2, (int64_t)ret.val, sum);
       
       assert(((int64_t)ret.val) == sum);
    */

    return ret.val;
}

#endif

