/*	$Id: support.h,v 1.1 2000/03/01 04:51:53 riq Exp $	*/
/********************************************************************** 
 Freeciv - Copyright (C) 1996 - A Kjeldberg, L Gregersen, P Unold
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
***********************************************************************/
/* Sacado del freeciv con algunas modificaciones */

#ifndef TEG_SUPPORT_H
#define TEG_SUPPORT_H

/********************************************************************** 
  Replacements for functions which are not available on all platforms.
  Where the functions are available natively, these are just wrappers.
  See also mem.h, rand.h, and see support.c for more comments.
***********************************************************************/

#include <stdlib.h>		/* size_t */
#include <stdarg.h>

int mystrcasecmp(const char *str0, const char *str1);
int mystrncasecmp(const char *str0, const char *str1, size_t n);

char *mystrerror(int errnum);
void myusleep(unsigned long usec);

size_t mystrlcpy(char *dest, const char *src, size_t n);
size_t mystrlcat(char *dest, const char *src, size_t n);

/* convenience macros for use when dest is a char ARRAY: */
#define sz_strlcpy(dest,src) mystrlcpy((dest),(src),sizeof(dest))
#define sz_strlcat(dest,src) mystrlcat((dest),(src),sizeof(dest))

int my_snprintf(char *str, size_t n, const char *format, ...);

int my_vsnprintf(char *str, size_t n, const char *format, va_list ap );

#endif  /* TEG_SUPPORT_H */
