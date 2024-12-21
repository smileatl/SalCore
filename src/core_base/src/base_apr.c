#include "base.h"
#include <apr.h>
#include <apr_general.h>
#include <apr_file_io.h>
#include <apr_file_info.h>

BASE_BEGIN_EXTERN_C

BASELIB_API void base_apr_initialize(void)
{
    apr_initialize();
}

BASE_END_EXTERN_C

