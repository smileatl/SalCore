/* 
* FreeBASE Modular Media Switching Software Library / Soft-Switch Application
* Copyright (C) 2005-2014, Anthony Minessale II <anthm@freebase.org>
*
* Version: MPL 1.1
*
* The contents of this file are subject to the Mozilla Public License Version
* 1.1 (the "License"); you may not use this file except in compliance with
* the License. You may obtain a copy of the License at
* http://www.mozilla.org/MPL/
*
* Software distributed under the License is distributed on an "AS IS" basis,
* WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
* for the specific language governing rights and limitations under the
* License.
*
* The Original Code is FreeBASE Modular Media Switching Software Library / Soft-Switch Application
*
* The Initial Developer of the Original Code is
* Anthony Minessale II <anthm@freebase.org>
* Portions created by the Initial Developer are Copyright (C)
* the Initial Developer. All Rights Reserved.
*
* Contributor(s):
* 
* Anthony Minessale II <anthm@freebase.org>
*
*
* base_log.h -- Logger
*
*/
/*! \file base_log.h
\brief Simple Log

Logging Routines
*/

#ifndef BASE_LOG_H
#define BASE_LOG_H

#include <base.h>

BASE_BEGIN_EXTERN_C
///\defgroup log Logger Routines
///\ingroup core1
///\{
/*! \brief Log Data
*/
typedef struct {
    /*! The complete log message */
    char *data;
    /*! The file where the message originated */
    char file[80];
    /*! The line number where the message originated */
    uint32_t line;
    /*! The function where the message originated */
    char func[80];
    /*! The log level of the message */
    base_log_level_t level;
    /*! The time when the log line was sent */
    base_time_t timestamp;
    /*! A pointer to where the actual content of the message starts (skipping past the preformatted portion) */
    char *content;
    char *userdata;
    /* To maintain abi, only add new elements to the end of this struct and do not delete any elements */
    base_text_channel_t channel;
    base_log_level_t slevel;
    int logout_type; /* Where is the log output */
} base_log_node_t;

typedef base_status_t (*base_log_function_t) (const base_log_node_t *node, base_log_level_t level);


/*! 
\brief Initilize the logging engine
\param pool the memory pool to use
\note to be called at application startup by the core
*/
BASE_DECLARE(base_status_t) base_log_init(_In_ base_memory_pool_t *pool, _In_ base_bool_t colorize);

/*! 
\brief Shut down the logging engine
\note to be called at application termination by the core
*/
BASE_DECLARE(base_status_t) base_log_shutdown(void);

#ifndef SWIG
/*! 
\brief Write log data to the logging engine
\param channel the log channel to write to
\param file the current file
\param func the current function
\param line the current line
\param userdata ununsed
\param level the current log level
\param fmt desired format
\param ... variable args
\note there are channel macros to supply the first 4 parameters (BASE_CHANNEL_LOG, BASE_CHANNEL_LOG_CLEAN, ...)
\see base_types.h
*/
BASE_DECLARE(void) base_log_printf(_In_ base_text_channel_t channel, _In_z_ const char *file,
    _In_z_ const char *func, _In_ int line,
    _In_opt_z_ const char *userdata, _In_ base_log_level_t level,
    _In_z_ _Printf_format_string_ const char *fmt, ...) PRINTF_FUNCTION(7, 8);
/*! 
\brief Write log data to the logging engine
\param channel the log channel to write to
\param file the current file
\param func the current function
\param line the current line
\param modname mod name
\param outstd printf to console
\param level the current log level
\param fmt desired format
\param ... variable args
\可以使用如 base_custom_log(BASE_CUSTOM_LOG("mod_meeting",1), BASE_LOG_NOTICE, "xxx"); 打印日志
\打印出的日志文件会存放在log/mod_meeting/mod_meeting.log，且不会打印在控制台。
\BASE_CUSTOM_LOG("mod_meeting",1)中的"mod_meeting"表示模块名，1表示不输出到控制台
*/
BASE_DECLARE(void) base_custom_log(_In_ base_text_channel_t channel, _In_z_ const char *file,
    _In_z_ const char *func, _In_ int line,
    _In_opt_z_ const char *filename, _In_opt_ int logout_type, _In_ base_log_level_t level,
    _In_z_ _Printf_format_string_ const char *fmt, ...) PRINTF_FUNCTION(8, 9);

#define CUSTOM_LOG_DEBUG(filename, ...) base_custom_log(BASE_CUSTOM_LOG(filename, -1), BASE_LOG_DEBUG, __VA_ARGS__)
#define CUSTOM_LOG_INFO(filename, ...) base_custom_log(BASE_CUSTOM_LOG(filename, -1), BASE_LOG_INFO, __VA_ARGS__)
#define CUSTOM_LOG_NOTICE(filename, ...) base_custom_log(BASE_CUSTOM_LOG(filename, -1), BASE_LOG_NOTICE, __VA_ARGS__)
#define CUSTOM_LOG_WARNING(filename, ...) base_custom_log(BASE_CUSTOM_LOG(filename, -1), BASE_LOG_WARNING, __VA_ARGS__)
#define CUSTOM_LOG_ERROR(filename, ...) base_custom_log(BASE_CUSTOM_LOG(filename, -1), BASE_LOG_ERROR, __VA_ARGS__)
#define CUSTOM_LOG_CRIT(filename, ...) base_custom_log(BASE_CUSTOM_LOG(filename, -1), BASE_LOG_CRIT, __VA_ARGS__)
#define CUSTOM_LOG_ALERT(filename, ...) base_custom_log(BASE_CUSTOM_LOG(filename, -1), BASE_LOG_ALERT, __VA_ARGS__)
#define CUSTOM_LOG_CONSOLE(filename, ...) base_custom_log(BASE_CUSTOM_LOG(filename, -1), BASE_LOG_CONSOLE, __VA_ARGS__)
#define CUSTOM_LOG_NOSTD_DEBUG(filename, ...) base_custom_log(BASE_CUSTOM_LOG(filename, BASE_LOG_OUT_TYPE_FILE), BASE_LOG_DEBUG, __VA_ARGS__)
#define CUSTOM_LOG_NOSTD_INFO(filename, ...) base_custom_log(BASE_CUSTOM_LOG(filename, BASE_LOG_OUT_TYPE_FILE), BASE_LOG_INFO, __VA_ARGS__)
#define CUSTOM_LOG_NOSTD_NOTICE(filename, ...) base_custom_log(BASE_CUSTOM_LOG(filename, BASE_LOG_OUT_TYPE_FILE), BASE_LOG_NOTICE, __VA_ARGS__)
#define CUSTOM_LOG_NOSTD_WARNING(filename, ...) base_custom_log(BASE_CUSTOM_LOG(filename, BASE_LOG_OUT_TYPE_FILE), BASE_LOG_WARNING, __VA_ARGS__)
#define CUSTOM_LOG_NOSTD_ERROR(filename, ...) base_custom_log(BASE_CUSTOM_LOG(filename, BASE_LOG_OUT_TYPE_FILE), BASE_LOG_ERROR, __VA_ARGS__)

/*!								
\brief Write log data to the logging engine
\param channel the log channel to write to
\param file the current file
\param func the current function
\param line the current line
\param userdata ununsed
\param level the current log level
\param fmt desired format
\param ap variable args
\note there are channel macros to supply the first 4 parameters (BASE_CHANNEL_LOG, BASE_CHANNEL_LOG_CLEAN, ...)
\see base_types.h
*/
BASE_DECLARE(void) base_log_vprintf(_In_ base_text_channel_t channel, _In_z_ const char *file,
    _In_z_ const char *func, _In_ int line,
    _In_opt_z_ const char *userdata, _In_ base_log_level_t level, const char *fmt, va_list ap);

BASE_DECLARE(void) base_log_vprintf2(_In_ base_text_channel_t channel, _In_z_ const char *file,
    _In_z_ const char *func, _In_ int line,
    _In_opt_z_ const char *userdata, _In_opt_ int logout_type, _In_ base_log_level_t level, const char *fmt, va_list ap);

#endif
/*! 
\brief Shut down  the logging engine
\note to be called at application termination by the core
*/
BASE_DECLARE(base_status_t) base_log_bind_logger(_In_ base_log_function_t function, _In_ base_log_level_t level, _In_ base_bool_t is_console);
BASE_DECLARE(base_status_t) base_log_unbind_logger(_In_ base_log_function_t function);

BASE_DECLARE(base_log_level_t) base_log_level_get();
/*! 
\brief Return the name of the specified log level
\param level the level
\return the name of the log level
*/
_Ret_z_ BASE_DECLARE(const char *) base_log_level2str(_In_ base_log_level_t level);

/*! 
\brief Return the level number of the specified log level name
\param str the name of the level
\return the log level
*/
BASE_DECLARE(base_log_level_t) base_log_str2level(_In_z_ const char *str);

BASE_DECLARE(uint32_t) base_log_str2mask(_In_z_ const char *str);
#define base_log_check_mask(_mask, _level) (_mask & (1 << _level))


BASE_DECLARE(base_log_node_t *) base_log_node_dup(const base_log_node_t *node);
BASE_DECLARE(void) base_log_node_free(base_log_node_t **pnode);

///\}
BASE_END_EXTERN_C
#endif
/* For Emacs:
* Local Variables:
* mode:c
* indent-tabs-mode:t
* tab-width:4
* c-basic-offset:4
* End:
* For VIM:
* vim:set softtabstop=4 shiftwidth=4 tabstop=4 noet:
*/
