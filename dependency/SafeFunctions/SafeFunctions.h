//Librement inspiré de Safe C Library (http://sourceforge.net/projects/safeclib)
#ifndef SAFEFUNCTIONS_H
#define SAFEFUNCTIONS_H

#include<string.h>
#include<cstdint>       //For SIZE_MAX constante
#include<stddef.h>
#include<errno.h>       //For Error constante

#define EOK 0
typedef int errno_t;

errno_t strcpy_s (char *dest, size_t dmax, const char *src);
errno_t memcpy_s (void *dest, size_t dmax, const void *src, size_t smax);

#endif
