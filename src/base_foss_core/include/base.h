#ifndef BASE_H
#define BASE_H

#ifdef __cplusplus
#define BASE_BEGIN_EXTERN_C       extern "C" {
#define BASE_END_EXTERN_C         }
#else
#define BASE_BEGIN_EXTERN_C
#define BASE_END_EXTERN_C
#endif

#ifndef WIN32
#include <switch_am_config.h>
#endif

#ifndef MACOSX
#if !defined(_XOPEN_SOURCE) && !defined(__OpenBSD__) && !defined(__NetBSD__)
#ifndef __cplusplus
#define _XOPEN_SOURCE 700
#endif
#endif
#ifdef __linux__
#ifndef _BSD_SOURCE
#define _BSD_SOURCE
#endif
#ifndef _DEFAULT_SOURCE
#define _DEFAULT_SOURCE
#endif
#endif
#ifndef __BSD_VISIBLE
#define __BSD_VISIBLE 1
#endif
#ifdef __linux__
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#endif
#endif
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <assert.h>
#include <setjmp.h>
#include <ctype.h>
#include <fcntl.h>
#include <string.h>
#ifdef _MSC_VER
#include <Winsock2.h>
#if _MSC_VER < 1500
/* work around bug in msvc 2005 code analysis http://connect.microsoft.com/VisualStudio/feedback/ViewFeedback.aspx?FeedbackID=99397 */
#pragma warning(push)
#pragma warning(disable:6011)
#include <Ws2tcpip.h>
#pragma warning(pop)
#else
/* work around for warnings in vs 2010 */
#pragma warning (disable:6386)
#include <Ws2tcpip.h>
#pragma warning (default:6386)
#endif
#else
#include <strings.h>
#endif
#include <sys/stat.h>
#include <limits.h>
#include <sys/types.h>
#include <time.h>
#include <signal.h>
#include <errno.h>

#include "base_platform.h"
#include "base_types.h"
#include "base_core.h"
#include "base_core_db.h"
#include "base_scheduler.h"
#include "base_apr.h"
#include "base_module_interfaces.h"

#include "base_log.h"
#include "base_xml.h"


#endif
