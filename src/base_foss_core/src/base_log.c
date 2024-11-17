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
* base_log.c -- Logging
*
*/

#include <base.h>

static const char *LEVELS[] = {
    "CONSOLE",
    "ALERT",
    "CRIT",
    "ERR",
    "WARNING",
    "NOTICE",
    "INFO",
    "DEBUG",
    NULL
};

struct base_log_binding {
    base_log_function_t function;
    base_log_level_t level;
    int is_console;
    struct base_log_binding *next;
};

typedef struct base_log_binding base_log_binding_t;

static base_memory_pool_t *LOG_POOL = NULL;
static base_log_binding_t *BINDINGS = NULL;
static base_mutex_t *BINDLOCK = NULL;
static base_queue_t *LOG_QUEUE = NULL;
#ifdef BASE_LOG_RECYCLE
static base_queue_t *LOG_RECYCLE_QUEUE = NULL;
#endif
static int8_t THREAD_RUNNING = 0;
static uint8_t MAX_LEVEL = 0;
static int mods_loaded = 0;
static int console_mods_loaded = 0;
static base_bool_t COLORIZE = BASE_FALSE;

#ifdef WIN32
static HANDLE hStdout;
static WORD wOldColorAttrs;
static CONSOLE_SCREEN_BUFFER_INFO csbiInfo;

static WORD
#else
static const char *
#endif




COLORS[] =
{ BASE_SEQ_DEFAULT_COLOR, BASE_SEQ_FRED, BASE_SEQ_FRED, BASE_SEQ_FRED, BASE_SEQ_FMAGEN, BASE_SEQ_FCYAN, BASE_SEQ_FGREEN,
BASE_SEQ_FYELLOW };


static base_log_node_t *base_log_node_alloc()
{
    base_log_node_t *node = NULL;
#ifdef BASE_LOG_RECYCLE
    void *pop = NULL;

    if (base_queue_trypop(LOG_RECYCLE_QUEUE, &pop) == BASE_STATUS_SUCCESS) {
        node = (base_log_node_t *) pop;
    } else {
#endif
        node = malloc(sizeof(*node));
        base_assert(node);
#ifdef BASE_LOG_RECYCLE
    }
#endif
    return node;
}

BASE_DECLARE(base_log_node_t *) base_log_node_dup(const base_log_node_t *node)
{
    base_log_node_t *newnode = base_log_node_alloc();

    *newnode = *node;

    if (!zstr(node->data)) {
        newnode->data = strdup(node->data);
        base_assert(node->data);
    }

    if (!zstr(node->userdata)) {
        newnode->userdata = strdup(node->userdata);
        base_assert(node->userdata);
    }

    return newnode;
}

BASE_DECLARE(void) base_log_node_free(base_log_node_t **pnode)
{
    base_log_node_t *node;

    if (!pnode) {
        return;
    }

    node = *pnode;

    if (node) {
        base_safe_free(node->userdata);
        base_safe_free(node->data);
#ifdef BASE_LOG_RECYCLE
        if (base_queue_trypush(LOG_RECYCLE_QUEUE, node) != BASE_STATUS_SUCCESS) {
            free(node);
        }
#else
        free(node);
#endif
    }
    *pnode = NULL;
}

BASE_DECLARE(base_log_level_t) base_log_level_get()
{
    return runtime.hard_log_level;
}

BASE_DECLARE(const char *) base_log_level2str(base_log_level_t level)
{
    if (level > BASE_LOG_DEBUG) {
        level = BASE_LOG_DEBUG;
    }
    return LEVELS[level];
}

BASE_DECLARE(uint32_t) base_log_str2mask(const char *str)
{
    int argc = 0, x = 0;
    char *argv[10] = { 0 };
    uint32_t mask = 0;
    char *p = strdup(str);
    base_log_level_t level;

    base_assert(p);

    if ((argc = base_separate_string(p, ',', argv, (sizeof(argv) / sizeof(argv[0]))))) {
        for (x = 0; x < argc && argv[x]; x++) {
            if (!strcasecmp(argv[x], "all")) {
                mask = 0xFF;
                break;
            } else {
                level = base_log_str2level(argv[x]);
                if (level != BASE_LOG_INVALID) {
                    mask |= (1 << level);
                }
            }
        }
    }

    free(p);

    return mask;
}

BASE_DECLARE(base_log_level_t) base_log_str2level(const char *str)
{
    int x = 0;
    base_log_level_t level = BASE_LOG_INVALID;

    if (base_is_number(str)) {
        x = atoi(str);

        if (x > BASE_LOG_INVALID) {
            return BASE_LOG_INVALID - 1;
        } else if (x < 0) {
            return 0;
        } else {
            return x;
        }
    }


    for (x = 0;; x++) {
        if (!LEVELS[x]) {
            break;
        }

        if (!strcasecmp(LEVELS[x], str)) {
            level = (base_log_level_t) x;
            break;
        }
    }

    return level;
}

BASE_DECLARE(base_status_t) base_log_unbind_logger(base_log_function_t function)
{
    base_log_binding_t *ptr = NULL, *last = NULL;
    base_status_t status = BASE_STATUS_FALSE;

    base_mutex_lock(BINDLOCK);
    for (ptr = BINDINGS; ptr; ptr = ptr->next) {
        if (ptr->function == function) {
            if (last) {
                last->next = ptr->next;
            } else {
                BINDINGS = ptr->next;
            }
            status = BASE_STATUS_SUCCESS;
            mods_loaded--;
            if (ptr->is_console) {
                console_mods_loaded--;
            }
            break;
        }
        last = ptr;
    }
    base_mutex_unlock(BINDLOCK);

    return status;
}

BASE_DECLARE(base_status_t) base_log_bind_logger(base_log_function_t function, base_log_level_t level, base_bool_t is_console)
{
    base_log_binding_t *binding = NULL, *ptr = NULL;
    base_assert(function != NULL);

    if (!(binding = base_core_alloc(LOG_POOL, sizeof(*binding)))) {
        return BASE_STATUS_MEMERR;
    }

    if ((uint8_t) level > MAX_LEVEL) {
        MAX_LEVEL = level;
    }

    binding->function = function;
    binding->level = level;
    binding->is_console = is_console;

    base_mutex_lock(BINDLOCK);
    for (ptr = BINDINGS; ptr && ptr->next; ptr = ptr->next);

    if (ptr) {
        ptr->next = binding;
    } else {
        BINDINGS = binding;
    }
    if (is_console) {
        console_mods_loaded++;
    }
    mods_loaded++;
    base_mutex_unlock(BINDLOCK);

    return BASE_STATUS_SUCCESS;
}

static base_thread_t *thread;

static void *BASE_THREAD_FUNC log_thread(base_thread_t *t, void *obj)
{

    if (!obj) {
        obj = NULL;
    }
    THREAD_RUNNING = 1;

    while (THREAD_RUNNING == 1) {
        void *pop = NULL;
        base_log_node_t *node = NULL;
        base_log_binding_t *binding;

        if (base_queue_pop(LOG_QUEUE, &pop) != BASE_STATUS_SUCCESS) {
            break;
        }

        if (!pop) {
            THREAD_RUNNING = -1;
            break;
        }

        node = (base_log_node_t *) pop;
        base_mutex_lock(BINDLOCK);
        for (binding = BINDINGS; binding; binding = binding->next) {
            if (binding->level >= node->level) {
                binding->function(node, node->level);
            }
        }
        base_mutex_unlock(BINDLOCK);

        base_log_node_free(&node);

    }

    THREAD_RUNNING = 0;
    base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_CONSOLE, "Logger Ended.\n");
    return NULL;
}

BASE_DECLARE(void) base_log_printf(base_text_channel_t channel, const char *file, const char *func, int line,
    const char *userdata, base_log_level_t level, const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    base_log_vprintf(channel, file, func, line, userdata, level, fmt, ap);
    va_end(ap);
}

BASE_DECLARE(void) base_custom_log(base_text_channel_t channel, const char *file, const char *func, int line,
    const char *filename, int logout_type, base_log_level_t level, const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    base_log_vprintf2(channel, file, func, line, filename, logout_type, level, fmt, ap);
    va_end(ap);
}

#define do_mods (LOG_QUEUE && THREAD_RUNNING)
BASE_DECLARE(void) base_log_vprintf(base_text_channel_t channel, const char *file, const char *func, int line,
    const char *userdata, base_log_level_t level, const char *fmt, va_list ap)
{
    base_log_vprintf2(channel, file, func, line, userdata, -1, level, fmt, ap);
}
BASE_DECLARE(void) base_log_vprintf2(base_text_channel_t channel, const char *file, const char *func, int line,
    const char *userdata, int logout_type, base_log_level_t level, const char *fmt, va_list ap)
{
    char *data = NULL;
    char *new_fmt = NULL;
    int ret = 0;
    FILE *handle;
    const char *filep = (file ? base_cut_path(file) : "");
    const char *funcp = (func ? func : "");
    char *content = NULL;
    base_time_t now = base_micro_time_now();
    uint32_t len;
#ifdef BASE_FUNC_IN_LOG
    const char *extra_fmt = "%s [%d] [%s] %s:%d %s()%c%s";
#else
    const char *extra_fmt = (zstr(filep) ? "%s [%d] [%s] %s" : "%s [%d] [%s] %s:%d%c%s");
#endif
    base_log_level_t limit_level = runtime.hard_log_level;
    base_log_level_t special_level = BASE_LOG_UNINIT;
    pid_t pid = 0;

    if (channel == BASE_CHANNEL_ID_SESSION && userdata) {
        /* zzf
        base_core_session_t *session = (base_core_session_t *) userdata;
        special_level = session->loglevel;
        if (limit_level < session->loglevel) {
        limit_level = session->loglevel;
        }*/
    }

    if (level > 100) {
        if ((uint32_t) (level - 100) > runtime.debug_level) {
            return;
        }

        level = 1;
    }

    if (level > limit_level) {
        return;
    }

    base_assert(level < BASE_LOG_INVALID);

    handle = base_core_data_channel(channel);
    pid = getpid();

    /*根据配置决定输出的日志格式*/
    if (base_core_get_log_format_josn() == BASE_TRUE && level != BASE_LOG_CONSOLE) {
        char *logdata = NULL;
        char *logdata_dup = NULL;
        char buf[2048];
        char date[80] = "";
        base_time_exp_t tm;

        base_time_exp_gmt(&tm, now);
        base_snprintf(date, sizeof(date), "%0.4d-%0.2d-%0.2dT%0.2d:%0.2d:%0.2d.%0.6d+0000",
            tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, tm.tm_usec);

        if (base_vasprintf(&logdata, fmt, ap) == -1) {
            goto end;
        }

        logdata_dup = base_cJSON_print_string_ptr2(logdata, buf, sizeof(buf));

#ifdef BASE_FUNC_IN_LOG
        data = base_mprintf("{\"time\":\"%s\","
            "\"pid\":%d,"
            "\"level\":\"%s\","
            "\"file\":\"%s:%d\","
            "\"func\":\"%s\","
            "\"msg\":\"%s\""
            "}\n",
            date, pid, base_log_level2str(level), filep, line,
            func,
            logdata_dup ? logdata_dup : buf);
#else
        if (zstr(filep)) {
            data = base_mprintf("{\"time\":\"%s\","
                "\"pid\":%d,"
                "\"level\":\"%s\","
                "\"msg\":\"%s\""
                "}\n",
                date, pid, base_log_level2str(level),
                logdata_dup ? logdata_dup : buf);
        } else {
            data = base_mprintf("{\"time\":\"%s\","
                "\"pid\":%d,"
                "\"level\":\"%s\","
                "\"file\":\"%s:%d\","
                "\"msg\":\"%s\""
                "}\n",
                date, pid, base_log_level2str(level), filep, line,
                logdata_dup ? logdata_dup : buf);
        }

#endif

        base_safe_free(logdata);
        base_safe_free(logdata_dup);

    } else if (channel != BASE_CHANNEL_ID_LOG_CLEAN) {
        char date[80] = "";
        //base_size_t retsize;
        base_time_exp_t tm;

        base_time_exp_gmt(&tm, now);
        base_snprintf(date, sizeof(date), "%0.4d-%0.2d-%0.2dT%0.2d:%0.2d:%0.2d.%0.6d+0000",
            tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, tm.tm_usec);

        //base_strftime_nocheck(date, &retsize, sizeof(date), "%Y-%m-%d %T", &tm);

#ifdef BASE_FUNC_IN_LOG
        len = (uint32_t) (strlen(extra_fmt) + strlen(date) + strlen(filep) + 32 + strlen(funcp) + strlen(fmt));
#else
        len = (uint32_t) (strlen(extra_fmt) + strlen(date) + strlen(filep) + 32 + strlen(fmt));
#endif
        new_fmt = malloc(len + 1);
        base_assert(new_fmt);
#ifdef BASE_FUNC_IN_LOG
        base_snprintf(new_fmt, len, extra_fmt, date, pid, base_log_level2str(level), filep, line, funcp, 128, fmt);
#else
        if (zstr(filep)) {
            base_snprintf(new_fmt, len, extra_fmt, date, pid, base_log_level2str(level), fmt);
        } else {
#ifdef __ANDROID__
            /** 这里new_fmt变量里有个%c类型,原来传的是128(属于扩展 ASCII 打印字符中的)
            * 这个%c传入大于127的,不一定在所有的平台都兼容,比如在api-level=21的Android平台上,%c=128的传入是个乱码,因此在vasprintf里面解释时会出错,即return -1
            * 我们的日志中实际上要打印的是空格,因此替换成空格的ASCII值,以此兼容Android平台
            */ 
            base_snprintf(new_fmt, len, extra_fmt, date, pid, base_log_level2str(level), filep, line, 32, fmt);
#else
            base_snprintf(new_fmt, len, extra_fmt, date, pid, base_log_level2str(level), filep, line, 128, fmt);
#endif
        }
#endif

        fmt = new_fmt;

        ret = base_vasprintf(&data, fmt, ap);

        if (ret == -1) {
            fprintf(stderr, "Memory Error\n");
            goto end;
        }
    } else {
        ret = base_vasprintf(&data, fmt, ap);

        if (ret == -1) {
            fprintf(stderr, "Memory Error\n");
            goto end;
        }
    }

    if (channel == BASE_CHANNEL_ID_LOG_CLEAN) {
        content = data;
    } else {
        if ((content = strchr(data, 128))) {
            *content = ' ';
        }
    }

    if (channel == BASE_CHANNEL_ID_EVENT) {
        base_event_t *event;
        if (base_event_running() == BASE_STATUS_SUCCESS && base_event_create(&event, BASE_EVENT_LOG) == BASE_STATUS_SUCCESS) {
            base_event_add_header_string(event, BASE_STACK_BOTTOM, "Log-Data", data);
            base_event_add_header_string(event, BASE_STACK_BOTTOM, "Log-File", filep);
            base_event_add_header_string(event, BASE_STACK_BOTTOM, "Log-Function", funcp);
            base_event_add_header(event, BASE_STACK_BOTTOM, "Log-Line", "%d", line);
            base_event_add_header(event, BASE_STACK_BOTTOM, "Log-Level", "%d", (int) level);
            if (!zstr(userdata)) {
                base_event_add_header_string(event, BASE_STACK_BOTTOM, "User-Data", userdata);
            }
            base_event_fire(&event);
            data = NULL;
        }

        goto end;
    }

    if ((console_mods_loaded == 0 || !do_mods) && (logout_type & BASE_LOG_OUT_TYPE_STD)) {
        if (handle) {
            int aok = 1;
#ifndef WIN32

            fd_set can_write;
            int fd;
            struct timeval to;

            fd = fileno(handle);
            memset(&to, 0, sizeof(to));
            FD_ZERO(&can_write);
            FD_SET(fd, &can_write);
            to.tv_sec = 0;
            to.tv_usec = 100000;
            if (select(fd + 1, NULL, &can_write, NULL, &to) > 0) {
                aok = FD_ISSET(fd, &can_write);
            } else {
                aok = 0;
            }
#endif
            if (aok) {
                if (COLORIZE) {

#ifdef WIN32
                    SetConsoleTextAttribute(hStdout, COLORS[level]);
                    WriteFile(hStdout, data, (DWORD) strlen(data), NULL, NULL);
                    SetConsoleTextAttribute(hStdout, wOldColorAttrs);
#else
                    fprintf(handle, "%s%s%s", COLORS[level], data, BASE_SEQ_DEFAULT_COLOR);
#endif
                } else {
                    fprintf(handle, "%s", data);
                }
            }
        }
    }

    if (do_mods && level <= MAX_LEVEL) {
        base_log_node_t *node = base_log_node_alloc();

        node->data = data;
        data = NULL;
        base_set_string(node->file, filep);
        base_set_string(node->func, funcp);
        node->line = line;
        node->level = level;
        node->slevel = special_level;
        node->content = content;
        node->timestamp = now;
        node->channel = channel;
        node->logout_type = logout_type;
        if (channel == BASE_CHANNEL_ID_SESSION) {
            /* zzf
            base_core_session_t *session = (base_core_session_t *) userdata;
            node->userdata = userdata ? strdup(base_core_session_get_uuid(session)) : NULL;
            */
        } else {
            node->userdata = !zstr(userdata) ? strdup(userdata) : NULL;
        }

        if (base_queue_trypush(LOG_QUEUE, node) != BASE_STATUS_SUCCESS) {
            base_log_node_free(&node);
        }
    }

end:

    base_safe_free(data);
    base_safe_free(new_fmt);

}

BASE_DECLARE(base_status_t) base_log_init(base_memory_pool_t *pool, base_bool_t colorize)
{
    base_threadattr_t *thd_attr;;

    base_assert(pool != NULL);

    LOG_POOL = pool;

    base_threadattr_create(&thd_attr, LOG_POOL);


    base_queue_create(&LOG_QUEUE, BASE_CORE_QUEUE_LEN, LOG_POOL);
#ifdef BASE_LOG_RECYCLE
    base_queue_create(&LOG_RECYCLE_QUEUE, BASE_CORE_QUEUE_LEN, LOG_POOL);
#endif
    base_mutex_init(&BINDLOCK, BASE_MUTEX_NESTED, LOG_POOL);
    base_threadattr_stacksize_set(thd_attr, BASE_THREAD_STACKSIZE);
    base_thread_create(&thread, thd_attr, log_thread, NULL, LOG_POOL);

    while (!THREAD_RUNNING) {
        base_cond_next();
    }

    if (colorize) {
#ifdef WIN32
        hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
        if (base_core_get_console() == stdout && hStdout != INVALID_HANDLE_VALUE && GetConsoleScreenBufferInfo(hStdout, &csbiInfo)) {
            wOldColorAttrs = csbiInfo.wAttributes;
            COLORIZE = BASE_TRUE;
        }
#else
        COLORIZE = BASE_TRUE;
#endif
    }


    return BASE_STATUS_SUCCESS;
}

BASE_DECLARE(void) base_core_memory_reclaim_logger(void)
{
#ifdef BASE_LOG_RECYCLE
    void *pop;
    int size = base_queue_size(LOG_RECYCLE_QUEUE);
    base_log_printf(BASE_CHANNEL_SESSION_LOG(session), BASE_LOG_CONSOLE, "Returning %d recycled log node(s) %d bytes\n", size,
        (int) sizeof(base_log_node_t) * size);
    while (base_queue_trypop(LOG_RECYCLE_QUEUE, &pop) == BASE_STATUS_SUCCESS) {
        base_log_node_free(&pop);
    }
#else
    return;
#endif

}

BASE_DECLARE(base_status_t) base_log_shutdown(void)
{
    base_status_t st;


    base_queue_push(LOG_QUEUE, NULL);
    while (THREAD_RUNNING) {
        base_cond_next();
    }

    base_thread_join(&st, thread);

    base_core_memory_reclaim_logger();

    return BASE_STATUS_SUCCESS;
}


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
