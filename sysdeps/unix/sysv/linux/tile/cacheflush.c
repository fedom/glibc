/* Copyright (C) 2011-2015 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Chris Metcalf <cmetcalf@tilera.com>, 2011.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library.  If not, see
   <http://www.gnu.org/licenses/>.  */

#include <errno.h>
#include <stddef.h>
#include <unistd.h>

/* Flush cache(s). */
int
_flush_cache (char *addr, const int nbytes, const int op)
{
#ifdef __NR_cacheflush
  return INLINE_SYSCALL (cacheflush, 3, addr, nbytes, op);
#else
  __set_errno (ENOSYS);
  return -1;
#endif
}
weak_alias (_flush_cache, cacheflush)
