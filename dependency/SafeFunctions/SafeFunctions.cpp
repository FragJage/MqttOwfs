//Librement inspiré de Safe C Library (http://sourceforge.net/projects/safeclib)
#include "SafeFunctions.h"

errno_t strcpy_s (char *dest, size_t dmax, const char *src)
{
    if (dest == nullptr) return EINVAL;
    *dest = '\0';
    if (src == nullptr) return EINVAL;
    if (dmax == 0) return ERANGE;
    if (dmax > 65535) return ERANGE;
    if (dest == src) return EOK;
    if ((dest < src)&&(dest+dmax >= src)) return ERANGE;
    if ((src < dest)&&(src+dmax >= dest)) return ERANGE;

    while (dmax > 0)
    {
        *dest = *src;
        if (*dest == '\0')  return EOK;

        dmax--;
        dest++;
        src++;
    }

    return ERANGE;
}

errno_t memcpy_s (void *dest, size_t dmax, const void *src, size_t smax)
{
    uint8_t *dp;
    const uint8_t *sp;
    size_t imax;


    if (dest == nullptr) return EINVAL;
    if (dmax == 0)  return ERANGE;
    if (dmax > 65535) return ERANGE;

    dp = (uint8_t *)dest;
    for (imax = dmax; imax; dp++, imax--) *dp = 0;

    if (smax == 0)  return EINVAL;
    if (smax > dmax) return ERANGE;
    if (src == nullptr) return EINVAL;

    dp = (uint8_t *)dest; sp = (uint8_t *)src;

    if((dp > sp) && (dp < (sp+smax))) return ERANGE;
    if((sp > dp) && (sp < (dp+dmax))) return ERANGE;

    for (imax = smax; imax; dp++, sp++, imax--) *dp = *sp;

    return EOK;
}
