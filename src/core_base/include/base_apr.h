#ifndef BASE_APR_H
#define BASE_APR_H

#include "base.h"

BASE_BEGIN_EXTERN_C

typedef struct apr_finfo_t base_finfo_t;
typedef struct apr_dir_t base_dir_t;
typedef struct apr_pool_t base_pool_t;

BASELIB_API void base_apr_initialize(void);
BASELIB_API base_status_t base_pool_create(base_pool_t** newpool, base_pool_t* parent);
BASELIB_API base_status_t base_stat(base_finfo_t *finfo, const char *fname, base_int32_t wanted, base_pool_t *pool);
BASELIB_API void base_pool_destroy(base_pool_t *pool);

BASE_END_EXTERN_C


#endif // BASE_APR_H