/* Copyright (c) 2017  SiFive Inc. All rights reserved.
   Copyright (c) 2025 Mahmoud Abumandour <ma.mandourr@gmail.com>

   This copyrighted material is made available to anyone wishing to use,
   modify, copy, or redistribute it subject to the terms and conditions
   of the FreeBSD License.   This program is distributed in the hope that
   it will be useful, but WITHOUT ANY WARRANTY expressed or implied,
   including the implied warranties of MERCHANTABILITY or FITNESS FOR
   A PARTICULAR PURPOSE.  A copy of this license is available at
   http://www.opensource.org/licenses.
*/

#if defined(PREFER_SIZE_OVER_SPEED) || defined(__OPTIMIZE_SIZE__)
// memcpy defined in memcpy-asm.S
#else

#include "../../string/local.h"
#include "xlenint.h"
#include <string.h>
#include <sys/asm.h>

#define unlikely(X) __builtin_expect (!!(X), 0)

static inline void
__libc_memcpy_bytewise (unsigned char *dst, const unsigned char *src,
                        const size_t sz)
{
  const unsigned char *end = dst + sz;
  while (dst < end)
    *dst++ = *src++;
}

#ifndef __riscv_misaligned_fast
static uintxlen_t
__libc_load_xlen (const void *src)
{
  const unsigned char *p = (const unsigned char *)src;
  uintxlen_t ret = 0;
  unsigned char b0 = *p++;
  unsigned char b1 = *p++;
  unsigned char b2 = *p++;
  unsigned char b3 = *p++;
  ret = (uintxlen_t)b0 | ((uintxlen_t)b1 << 8) | ((uintxlen_t)b2 << 16)
        | ((uintxlen_t)b3 << 24);
#if __riscv_xlen == 64 || __riscv_xlen == 128
  unsigned char b4 = *p++;
  unsigned char b5 = *p++;
  unsigned char b6 = *p++;
  unsigned char b7 = *p++;
  ret |= ((uintxlen_t)b4 << 32) | ((uintxlen_t)b5 << 40)
         | ((uintxlen_t)b6 << 48) | ((uintxlen_t)b7 << 56);
#endif
#if __riscv_xlen == 128
  unsigned char b8 = *p++;
  unsigned char b9 = *p++;
  unsigned char b10 = *p++;
  unsigned char b11 = *p++;
  unsigned char b12 = *p++;
  unsigned char b13 = *p++;
  unsigned char b14 = *p++;
  unsigned char b15 = *p++;
  ret |= ((uintxlen_t)b8 << 64) | ((uintxlen_t)b9 << 72)
         | ((uintxlen_t)b10 << 80) | ((uintxlen_t)b11 << 88)
	 | ((uintxlen_t)b12 << 96) | ((uintxlen_t)b13 << 104)
	 | ((uintxlen_t)b14 << 112) | ((uintxlen_t)b15 << 120);
#endif

  return ret;
}
#endif

void *
__inhibit_loop_to_libcall
memcpy (void *__restrict aa, const void *__restrict bb, size_t n)
{
  unsigned char *a = (unsigned char *)aa;
  const unsigned char *b = (const unsigned char *)bb;
  unsigned char *end = a + n;
  uintptr_t msk = SZREG - 1;
  if (n < SZREG)
    {
      if (__builtin_expect (a < end, 1))
        __libc_memcpy_bytewise (a, b, n);
      return aa;
    }

/*
 * If misaligned access is slow or prohibited, and the alignments of the source
 * and destination are different, we align the destination to do XLEN stores.
 * This uses only one aligned store for every four (or eight for XLEN == 64)
 * bytes of data.
 */
#ifndef __riscv_misaligned_fast
  if (unlikely ((((uintptr_t)a & msk) != ((uintptr_t)b & msk))))
    {
      size_t dst_pad = (uintptr_t)a & msk;
      dst_pad = (SZREG - dst_pad) & msk;
      __libc_memcpy_bytewise (a, b, dst_pad);
      a += dst_pad;
      b += dst_pad;

      uintxlen_t *la = (uintxlen_t *)a;
      const unsigned char *cb = (const unsigned char *)b;
      uintxlen_t *lend = (uintxlen_t *)((uintptr_t)end & ~msk);

      while (la < lend)
        {
          *la++ = __libc_load_xlen (cb);
          cb += SZREG;
        }
      a = (unsigned char *)la;
      b = (const unsigned char *)cb;
      if (unlikely (a < end))
        __libc_memcpy_bytewise (a, b, end - a);
      return aa;
    }
#endif

  if (unlikely (((uintptr_t)a & msk) != 0))
    {
      size_t pad = SZREG - ((uintptr_t)a & msk);
      __libc_memcpy_bytewise (a, b, pad);
      a += pad;
      b += pad;
    }

  uintxlen_t *la = (uintxlen_t *)a;
  const uintxlen_t *lb = (const uintxlen_t *)b;
  uintxlen_t *lend = (uintxlen_t *)((uintptr_t)end & ~msk);

  if (unlikely (lend - la > 8))
    {
      while (lend - la > 8)
        {
          uintxlen_t b0 = *lb++;
          uintxlen_t b1 = *lb++;
          uintxlen_t b2 = *lb++;
          uintxlen_t b3 = *lb++;
          uintxlen_t b4 = *lb++;
          uintxlen_t b5 = *lb++;
          uintxlen_t b6 = *lb++;
          uintxlen_t b7 = *lb++;
          uintxlen_t b8 = *lb++;
#if __riscv_xlen == 128
	  uintxlen_t b9 = *lb++;
          uintxlen_t b10 = *lb++;
          uintxlen_t b11 = *lb++;
          uintxlen_t b12 = *lb++;
          uintxlen_t b13 = *lb++;
          uintxlen_t b14 = *lb++;
          uintxlen_t b15 = *lb++;
          uintxlen_t b16 = *lb++;
#endif
          *la++ = b0;
          *la++ = b1;
          *la++ = b2;
          *la++ = b3;
          *la++ = b4;
          *la++ = b5;
          *la++ = b6;
          *la++ = b7;
          *la++ = b8;
#if __riscv_xlen == 128
	  *la++ = b9;
          *la++ = b10;
          *la++ = b11;
          *la++ = b12;
          *la++ = b13;
          *la++ = b14;
          *la++ = b15;
          *la++ = b16;
#endif
        }
    }

  a = (unsigned char *)la;
  b = (const unsigned char *)lb;
  if (unlikely (a < end))
    __libc_memcpy_bytewise (a, b, end - a);
  return aa;
}
#endif
