/* Copyright (c) 2017  SiFive Inc. All rights reserved.

   This copyrighted material is made available to anyone wishing to use,
   modify, copy, or redistribute it subject to the terms and conditions
   of the FreeBSD License.   This program is distributed in the hope that
   it will be useful, but WITHOUT ANY WARRANTY expressed or implied,
   including the implied warranties of MERCHANTABILITY or FITNESS FOR
   A PARTICULAR PURPOSE.  A copy of this license is available at
   http://www.opensource.org/licenses.
*/

#include <sys/types.h>
#include <string.h>
#include <stdint.h>
#include "rv_string.h"

size_t strlen(const char *str)
{
  const char *start = str;

#if defined(PREFER_SIZE_OVER_SPEED) || defined(__OPTIMIZE_SIZE__)
  while (*str++)
    ;
  return str - start - 1;
#else
  if (__builtin_expect ((uintxlen_t)str & (sizeof (uintxlen_t) - 1), 0)) do
    {
      char ch = *str;
      str++;
      if (!ch)
      return str - start - 1;
    } while ((uintxlen_t)str & (sizeof (uintxlen_t) - 1));

  uintxlen_t *ps = (uintxlen_t *)str;
  uintxlen_t psval;

  while (!__libc_detect_null ((psval = *ps++)))
    ;
  asm volatile ("" : "+r"(ps)); /* prevent "optimization" */

  str = (const char *)ps;

  size_t ret = str - start;
  ssize_t sp = sizeof (*ps);

  #if __riscv_zbb
    psval = ~__LIBC_RISCV_ZBB_ORC_B(psval);
    psval = __LIBC_RISCV_ZBB_CNT_Z(psval);

    return ret + (psval >> 3) - sp;
  #else
    char c0 = str[0 - sp], c1 = str[1 - sp], c2 = str[2 - sp], c3 = str[3 - sp];
    if (c0 == 0)                        return ret + 0 - sp;
    if (c1 == 0)                        return ret + 1 - sp;
    if (c2 == 0)                        return ret + 2 - sp;
    if (__riscv_xlen == 32 || c3 == 0)  return ret + 3 - sp;

    #if __riscv_xlen == 64 || __riscv_xlen == 128
      c0 = str[4 - sp], c1 = str[5 - sp], c2 = str[6 - sp], c3 = str[7 -sp];
      if (c0 == 0)                      return ret + 4 - sp;
      if (c1 == 0)                      return ret + 5 - sp;
      if (c2 == 0)                      return ret + 6 - sp;
      if (c3 == 0)                      return ret + 7 - sp;
    #endif
	
    #if __riscv_xlen == 128
      c0 = str[8 - sp], c1 = str[9 - sp], c2 = str[10 - sp], c3 = str[11 -sp];
      if (c0 == 0)                      return ret + 8 - sp;
      if (c1 == 0)                      return ret + 9 - sp;
      if (c2 == 0)                      return ret + 10 - sp;
      if (c3 == 0)                      return ret + 11 - sp;

      c0 = str[12 - sp], c1 = str[13 - sp], c2 = str[14 - sp], c3 = str[15 -sp];
      if (c0 == 0)                      return ret + 12 - sp;
      if (c1 == 0)                      return ret + 13 - sp;
      if (c2 == 0)                      return ret + 14 - sp;

    #endif
    return ret + 15 - sp;
  #endif
#endif /* not PREFER_SIZE_OVER_SPEED */
}
