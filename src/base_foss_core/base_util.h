#pragma once

#include "base.h"

/*!
  \brief Test for NULL or zero length string
  \param s the string to test
  \return true value if the string is NULL or zero length
*/
//_Check_return_ static inline int _zstr(_In_opt_z_ const char *s)
//{
//	if (!s) return 1;
//	if (*s == '\0') return 1;
//	return 0;
//}
//#ifdef _PREFAST_
//#define zstr(x) (_zstr(x) ? 1 : __analysis_assume(x),0)
//#else
//#define zstr(x) _zstr(x)
//#endif
//#define base_strlen_zero(x) zstr(x)

BASE_DECLARE(bool) base_strlen_zero(const char* str);