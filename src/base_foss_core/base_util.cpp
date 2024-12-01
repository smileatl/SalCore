#include "base_util.h"

BASE_DECLARE(bool) base_strlen_zero(const char* str) {
    return str == nullptr || str[0] == '\0';
}

