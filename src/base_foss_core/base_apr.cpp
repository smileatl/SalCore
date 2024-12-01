#include "base.h"
#include <apr.h>
#include <apr_general.h>
#include <apr_pools.h>
#include <apr_file_io.h>

BASE_DECLARE(base_status_t) base_apr_initialize(void)
{
    apr_status_t apr_status = apr_initialize();
    switch (apr_status) {
        case APR_SUCCESS:
            return BASE_STATUS_SUCCESS;
        // Add other mappings as needed
        default:
            return BASE_STATUS_GENERR; // Or a suitable default/base error code
    }
}