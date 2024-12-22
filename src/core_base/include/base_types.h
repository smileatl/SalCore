#ifndef BASE_TYPES_H
#define BASE_TYPES_H

#include "base.h"
#include <stdint.h>

BASE_BEGIN_EXTERN_C

typedef enum {
    BASE_STATUS_SUCCESS,
    BASE_STATUS_FALSE,
    BASE_STATUS_TIMEOUT,
    BASE_STATUS_RESTART,
    BASE_STATUS_INTR,
    BASE_STATUS_NOTIMPL,
    BASE_STATUS_MEMERR,
    BASE_STATUS_NOOP,
    BASE_STATUS_RESAMPLE,
    BASE_STATUS_GENERR,
    BASE_STATUS_INUSE,
    BASE_STATUS_BREAK,
    BASE_STATUS_SOCKERR,
    BASE_STATUS_MORE_DATA,
    BASE_STATUS_NOTFOUND,
    BASE_STATUS_UNLOAD,
    BASE_STATUS_NOUNLOAD,
    BASE_STATUS_IGNORE,
    BASE_STATUS_TOO_SMALL,
    BASE_STATUS_FOUND,
    BASE_STATUS_CONTINUE,
    BASE_STATUS_TERM,
    BASE_STATUS_NOT_INITALIZED,
    BASE_STATUS_INVAL,                /* 参数错误 */
    BASE_STATUS_EXISTS,               /* 已存在 */
    BASE_STATUS_CANCELED,
    BASE_STATUS_XBREAK = 35,
    BASE_STATUS_WINBREAK = 730035
} base_status_t;

typedef int32_t         base_int32_t;
typedef uint32_t        base_uint32_t;
typedef size_t          base_size_t;

typedef unsigned char   base_byte_t;

BASE_END_EXTERN_C


#endif // BASE_TYPES_H