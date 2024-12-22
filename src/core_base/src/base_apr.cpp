#include "base.h"
#include <apr.h>
#include <apr_general.h>
#include <apr_pools.h>
#include <apr_errno.h>
#include <apr_file_info.h>

BASE_BEGIN_EXTERN_C

static base_status_t convert_apr_status(apr_status_t apr_status)
{
    switch (apr_status) {
        case APR_SUCCESS:
            return BASE_STATUS_SUCCESS;
        // 根据需要添加更多的映射
        default:
            return BASE_STATUS_GENERR; // 默认情况

    }
}

BASELIB_API void base_apr_initialize(void)
{
    apr_initialize();
}

BASELIB_API base_status_t base_pool_create(base_pool_t** newpool, base_pool_t* parent)
{
    apr_status_t apr_status =  apr_pool_create(newpool, parent);
    return convert_apr_status(apr_status);
}

BASELIB_API base_status_t base_stat(base_finfo_t *finfo, const char *fname, base_int32_t wanted, base_pool_t *pool)
{
    apr_status_t apr_status = apr_stat(finfo, fname, wanted, pool);
    return convert_apr_status(apr_status);
}

BASELIB_API void base_pool_destroy(base_pool_t* pool)
{
    apr_pool_destroy(pool);
}


BASE_END_EXTERN_C

