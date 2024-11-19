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
* Michael Jerris <mike@jerris.com>
* Paul D. Tinsley <pdt at jackhammer.org>
* Marcel Barbulescu <marcelbarbulescu@gmail.com>
* Joseph Sullivan <jossulli@amazon.com>
* Seven Du <dujinfang@gmail.com>
* Andrey Volk <andywolk@gmail.com>
*
* base_core.c -- Main Core Library
*
*/



#include <base.h>
#ifndef WIN32
#include <base_private.h>
#ifdef HAVE_SETRLIMIT
#include <sys/resource.h>
#endif
#endif
#include <errno.h>
#include <sqlite3.h>
#ifdef HAVE_SYS_PRCTL_H
#include <sys/prctl.h>
#endif
#ifdef SOLARIS_PRIVILEGES
#include <priv.h>
#endif

#ifdef __linux__
#include <sys/wait.h>
#ifndef _GNU_SOURCE
#define _GNU_SOURCE /* Required for POSIX_SPAWN_USEVFORK */
#endif
#include <spawn.h>
#include <poll.h>
#endif

#ifdef WIN32
#define popen _popen
#define pclose _pclose
#endif

#ifdef HAVE_SYSTEMD
#include <systemd/sd-daemon.h>
#endif

BASE_DECLARE_DATA base_directories BASE_GLOBAL_dirs = { 0 };
BASE_DECLARE_DATA base_filenames BASE_GLOBAL_filenames = { 0 };

/* The main runtime obj we keep this hidden for ourselves */
struct base_runtime runtime = { 0 };
static void base_load_core_config(const char *file);

static void send_heartbeat(void)
{
    base_event_t *event;
    base_core_time_duration_t duration;

    base_core_measure_time(base_core_uptime(), &duration);

    if (base_event_create(&event, BASE_EVENT_HEARTBEAT) == BASE_STATUS_SUCCESS) {
        base_event_add_header(event, BASE_STACK_BOTTOM, "Event-Info", "System Ready");
        base_event_add_header(event, BASE_STACK_BOTTOM, "Up-Time",
            "%u year%s, "
            "%u day%s, "
            "%u hour%s, "
            "%u minute%s, "
            "%u second%s, "
            "%u millisecond%s, "
            "%u microsecond%s",
            duration.yr, duration.yr == 1 ? "" : "s",
            duration.day, duration.day == 1 ? "" : "s",
            duration.hr, duration.hr == 1 ? "" : "s",
            duration.min, duration.min == 1 ? "" : "s",
            duration.sec, duration.sec == 1 ? "" : "s",
            duration.ms, duration.ms == 1 ? "" : "s", duration.mms, duration.mms == 1 ? "" : "s");

        base_event_add_header(event, BASE_STACK_BOTTOM, "FreeBASE-Version", "%s", base_version_full());
        base_event_add_header(event, BASE_STACK_BOTTOM, "Uptime-msec", "%"BASE_TIME_T_FMT, base_core_uptime() / 1000);
        base_event_add_header(event, BASE_STACK_BOTTOM, "Session-Count", "%u", base_core_session_count());
        base_event_add_header(event, BASE_STACK_BOTTOM, "Max-Sessions", "%u", base_core_session_limit(0));
        base_event_add_header(event, BASE_STACK_BOTTOM, "Session-Per-Sec", "%u", runtime.sps);
        base_event_add_header(event, BASE_STACK_BOTTOM, "Session-Per-Sec-Last", "%u", runtime.sps_last);
        base_event_add_header(event, BASE_STACK_BOTTOM, "Session-Per-Sec-Max", "%u", runtime.sps_peak);
        base_event_add_header(event, BASE_STACK_BOTTOM, "Session-Per-Sec-FiveMin", "%u", runtime.sps_peak_fivemin);
        base_event_add_header(event, BASE_STACK_BOTTOM, "Session-Since-Startup", "%" BASE_SIZE_T_FMT, base_core_session_id() - 1);
        base_event_add_header(event, BASE_STACK_BOTTOM, "Session-Peak-Max", "%u", runtime.sessions_peak);
        base_event_add_header(event, BASE_STACK_BOTTOM, "Session-Peak-FiveMin", "%u", runtime.sessions_peak_fivemin);
        base_event_add_header(event, BASE_STACK_BOTTOM, "Idle-CPU", "%f", base_core_idle_cpu());
        base_event_fire(&event);
    }
}

static char main_ip4[256] = "";
static char main_ip6[256] = "";

static void check_ip(void)
{
    char guess_ip4[256] = "";
    char guess_ip6[256] = "";
    char old_ip4[256] = "";
    char old_ip6[256] = "";
    int ok4 = 1, ok6 = 1;
    int mask = 0;
    base_status_t check6, check4;
    base_event_t *event;
    char *hostname = base_core_get_variable("hostname");

    gethostname(runtime.hostname, sizeof(runtime.hostname));

    if (zstr(hostname)) {
        base_core_set_variable("hostname", runtime.hostname);
    } else if (strcmp(hostname, runtime.hostname)) {
        if (base_event_create(&event, BASE_EVENT_TRAP) == BASE_STATUS_SUCCESS) {
            base_event_add_header(event, BASE_STACK_BOTTOM, "condition", "hostname-change");
            base_event_add_header_string(event, BASE_STACK_BOTTOM, "old-hostname", hostname);
            base_event_add_header_string(event, BASE_STACK_BOTTOM, "new-hostname", runtime.hostname);
            base_event_fire(&event);
        }

        base_core_set_variable("hostname", runtime.hostname);
    }

    check4 = base_find_local_ip(guess_ip4, sizeof(guess_ip4), &mask, AF_INET);
    check6 = base_find_local_ip(guess_ip6, sizeof(guess_ip6), NULL, AF_INET6);

    if (check6 != BASE_STATUS_SUCCESS && (zstr(main_ip6) || !strcasecmp(main_ip6, "::1"))) {
        check6 = BASE_STATUS_SUCCESS;
    }

    if (check4 != BASE_STATUS_SUCCESS) {
        ok4 = 2;
    } else if (!*main_ip4) {
        base_set_string(main_ip4, guess_ip4);
    } else {
        if (!(ok4 = !strcmp(main_ip4, guess_ip4))) {
            struct in_addr in;

            in.s_addr = mask;
            base_set_string(old_ip4, main_ip4);
            base_set_string(main_ip4, guess_ip4);
            base_core_set_variable("local_ip_v4", guess_ip4);
            base_core_set_variable("local_mask_v4", inet_ntoa(in));
        }
    }

    if (check6 != BASE_STATUS_SUCCESS) {
        ok6 = 2;
    } else if (!*main_ip6) {
        base_set_string(main_ip6, guess_ip6);
    } else {
        if (!(ok6 = !strcmp(main_ip6, guess_ip6))) {
            base_set_string(old_ip6, main_ip6);
            base_set_string(main_ip6, guess_ip6);
            base_core_set_variable("local_ip_v6", guess_ip6);
        }
    }

    if (!ok4 || !ok6) {
        if (base_event_create(&event, BASE_EVENT_TRAP) == BASE_STATUS_SUCCESS) {
            base_event_add_header(event, BASE_STACK_BOTTOM, "condition", "network-address-change");
            if (!ok4) {
                base_event_add_header_string(event, BASE_STACK_BOTTOM, "network-address-previous-v4", old_ip4);
                base_event_add_header_string(event, BASE_STACK_BOTTOM, "network-address-change-v4", main_ip4);
            }
            if (!ok6) {
                base_event_add_header_string(event, BASE_STACK_BOTTOM, "network-address-previous-v6", old_ip6);
                base_event_add_header_string(event, BASE_STACK_BOTTOM, "network-address-change-v6", main_ip6);
            }
            base_event_fire(&event);
        }
    }

    if (ok4 == 2 || ok6 == 2) {
        if (base_event_create(&event, BASE_EVENT_TRAP) == BASE_STATUS_SUCCESS) {
            base_event_add_header(event, BASE_STACK_BOTTOM, "condition", "network-outage");

            base_event_add_header_string(event, BASE_STACK_BOTTOM, "network-status-v4", ok4 == 2 ? "disconnected" : "active");
            base_event_add_header_string(event, BASE_STACK_BOTTOM, "network-address-v4", main_ip4);

            base_event_add_header_string(event, BASE_STACK_BOTTOM, "network-status-v6", ok6 == 2 ? "disconnected" : "active");
            base_event_add_header_string(event, BASE_STACK_BOTTOM, "network-address-v6", main_ip6);

            base_event_fire(&event);
        }
    }

}

BASE_STANDARD_SCHED_FUNC(heartbeat_callback)
{
    send_heartbeat();

    /* reschedule this task */
    task->runtime = base_epoch_time_now(NULL) + runtime.event_heartbeat_interval;
}


BASE_STANDARD_SCHED_FUNC(check_ip_callback)
{
    check_ip();

    /* reschedule this task */
    task->runtime = base_epoch_time_now(NULL) + 60;
}


BASE_DECLARE(base_status_t) base_core_set_console(const char *console)
{
    if ((runtime.console = fopen(console, "a")) == 0) {
        fprintf(stderr, "Cannot open output file %s.\n", console);
        return BASE_STATUS_FALSE;
    }

    return BASE_STATUS_SUCCESS;
}

BASE_DECLARE(FILE *) base_core_get_console(void)
{
    return runtime.console;
}

#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif
BASE_DECLARE(void) base_core_screen_size(int *x, int *y)
{

#ifdef WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    int ret;

    if ((ret = GetConsoleScreenBufferInfo(GetStdHandle( STD_OUTPUT_HANDLE ), &csbi))) {
        if (x) *x = csbi.dwSize.X;
        if (y) *y = csbi.dwSize.Y;
    }

#elif defined(TIOCGWINSZ)
    struct winsize w;
    ioctl(0, TIOCGWINSZ, &w);

    if (x) *x = w.ws_col;
    if (y) *y = w.ws_row;
#else
    if (x) *x = 80;
    if (y) *y = 24;
#endif

}

BASE_DECLARE(FILE *) base_core_data_channel(base_text_channel_t channel)
{
    return runtime.console;
}


BASE_DECLARE(void) base_core_remove_state_handler(const base_state_handler_table_t *state_handler)
{
    int index, tmp_index = 0;
    const base_state_handler_table_t *tmp[BASE_MAX_STATE_HANDLERS + 1] = { 0 };

    base_mutex_lock(runtime.global_mutex);

    for (index = 0; index < runtime.state_handler_index; index++) {
        const base_state_handler_table_t *cur = runtime.state_handlers[index];
        runtime.state_handlers[index] = NULL;
        if (cur == state_handler) {
            continue;
        }
        tmp[tmp_index++] = cur;
    }

    runtime.state_handler_index = 0;

    for (index = 0; index < tmp_index; index++) {
        runtime.state_handlers[runtime.state_handler_index++] = tmp[index];
    }
    base_mutex_unlock(runtime.global_mutex);
}


BASE_DECLARE(int) base_core_add_state_handler(const base_state_handler_table_t *state_handler)
{
    int index;

    base_mutex_lock(runtime.global_mutex);
    index = runtime.state_handler_index;

    if (index > (BASE_MAX_STATE_HANDLERS - 1)) {
        index = -1;
    } else {
        runtime.state_handlers[index] = state_handler;
        runtime.state_handler_index++;
    }

    base_mutex_unlock(runtime.global_mutex);
    return index;
}

BASE_DECLARE(const base_state_handler_table_t *) base_core_get_state_handler(int index)
{

    if (index >= BASE_MAX_STATE_HANDLERS || index > runtime.state_handler_index) {
        return NULL;
    }

    return runtime.state_handlers[index];
}

BASE_DECLARE(void) base_core_dump_variables(base_stream_handle_t *stream)
{
    base_event_header_t *hi;

    base_mutex_lock(runtime.global_mutex);
    for (hi = runtime.global_vars->headers; hi; hi = hi->next) {
        stream->write_function(stream, "%s=%s\n", hi->name, hi->value);
    }
    base_mutex_unlock(runtime.global_mutex);
}

BASE_DECLARE(const char *) base_core_get_hostname(void)
{
    return runtime.hostname;
}

BASE_DECLARE(const char *) base_core_get_basename(void)
{
    if (!zstr(runtime.basename)) return runtime.basename;
    return runtime.hostname;
}

BASE_DECLARE(char *) base_core_get_domain(base_bool_t dup)
{
    char *domain;
    const char *var;

    base_thread_rwlock_rdlock(runtime.global_var_rwlock);
    if (!(var = base_core_get_variable("domain"))) {
        var = "freebase.local";
    }
    if (dup) {
        domain = strdup(var);
    } else {
        domain = (char *) var;
    }
    base_thread_rwlock_unlock(runtime.global_var_rwlock);

    return domain;
}

BASE_DECLARE(base_status_t) base_core_get_variables(base_event_t **event)
{
    base_status_t status;
    base_thread_rwlock_rdlock(runtime.global_var_rwlock);
    status = base_event_dup(event, runtime.global_vars);
    base_thread_rwlock_unlock(runtime.global_var_rwlock);
    return status;
}

BASE_DECLARE(char *) base_core_get_variable(const char *varname)
{
    char *val;
    base_thread_rwlock_rdlock(runtime.global_var_rwlock);
    val = (char *) base_event_get_header(runtime.global_vars, varname);
    base_thread_rwlock_unlock(runtime.global_var_rwlock);
    return val;
}

BASE_DECLARE(char *) base_core_get_variable_dup(const char *varname)
{
    char *val = NULL, *v;

    if (varname) {
        base_thread_rwlock_rdlock(runtime.global_var_rwlock);
        if ((v = (char *) base_event_get_header(runtime.global_vars, varname))) {
            val = strdup(v);
        }
        base_thread_rwlock_unlock(runtime.global_var_rwlock);
    }

    return val;
}

BASE_DECLARE(char *) base_core_get_variable_pdup(const char *varname, base_memory_pool_t *pool)
{
    char *val = NULL, *v;

    if (varname) {
        base_thread_rwlock_rdlock(runtime.global_var_rwlock);
        if ((v = (char *) base_event_get_header(runtime.global_vars, varname))) {
            val = base_core_strdup(pool, v);
        }
        base_thread_rwlock_unlock(runtime.global_var_rwlock);
    }

    return val;
}

static void base_core_unset_variables(void)
{
    base_thread_rwlock_wrlock(runtime.global_var_rwlock);
    base_event_destroy(&runtime.global_vars);
    base_event_create_plain(&runtime.global_vars, BASE_EVENT_CHANNEL_DATA);
    base_thread_rwlock_unlock(runtime.global_var_rwlock);
}

BASE_DECLARE(void) base_core_set_variable(const char *varname, const char *value)
{
    char *val;

    if (varname) {
        base_thread_rwlock_wrlock(runtime.global_var_rwlock);
        val = (char *) base_event_get_header(runtime.global_vars, varname);
        if (val) {
            base_event_del_header(runtime.global_vars, varname);
        }
        if (value) {
            char *v = strdup(value);
            base_string_var_check(v, BASE_TRUE);
            base_event_add_header_string(runtime.global_vars, BASE_STACK_BOTTOM, varname, v);
            free(v);
        } else {
            base_event_del_header(runtime.global_vars, varname);
        }
        base_thread_rwlock_unlock(runtime.global_var_rwlock);
    }
}

BASE_DECLARE(base_bool_t) base_core_set_var_conditional(const char *varname, const char *value, const char *val2)
{
    char *val;

    if (varname) {
        base_thread_rwlock_wrlock(runtime.global_var_rwlock);
        val = (char *) base_event_get_header(runtime.global_vars, varname);

        if (val) {
            if (!val2 || strcmp(val, val2) != 0) {
                base_thread_rwlock_unlock(runtime.global_var_rwlock);
                return BASE_FALSE;
            }
            base_event_del_header(runtime.global_vars, varname);
        } else if (!zstr(val2)) {
            base_thread_rwlock_unlock(runtime.global_var_rwlock);
            return BASE_FALSE;
        }

        if (value) {
            char *v = strdup(value);
            base_string_var_check(v, BASE_TRUE);
            base_event_add_header_string_nodup(runtime.global_vars, BASE_STACK_BOTTOM, varname, v);
        } else {
            base_event_del_header(runtime.global_vars, varname);
        }
        base_thread_rwlock_unlock(runtime.global_var_rwlock);
    }
    return BASE_TRUE;
}

BASE_DECLARE(char *) base_core_get_uuid(void)
{
    return runtime.uuid_str;
}


static void *BASE_THREAD_FUNC base_core_service_thread(base_thread_t *thread, void *obj)
{
    base_core_session_t *session = obj;
    base_channel_t *channel;
    base_frame_t *read_frame = NULL;

    //  base_assert(thread != NULL);
    //  base_assert(session != NULL);

    if (base_core_session_read_lock(session) != BASE_STATUS_SUCCESS) {
        return NULL;
    }

    base_mutex_lock(session->frame_read_mutex);

    channel = base_core_session_get_channel(session);

    base_channel_set_flag(channel, CF_SERVICE);
    while (base_channel_test_flag(channel, CF_SERVICE)) {

        if (base_channel_test_flag(channel, CF_SERVICE_AUDIO)) {
            base (base_core_session_read_frame(session, &read_frame, BASE_IO_FLAG_NONE, 0)) {
            case BASE_STATUS_SUCCESS:
            case BASE_STATUS_TIMEOUT:
            case BASE_STATUS_BREAK:
                break;
            default:
                base_channel_clear_flag(channel, CF_SERVICE);
                break;
            }
        }

        if (base_channel_test_flag(channel, CF_SERVICE_VIDEO) && base_channel_test_flag(channel, CF_VIDEO)) {
            base (base_core_session_read_video_frame(session, &read_frame, BASE_IO_FLAG_NONE, 0)) {
            case BASE_STATUS_SUCCESS:
            case BASE_STATUS_TIMEOUT:
            case BASE_STATUS_BREAK:
                break;
            default:
                base_channel_clear_flag(channel, CF_SERVICE);
                break;
            }
        }
    }

    base_mutex_unlock(session->frame_read_mutex);

    base_channel_clear_flag(channel, CF_SERVICE_AUDIO);
    base_channel_clear_flag(channel, CF_SERVICE_VIDEO);

    base_core_session_rwunlock(session);

    return NULL;
}

/* Either add a timeout here or make damn sure the thread cannot get hung somehow (my preference) */
BASE_DECLARE(void) base_core_thread_session_end(base_core_session_t *session)
{
    base_channel_t *channel;
    base_assert(session);

    channel = base_core_session_get_channel(session);
    base_assert(channel);

    base_channel_clear_flag(channel, CF_SERVICE);
    base_channel_clear_flag(channel, CF_SERVICE_AUDIO);
    base_channel_clear_flag(channel, CF_SERVICE_VIDEO);

    base_core_session_kill_channel(session, BASE_SIG_BREAK);

}

BASE_DECLARE(void) base_core_service_session_av(base_core_session_t *session, base_bool_t audio, base_bool_t video)
{
    base_channel_t *channel;
    base_assert(session);

    channel = base_core_session_get_channel(session);
    base_assert(channel);

    if (audio) base_channel_set_flag(channel, CF_SERVICE_AUDIO);
    if (video) base_channel_set_flag(channel, CF_SERVICE_VIDEO);

    base_core_session_launch_thread(session, (void *(*)(base_thread_t *,void *))base_core_service_thread, session);
}

/* This function abstracts the thread creation for modules by allowing you to pass a function ptr and
a void object and trust that that the function will be run in a thread with arg  This lets
you request and activate a thread without giving up any knowledge about what is in the thread
neither the core nor the calling module know anything about each other.

This thread is expected to never exit until the application exits so the func is responsible
to make sure that is the case.

The typical use for this is so base_loadable_module.c can start up a thread for each module
passing the table of module methods as a session obj into the core without actually allowing
the core to have any clue and keeping base_loadable_module.c from needing any thread code.

*/

BASE_DECLARE(base_thread_t *) base_core_launch_thread(base_thread_start_t func, void *obj, base_memory_pool_t *pool)
{
    base_thread_t *thread = NULL;
    base_threadattr_t *thd_attr = NULL;
    base_core_thread_session_t *ts;
    int mypool;

    mypool = pool ? 0 : 1;

    if (!pool && base_core_new_memory_pool(&pool) != BASE_STATUS_SUCCESS) {
        base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_CRIT, "Could not allocate memory pool\n");
        return NULL;
    }

    base_threadattr_create(&thd_attr, pool);

    if ((ts = base_core_alloc(pool, sizeof(*ts))) == 0) {
        base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_CRIT, "Could not allocate memory\n");
    } else {
        if (mypool) {
            ts->pool = pool;
        }
        ts->objs[0] = obj;
        ts->objs[1] = thread;
        base_threadattr_stacksize_set(thd_attr, BASE_THREAD_STACKSIZE);
        base_threadattr_priority_set(thd_attr, BASE_PRI_REALTIME);
        base_thread_create(&thread, thd_attr, func, ts, pool);
    }

    return thread;
}

BASE_DECLARE(void) base_core_set_globals(void)
{
#define BUFSIZE 1024
#ifdef WIN32
    char lpPathBuffer[BUFSIZE];
    DWORD dwBufSize = BUFSIZE;
    char base_dir[1024];
    char *lastbacklash;
    char *tmp;

    GetModuleFileName(NULL, base_dir, BUFSIZE);
    lastbacklash = strrchr(base_dir, '\\');
    base_dir[(lastbacklash - base_dir)] = '\0';
    /* set base_dir as cwd, to be able to use relative paths in scripting languages (e.g. mod_lua) when FS is running as a service or while debugging FS using visual studio */
    SetCurrentDirectory(base_dir);
    tmp = base_string_replace(base_dir, "\\", "/");
    strcpy(base_dir, tmp);
    free(tmp);

#else
    char base_dir[1024] = BASE_PREFIX_DIR;
#endif

    /* Order of precedence for, eg, rundir:
    *   -run
    *   -base
    *   --with-rundir
    *   --prefix
    */

    if (!BASE_GLOBAL_dirs.mod_dir && (BASE_GLOBAL_dirs.mod_dir = (char *) malloc(BUFSIZE))) {
        if (BASE_GLOBAL_dirs.base_dir)
            base_snprintf(BASE_GLOBAL_dirs.mod_dir, BUFSIZE, "%s%smod", BASE_GLOBAL_dirs.base_dir, BASE_PATH_SEPARATOR);
        else
#ifdef BASE_MOD_DIR
            base_snprintf(BASE_GLOBAL_dirs.mod_dir, BUFSIZE, "%s", BASE_MOD_DIR);
#else
            base_snprintf(BASE_GLOBAL_dirs.mod_dir, BUFSIZE, "%s%smod", base_dir, BASE_PATH_SEPARATOR);
#endif
    }

    if (!BASE_GLOBAL_dirs.lib_dir && (BASE_GLOBAL_dirs.lib_dir = (char *) malloc(BUFSIZE))) {
        if (BASE_GLOBAL_dirs.base_dir)
            base_snprintf(BASE_GLOBAL_dirs.lib_dir, BUFSIZE, "%s%slib", BASE_GLOBAL_dirs.base_dir, BASE_PATH_SEPARATOR);
        else
#ifdef BASE_LIB_DIR
            base_snprintf(BASE_GLOBAL_dirs.lib_dir, BUFSIZE, "%s", BASE_LIB_DIR);
#else
            base_snprintf(BASE_GLOBAL_dirs.lib_dir, BUFSIZE, "%s%slib", base_dir, BASE_PATH_SEPARATOR);
#endif
    }

    if (!BASE_GLOBAL_dirs.conf_dir && (BASE_GLOBAL_dirs.conf_dir = (char *) malloc(BUFSIZE))) {
        if (BASE_GLOBAL_dirs.base_dir)
            base_snprintf(BASE_GLOBAL_dirs.conf_dir, BUFSIZE, "%s%sconf", BASE_GLOBAL_dirs.base_dir, BASE_PATH_SEPARATOR);
        else
#ifdef BASE_CONF_DIR
            base_snprintf(BASE_GLOBAL_dirs.conf_dir, BUFSIZE, "%s", BASE_CONF_DIR);
#else
            base_snprintf(BASE_GLOBAL_dirs.conf_dir, BUFSIZE, "%s%sconf", base_dir, BASE_PATH_SEPARATOR);
#endif
    }

    if (!BASE_GLOBAL_dirs.log_dir && (BASE_GLOBAL_dirs.log_dir = (char *) malloc(BUFSIZE))) {
        if (BASE_GLOBAL_dirs.base_dir)
            base_snprintf(BASE_GLOBAL_dirs.log_dir, BUFSIZE, "%s%slog", BASE_GLOBAL_dirs.base_dir, BASE_PATH_SEPARATOR);
        else
#ifdef BASE_LOG_DIR
            base_snprintf(BASE_GLOBAL_dirs.log_dir, BUFSIZE, "%s", BASE_LOG_DIR);
#else
            base_snprintf(BASE_GLOBAL_dirs.log_dir, BUFSIZE, "%s%slog", base_dir, BASE_PATH_SEPARATOR);
#endif
    }

    if (!BASE_GLOBAL_dirs.run_dir && (BASE_GLOBAL_dirs.run_dir = (char *) malloc(BUFSIZE))) {
        if (BASE_GLOBAL_dirs.base_dir)
            base_snprintf(BASE_GLOBAL_dirs.run_dir, BUFSIZE, "%s%srun", BASE_GLOBAL_dirs.base_dir, BASE_PATH_SEPARATOR);
        else
#ifdef BASE_RUN_DIR
            base_snprintf(BASE_GLOBAL_dirs.run_dir, BUFSIZE, "%s", BASE_RUN_DIR);
#else
            base_snprintf(BASE_GLOBAL_dirs.run_dir, BUFSIZE, "%s%srun", base_dir, BASE_PATH_SEPARATOR);
#endif
    }

    if (!BASE_GLOBAL_dirs.recordings_dir && (BASE_GLOBAL_dirs.recordings_dir = (char *) malloc(BUFSIZE))) {
        if (BASE_GLOBAL_dirs.base_dir)
            base_snprintf(BASE_GLOBAL_dirs.recordings_dir, BUFSIZE, "%s%srecordings", BASE_GLOBAL_dirs.base_dir, BASE_PATH_SEPARATOR);
        else
#ifdef BASE_RECORDINGS_DIR
            base_snprintf(BASE_GLOBAL_dirs.recordings_dir, BUFSIZE, "%s", BASE_RECORDINGS_DIR);
#else
            base_snprintf(BASE_GLOBAL_dirs.recordings_dir, BUFSIZE, "%s%srecordings", base_dir, BASE_PATH_SEPARATOR);
#endif
    }

    if (!BASE_GLOBAL_dirs.sounds_dir && (BASE_GLOBAL_dirs.sounds_dir = (char *) malloc(BUFSIZE))) {
        if (BASE_GLOBAL_dirs.base_dir)
            base_snprintf(BASE_GLOBAL_dirs.sounds_dir, BUFSIZE, "%s%ssounds", BASE_GLOBAL_dirs.base_dir, BASE_PATH_SEPARATOR);
        else
#ifdef BASE_SOUNDS_DIR
            base_snprintf(BASE_GLOBAL_dirs.sounds_dir, BUFSIZE, "%s", BASE_SOUNDS_DIR);
#else
            base_snprintf(BASE_GLOBAL_dirs.sounds_dir, BUFSIZE, "%s%ssounds", base_dir, BASE_PATH_SEPARATOR);
#endif
    }

    if (!BASE_GLOBAL_dirs.storage_dir && (BASE_GLOBAL_dirs.storage_dir = (char *) malloc(BUFSIZE))) {
        if (BASE_GLOBAL_dirs.base_dir)
            base_snprintf(BASE_GLOBAL_dirs.storage_dir, BUFSIZE, "%s%sstorage", BASE_GLOBAL_dirs.base_dir, BASE_PATH_SEPARATOR);
        else
#ifdef BASE_STORAGE_DIR
            base_snprintf(BASE_GLOBAL_dirs.storage_dir, BUFSIZE, "%s", BASE_STORAGE_DIR);
#else
            base_snprintf(BASE_GLOBAL_dirs.storage_dir, BUFSIZE, "%s%sstorage", base_dir, BASE_PATH_SEPARATOR);
#endif
    }

    if (!BASE_GLOBAL_dirs.cache_dir && (BASE_GLOBAL_dirs.cache_dir = (char *) malloc(BUFSIZE))) {
        if (BASE_GLOBAL_dirs.base_dir)
            base_snprintf(BASE_GLOBAL_dirs.cache_dir, BUFSIZE, "%s%scache", BASE_GLOBAL_dirs.base_dir, BASE_PATH_SEPARATOR);
        else
#ifdef BASE_CACHE_DIR
            base_snprintf(BASE_GLOBAL_dirs.cache_dir, BUFSIZE, "%s", BASE_CACHE_DIR);
#else
            base_snprintf(BASE_GLOBAL_dirs.cache_dir, BUFSIZE, "%s%scache", base_dir, BASE_PATH_SEPARATOR);
#endif
    }

    if (!BASE_GLOBAL_dirs.db_dir && (BASE_GLOBAL_dirs.db_dir = (char *) malloc(BUFSIZE))) {
        if (BASE_GLOBAL_dirs.base_dir)
            base_snprintf(BASE_GLOBAL_dirs.db_dir, BUFSIZE, "%s%sdb", BASE_GLOBAL_dirs.base_dir, BASE_PATH_SEPARATOR);
        else
#ifdef BASE_DB_DIR
            base_snprintf(BASE_GLOBAL_dirs.db_dir, BUFSIZE, "%s", BASE_DB_DIR);
#else
            base_snprintf(BASE_GLOBAL_dirs.db_dir, BUFSIZE, "%s%sdb", base_dir, BASE_PATH_SEPARATOR);
#endif
    }

    if (!BASE_GLOBAL_dirs.script_dir && (BASE_GLOBAL_dirs.script_dir = (char *) malloc(BUFSIZE))) {
        if (BASE_GLOBAL_dirs.base_dir)
            base_snprintf(BASE_GLOBAL_dirs.script_dir, BUFSIZE, "%s%sscripts", BASE_GLOBAL_dirs.base_dir, BASE_PATH_SEPARATOR);
        else
#ifdef BASE_SCRIPT_DIR
            base_snprintf(BASE_GLOBAL_dirs.script_dir, BUFSIZE, "%s", BASE_SCRIPT_DIR);
#else
            base_snprintf(BASE_GLOBAL_dirs.script_dir, BUFSIZE, "%s%sscripts", base_dir, BASE_PATH_SEPARATOR);
#endif
    }

    if (!BASE_GLOBAL_dirs.htdocs_dir && (BASE_GLOBAL_dirs.htdocs_dir = (char *) malloc(BUFSIZE))) {
        if (BASE_GLOBAL_dirs.base_dir)
            base_snprintf(BASE_GLOBAL_dirs.htdocs_dir, BUFSIZE, "%s%shtdocs", BASE_GLOBAL_dirs.base_dir, BASE_PATH_SEPARATOR);
        else
#ifdef BASE_HTDOCS_DIR
            base_snprintf(BASE_GLOBAL_dirs.htdocs_dir, BUFSIZE, "%s", BASE_HTDOCS_DIR);
#else
            base_snprintf(BASE_GLOBAL_dirs.htdocs_dir, BUFSIZE, "%s%shtdocs", base_dir, BASE_PATH_SEPARATOR);
#endif
    }

    if (!BASE_GLOBAL_dirs.grammar_dir && (BASE_GLOBAL_dirs.grammar_dir = (char *) malloc(BUFSIZE))) {
        if (BASE_GLOBAL_dirs.base_dir)
            base_snprintf(BASE_GLOBAL_dirs.grammar_dir, BUFSIZE, "%s%sgrammar", BASE_GLOBAL_dirs.base_dir, BASE_PATH_SEPARATOR);
        else
#ifdef BASE_GRAMMAR_DIR
            base_snprintf(BASE_GLOBAL_dirs.grammar_dir, BUFSIZE, "%s", BASE_GRAMMAR_DIR);
#else
            base_snprintf(BASE_GLOBAL_dirs.grammar_dir, BUFSIZE, "%s%sgrammar", base_dir, BASE_PATH_SEPARATOR);
#endif
    }

    if (!BASE_GLOBAL_dirs.fonts_dir && (BASE_GLOBAL_dirs.fonts_dir = (char *) malloc(BUFSIZE))) {
        if (BASE_GLOBAL_dirs.base_dir)
            base_snprintf(BASE_GLOBAL_dirs.fonts_dir, BUFSIZE, "%s%sfonts", BASE_GLOBAL_dirs.base_dir, BASE_PATH_SEPARATOR);
        else
#ifdef BASE_FONTS_DIR
            base_snprintf(BASE_GLOBAL_dirs.fonts_dir, BUFSIZE, "%s", BASE_FONTS_DIR);
#else
            base_snprintf(BASE_GLOBAL_dirs.fonts_dir, BUFSIZE, "%s%sfonts", base_dir, BASE_PATH_SEPARATOR);
#endif
    }

    if (!BASE_GLOBAL_dirs.images_dir && (BASE_GLOBAL_dirs.images_dir = (char *) malloc(BUFSIZE))) {
        if (BASE_GLOBAL_dirs.base_dir)
            base_snprintf(BASE_GLOBAL_dirs.images_dir, BUFSIZE, "%s%simages", BASE_GLOBAL_dirs.base_dir, BASE_PATH_SEPARATOR);
        else
#ifdef BASE_IMAGES_DIR
            base_snprintf(BASE_GLOBAL_dirs.images_dir, BUFSIZE, "%s", BASE_IMAGES_DIR);
#else
            base_snprintf(BASE_GLOBAL_dirs.images_dir, BUFSIZE, "%s%simages", base_dir, BASE_PATH_SEPARATOR);
#endif
    }

    if (!BASE_GLOBAL_dirs.data_dir && (BASE_GLOBAL_dirs.data_dir = (char *) malloc(BUFSIZE))) {
        if (BASE_GLOBAL_dirs.base_dir)
            base_snprintf(BASE_GLOBAL_dirs.data_dir, BUFSIZE, "%s", BASE_GLOBAL_dirs.base_dir);
        else
#ifdef BASE_DATA_DIR
            base_snprintf(BASE_GLOBAL_dirs.data_dir, BUFSIZE, "%s", BASE_DATA_DIR);
#else
            base_snprintf(BASE_GLOBAL_dirs.data_dir, BUFSIZE, "%s", base_dir);
#endif
    }

    if (!BASE_GLOBAL_dirs.localstate_dir && (BASE_GLOBAL_dirs.localstate_dir = (char *) malloc(BUFSIZE))) {
        if (BASE_GLOBAL_dirs.base_dir)
            base_snprintf(BASE_GLOBAL_dirs.localstate_dir, BUFSIZE, "%s", BASE_GLOBAL_dirs.base_dir);
        else
#ifdef BASE_LOCALSTATE_DIR
            base_snprintf(BASE_GLOBAL_dirs.localstate_dir, BUFSIZE, "%s", BASE_LOCALSTATE_DIR);
#else
            base_snprintf(BASE_GLOBAL_dirs.localstate_dir, BUFSIZE, "%s", base_dir);
#endif
    }

    if (!BASE_GLOBAL_dirs.certs_dir && (BASE_GLOBAL_dirs.certs_dir = (char *) malloc(BUFSIZE))) {
        if (BASE_GLOBAL_dirs.base_dir)
            base_snprintf(BASE_GLOBAL_dirs.certs_dir, BUFSIZE, "%s%scert", BASE_GLOBAL_dirs.base_dir, BASE_PATH_SEPARATOR);
        else
#ifdef BASE_CERTS_DIR
            base_snprintf(BASE_GLOBAL_dirs.certs_dir, BUFSIZE, "%s", BASE_CERTS_DIR);
#else
            base_snprintf(BASE_GLOBAL_dirs.certs_dir, BUFSIZE, "%s%scert", base_dir, BASE_PATH_SEPARATOR);
#endif
    }

    if (!BASE_GLOBAL_dirs.temp_dir && (BASE_GLOBAL_dirs.temp_dir = (char *) malloc(BUFSIZE))) {
#ifdef BASE_TEMP_DIR
        base_snprintf(BASE_GLOBAL_dirs.temp_dir, BUFSIZE, "%s", BASE_TEMP_DIR);
#else
#ifdef WIN32
        GetTempPath(dwBufSize, lpPathBuffer);
        lpPathBuffer[strlen(lpPathBuffer)-1] = 0;
        tmp = base_string_replace(lpPathBuffer, "\\", "/");
        strcpy(lpPathBuffer, tmp);
        free(tmp);
        base_snprintf(BASE_GLOBAL_dirs.temp_dir, BUFSIZE, "%s", lpPathBuffer);
#else
        base_snprintf(BASE_GLOBAL_dirs.temp_dir, BUFSIZE, "%s", "/tmp");
#endif
#endif
    }

    if (!BASE_GLOBAL_filenames.conf_name && (BASE_GLOBAL_filenames.conf_name = (char *) malloc(BUFSIZE))) {
        base_snprintf(BASE_GLOBAL_filenames.conf_name, BUFSIZE, "%s", "freebase.xml");
    }

    /* Do this last because it being empty is part of the above logic */
    if (!BASE_GLOBAL_dirs.base_dir && (BASE_GLOBAL_dirs.base_dir = (char *) malloc(BUFSIZE))) {
        base_snprintf(BASE_GLOBAL_dirs.base_dir, BUFSIZE, "%s", base_dir);
    }

    base_assert(BASE_GLOBAL_dirs.base_dir);
    base_assert(BASE_GLOBAL_dirs.mod_dir);
    base_assert(BASE_GLOBAL_dirs.lib_dir);
    base_assert(BASE_GLOBAL_dirs.conf_dir);
    base_assert(BASE_GLOBAL_dirs.log_dir);
    base_assert(BASE_GLOBAL_dirs.run_dir);
    base_assert(BASE_GLOBAL_dirs.db_dir);
    base_assert(BASE_GLOBAL_dirs.script_dir);
    base_assert(BASE_GLOBAL_dirs.htdocs_dir);
    base_assert(BASE_GLOBAL_dirs.grammar_dir);
    base_assert(BASE_GLOBAL_dirs.fonts_dir);
    base_assert(BASE_GLOBAL_dirs.images_dir);
    base_assert(BASE_GLOBAL_dirs.recordings_dir);
    base_assert(BASE_GLOBAL_dirs.sounds_dir);
    base_assert(BASE_GLOBAL_dirs.certs_dir);
    base_assert(BASE_GLOBAL_dirs.temp_dir);
    base_assert(BASE_GLOBAL_dirs.data_dir);
    base_assert(BASE_GLOBAL_dirs.localstate_dir);

    base_assert(BASE_GLOBAL_filenames.conf_name);
}


BASE_DECLARE(int32_t) base_core_set_process_privileges(void)
{
#ifdef SOLARIS_PRIVILEGES
    priv_set_t *basicset;

    /* make the process privilege-aware */
    setpflags(PRIV_AWARE, 1);

    /* reset the privileges to basic */
    basicset = priv_str_to_set("basic", ",", NULL);
    if (setppriv(PRIV_SET, PRIV_EFFECTIVE, basicset) != 0) {
        fprintf(stderr, "ERROR: Failed to acquire basic privileges (%s)\n", strerror(errno));
    }

    /* we need high-resolution clock, and this requires a non-basic privilege */
    if (priv_set(PRIV_ON, PRIV_EFFECTIVE, PRIV_PROC_CLOCK_HIGHRES, NULL) < 0) {
        fprintf(stderr, "ERROR: Failed to acquire proc_clock_highres privilege (%s)\n", strerror(errno));
        return -1;
    }

    /* need this for setrlimit */
    if (priv_set(PRIV_ON, PRIV_EFFECTIVE, PRIV_SYS_RESOURCE, NULL) < 0) {
        fprintf(stderr, "ERROR: Failed to acquire sys_resource privilege (%s)\n", strerror(errno));
        return -1;
    }

    /* we need to read directories belonging to other uid */
    if (priv_set(PRIV_ON, PRIV_EFFECTIVE, PRIV_FILE_DAC_SEARCH, NULL) < 0) {
        fprintf(stderr, "ERROR: Failed to acquire file_dac_search privilege (%s)\n", strerror(errno));
        return -1;
    }
#endif
    return 0;
}

BASE_DECLARE(int32_t) set_low_priority(void)
{
#ifdef WIN32
    return SetPriorityClass(GetCurrentProcess(), BELOW_NORMAL_PRIORITY_CLASS);
#else
#if defined(USE_SCHED_SETSCHEDULER) && ! defined(SOLARIS_PRIVILEGES)
    /*
    * Try to use a normal scheduler
    */
    struct sched_param sched = { 0 };
    sched.sched_priority = 0;
    if (sched_setscheduler(0, SCHED_OTHER, &sched) < 0) {
        fprintf(stderr, "ERROR: Failed to set SCHED_OTHER scheduler (%s)\n", strerror(errno));
        return -1;
    }
#endif

#ifdef HAVE_SETPRIORITY
    /*
    * setpriority() works on FreeBSD (6.2), nice() doesn't
    */
    if (setpriority(PRIO_PROCESS, getpid(), 19) < 0) {
        fprintf(stderr, "ERROR: Could not set nice level\n");
        return -1;
    }
#else
    if (nice(19) != 19) {
        fprintf(stderr, "ERROR: Could not set nice level\n");
        return -1;
    }
#endif

    return 0;
#endif
}

BASE_DECLARE(int32_t) set_realtime_priority(void)
{
#ifdef WIN32
    return SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
#else
#ifdef USE_SCHED_SETSCHEDULER
    /*
    * Try to use a round-robin scheduler
    * with a fallback if that does not work
    */
    struct sched_param sched = { 0 };
    sched.sched_priority = BASE_PRI_LOW;
#endif

#ifdef SOLARIS_PRIVILEGES
    /* request the privileges to elevate the priority */
    if (priv_set(PRIV_ON, PRIV_EFFECTIVE, PRIV_PROC_PRIOCNTL, NULL) < 0) {
#ifdef PRIV_PROC_PRIOUP
        /* fallback to PRIV_PROC_PRIOUP on SmartOS */
        fprintf(stderr, "WARN: Failed to acquire proc_priocntl privilege (%s)\n", strerror(errno));
        if (priv_set(PRIV_ON, PRIV_EFFECTIVE, PRIV_PROC_PRIOUP, NULL) < 0) {
            fprintf(stderr, "ERROR: Failed to acquire proc_prioup privilege (%s)\n", strerror(errno));
            return -1;
        }
#else
        fprintf(stderr, "ERROR: Failed to acquire proc_priocntl privilege (%s)\n", strerror(errno));
        return -1;
#endif
    }

    if (sched_setscheduler(0, SCHED_FIFO, &sched) < 0) {
        fprintf(stderr, "WARN: Failed to set SCHED_FIFO scheduler (%s)\n", strerror(errno));
    } else {
        return 0;
    }

    if (setpriority(PRIO_PROCESS, 0, -10) < 0) {
        fprintf(stderr, "ERROR: Could not set nice level\n");
        return -1;
    }

    return 0;
#else

#ifdef USE_SCHED_SETSCHEDULER
    if (sched_setscheduler(0, SCHED_FIFO, &sched) < 0) {
        fprintf(stderr, "ERROR: Failed to set SCHED_FIFO scheduler (%s)\n", strerror(errno));
        sched.sched_priority = 0;
        if (sched_setscheduler(0, SCHED_OTHER, &sched) < 0 ) {
            fprintf(stderr, "ERROR: Failed to set SCHED_OTHER scheduler (%s)\n", strerror(errno));
            return -1;
        }
    }
#endif

#ifdef HAVE_SETPRIORITY
    /*
    * setpriority() works on FreeBSD (6.2), nice() doesn't
    */
    if (setpriority(PRIO_PROCESS, getpid(), -10) < 0) {
        fprintf(stderr, "ERROR: Could not set nice level\n");
        return -1;
    }
#else
    if (nice(-10) != -10) {
        fprintf(stderr, "ERROR: Could not set nice level\n");
        return -1;
    }
#endif
#endif
    return 0;
#endif
}

BASE_DECLARE(uint32_t) base_core_cpu_count(void)
{
    return runtime.cpu_count;
}

BASE_DECLARE(int32_t) set_normal_priority(void)
{
    return 0;
}

BASE_DECLARE(int32_t) set_auto_priority(void)
{
#ifndef WIN32
    runtime.cpu_count = sysconf (_SC_NPROCESSORS_ONLN);
#else
    SYSTEM_INFO sysinfo;
    GetSystemInfo( &sysinfo );
    runtime.cpu_count = sysinfo.dwNumberOfProcessors;
#endif

    if (!runtime.cpu_count) runtime.cpu_count = 1;

    return set_realtime_priority();


    // ERROR: code not reachable on Windows Visual Studio Express 2008 return 0;
}

BASE_DECLARE(int32_t) change_user_group(const char *user, const char *group)
{
#ifndef WIN32
    uid_t runas_uid = 0;
    gid_t runas_gid = 0;
    struct passwd *runas_pw = NULL;

    if (user) {
        /*
        * Lookup user information in the system's db
        */
        runas_pw = getpwnam(user);
        if (!runas_pw) {
            fprintf(stderr, "ERROR: Unknown user \"%s\"\n", user);
            return -1;
        }
        runas_uid = runas_pw->pw_uid;
    }

    if (group) {
        struct group *gr = NULL;

        /*
        * Lookup group information in the system's db
        */
        gr = getgrnam(group);
        if (!gr) {
            fprintf(stderr, "ERROR: Unknown group \"%s\"\n", group);
            return -1;
        }
        runas_gid = gr->gr_gid;
    }

    if (runas_uid && getuid() == runas_uid && (!runas_gid || runas_gid == getgid())) {
        /* already running as the right user and group, nothing to do! */
        return 0;
    }

    if (runas_uid) {
#ifdef SOLARIS_PRIVILEGES
        /* request the privilege to set the UID */
        if (priv_set(PRIV_ON, PRIV_EFFECTIVE, PRIV_PROC_SETID, NULL) < 0) {
            fprintf(stderr, "ERROR: Failed to acquire proc_setid privilege (%s)\n", strerror(errno));
            return -1;
        }
#endif
#ifdef HAVE_SETGROUPS
        /*
        * Drop all group memberships prior to changing anything
        * or else we're going to inherit the parent's list of groups
        * (which is not what we want...)
        */
        if (setgroups(0, NULL) < 0) {
            fprintf(stderr, "ERROR: Failed to drop group access list\n");
            return -1;
        }
#endif
        if (runas_gid) {
            /*
            * A group has been passed, base to it
            * (without loading the user's other groups)
            */
            if (setgid(runas_gid) < 0) {
                fprintf(stderr, "ERROR: Failed to change gid!\n");
                return -1;
            }
        } else {
            /*
            * No group has been passed, use the user's primary group in this case
            */
            if (setgid(runas_pw->pw_gid) < 0) {
                fprintf(stderr, "ERROR: Failed to change gid!\n");
                return -1;
            }
#ifdef HAVE_INITGROUPS
            /*
            * Set all the other groups the user is a member of
            * (This can be really useful for fine-grained access control)
            */
            if (initgroups(runas_pw->pw_name, runas_pw->pw_gid) < 0) {
                fprintf(stderr, "ERROR: Failed to set group access list for user\n");
                return -1;
            }
#endif
        }

        /*
        * Finally drop all privileges by baseing to the new userid
        */
        if (setuid(runas_uid) < 0) {
            fprintf(stderr, "ERROR: Failed to change uid!\n");
            return -1;
        }
#ifdef HAVE_SYS_PRCTL_H
        if (prctl(PR_SET_DUMPABLE, 1) < 0) {
            fprintf(stderr, "ERROR: Failed to enable core dumps!\n");
            return -1;
        }
#endif
    }
#endif
    return 0;
}

BASE_DECLARE(void) base_core_runtime_loop(int bg)
{
#ifdef WIN32
    HANDLE shutdown_event;
    char path[256] = "";
#endif
    if (bg) {
#ifdef WIN32
        base_snprintf(path, sizeof(path), "Global\\Freebase.%d", getpid());
        shutdown_event = CreateEvent(NULL, FALSE, FALSE, path);
        if (shutdown_event) {
            WaitForSingleObject(shutdown_event, INFINITE);
        }
#else
        while (runtime.running) {
            base_yield(1000000);
        }
#endif
    } else {
        /* wait for console input */
        base_console_loop();
    }
}

BASE_DECLARE(const char *) base_core_mime_ext2type(const char *ext)
{
    if (!ext) {
        return NULL;
    }
    return (const char *) base_core_hash_find(runtime.mime_types, ext);
}

BASE_DECLARE(const char *) base_core_mime_type2ext(const char *mime)
{
    if (!mime) {
        return NULL;
    }
    return (const char *) base_core_hash_find(runtime.mime_type_exts, mime);
}

BASE_DECLARE(base_hash_index_t *) base_core_mime_index(void)
{
    return base_core_hash_first(runtime.mime_types);
}

BASE_DECLARE(base_status_t) base_core_mime_add_type(const char *type, const char *ext)
{
    char *ptype = NULL;
    char *ext_list = NULL;
    int argc = 0;
    char *argv[20] = { 0 };
    int x;
    base_status_t status = BASE_STATUS_FALSE;

    base_assert(type);
    base_assert(ext);

    ptype = base_core_permanent_strdup(type);
    ext_list = strdup(ext);

    base_assert(ext_list);

    /* Map each file extension to this MIME type if not already mapped.  Map the MIME type to the first file extension in the list if not already mapped. */
    if ((argc = base_separate_string(ext_list, ' ', argv, (sizeof(argv) / sizeof(argv[0]))))) {
        int is_mapped_type = base_core_hash_find(runtime.mime_type_exts, ptype) != NULL;
        for (x = 0; x < argc; x++) {
            if (argv[x] && ptype) {
                if (!base_core_hash_find(runtime.mime_types, ext)) {
                    base_core_hash_insert(runtime.mime_types, argv[x], ptype);
                }
                if (!is_mapped_type) {
                    base_core_hash_insert(runtime.mime_type_exts, ptype, base_core_permanent_strdup(argv[x]));
                    is_mapped_type = 1;
                }
            }
        }

        status = BASE_STATUS_SUCCESS;
    }

    free(ext_list);

    return status;
}

static void load_mime_types(void)
{
    char *cf = "mime.types";
    FILE *fd = NULL;
    char *line_buf = NULL;
    base_size_t llen = 0;
    char *mime_path = NULL;

    mime_path = base_mprintf("%s/%s", BASE_GLOBAL_dirs.conf_dir, cf);
    base_assert(mime_path);

    fd = fopen(mime_path, "rb");

    if (fd == NULL) {
        goto end;
    }

    while ((base_fp_read_dline(fd, &line_buf, &llen))) {
        char *p;
        char *type = line_buf;

        if (*line_buf == '#') {
            continue;
        }

        if ((p = strchr(line_buf, '\r')) || (p = strchr(line_buf, '\n'))) {
            *p = '\0';
        }

        if ((p = strchr(type, '\t')) || (p = strchr(type, ' '))) {
            *p++ = '\0';

            while (*p == ' ' || *p == '\t') {
                p++;
            }

            base_core_mime_add_type(type, p);
        }

    }

    base_safe_free(line_buf);
    fclose(fd);

end:

    base_safe_free(mime_path);

}

BASE_DECLARE(void) base_core_setrlimits(void)
{
#ifdef HAVE_SETRLIMIT
    struct rlimit rlp;

    /*
    Setting the stack size on FreeBSD results in an instant crash.

    If anyone knows how to fix this,
    feel free to submit a patch to https://github.com/signalwire/freebase
    */

#ifndef __FreeBSD__
    memset(&rlp, 0, sizeof(rlp));
    rlp.rlim_cur = BASE_THREAD_STACKSIZE;
    rlp.rlim_max = BASE_SYSTEM_THREAD_STACKSIZE;
    setrlimit(RLIMIT_STACK, &rlp);
#endif

    memset(&rlp, 0, sizeof(rlp));
    rlp.rlim_cur = 999999;
    rlp.rlim_max = 999999;
    setrlimit(RLIMIT_NOFILE, &rlp);

    memset(&rlp, 0, sizeof(rlp));
    rlp.rlim_cur = RLIM_INFINITY;
    rlp.rlim_max = RLIM_INFINITY;

    setrlimit(RLIMIT_CPU, &rlp);
    setrlimit(RLIMIT_DATA, &rlp);
    setrlimit(RLIMIT_FSIZE, &rlp);
#ifdef RLIMIT_NPROC
    setrlimit(RLIMIT_NPROC, &rlp);
#endif
#ifdef RLIMIT_RTPRIO
    setrlimit(RLIMIT_RTPRIO, &rlp);
#endif

#if !defined(__OpenBSD__) && !defined(__NetBSD__)
    setrlimit(RLIMIT_AS, &rlp);
#endif
#endif
    return;
}

typedef struct {
    base_memory_pool_t *pool;
    base_hash_t *hash;
} base_ip_list_t;

static base_ip_list_t IP_LIST = { 0 };

BASE_DECLARE(base_bool_t) base_check_network_list_ip_port_token(const char *ip_str, int port, const char *list_name, const char **token)
{
    base_network_list_t *list;
    ip_t  ip, mask, net;
    uint32_t bits;
    char *ipv6 = strchr(ip_str,':');
    base_bool_t ok = BASE_FALSE;
    char *ipv4 = NULL;

    if (!list_name) {
        return BASE_FALSE;
    }

    if ((ipv4 = base_network_ipv4_mapped_ipv6_addr(ip_str))) {
        ip_str = ipv4;
        ipv6 = NULL;
    }

    base_mutex_lock(runtime.global_mutex);
    if (ipv6) {
        base_inet_pton(AF_INET6, ip_str, &ip);
    } else {
        base_inet_pton(AF_INET, ip_str, &ip);
        ip.v4 = htonl(ip.v4);
    }

    if ((list = base_core_hash_find(IP_LIST.hash, list_name))) {
        if (ipv6) {
            ok = base_network_list_validate_ip6_port_token(list, ip, port, token);
        } else {
            ok = base_network_list_validate_ip_port_token(list, ip.v4, port, token);
        }
    } else if (strchr(list_name, '/')) {
        if (strchr(list_name, ',')) {
            char *list_name_dup = strdup(list_name);
            char *argv[100]; /* MAX ACL */
            int argc;

            base_assert(list_name_dup);

            if ((argc = base_separate_string(list_name_dup, ',', argv, (sizeof(argv) / sizeof(argv[0]))))) {
                int i;
                for (i = 0; i < argc; i++) {
                    base_parse_cidr(argv[i], &net, &mask, &bits);
                    if (ipv6) {
                        if ((ok = base_testv6_subnet(ip, net, mask))){
                            break;
                        }
                    } else {
                        if ((ok = base_test_subnet(ip.v4, net.v4, mask.v4))) {
                            break;
                        }
                    }
                }
            }
            free(list_name_dup);
        } else {
            base_parse_cidr(list_name, &net, &mask, &bits);

            if (ipv6) {
                ok = base_testv6_subnet(ip, net, mask);
            } else {
                ok = base_test_subnet(ip.v4, net.v4, mask.v4);
            }
        }
    }

    base_safe_free(ipv4);
    base_mutex_unlock(runtime.global_mutex);

    return ok;
}

BASE_DECLARE(base_bool_t) base_check_network_list_ip_token(const char *ip_str, const char *list_name, const char **token)
{
    return base_check_network_list_ip_port_token(ip_str, 0, list_name, token);
}

BASE_DECLARE(void) base_load_network_lists(base_bool_t reload)
{
    base_xml_t xml = NULL, x_lists = NULL, x_list = NULL, x_node = NULL, cfg = NULL;
    base_network_list_t *rfc_list, *list;
    char guess_ip[16] = "";
    int mask = 0;
    char guess_mask[16] = "";
    char *tmp_name;
    struct in_addr in;

    base_find_local_ip(guess_ip, sizeof(guess_ip), &mask, AF_INET);
    in.s_addr = mask;
    base_set_string(guess_mask, inet_ntoa(in));

    base_mutex_lock(runtime.global_mutex);

    if (IP_LIST.hash) {
        base_core_hash_destroy(&IP_LIST.hash);
    }

    if (IP_LIST.pool) {
        base_core_destroy_memory_pool(&IP_LIST.pool);
    }

    memset(&IP_LIST, 0, sizeof(IP_LIST));
    base_core_new_memory_pool(&IP_LIST.pool);
    base_core_hash_init(&IP_LIST.hash);


    tmp_name = "rfc6598.auto";
    base_network_list_create(&rfc_list, tmp_name, BASE_FALSE, IP_LIST.pool);
    base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_NOTICE, "Created ip list %s default (deny)\n", tmp_name);
    base_network_list_add_cidr(rfc_list, "100.64.0.0/10", BASE_TRUE);
    base_core_hash_insert(IP_LIST.hash, tmp_name, rfc_list);

    tmp_name = "rfc1918.auto";
    base_network_list_create(&rfc_list, tmp_name, BASE_FALSE, IP_LIST.pool);
    base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_NOTICE, "Created ip list %s default (deny)\n", tmp_name);
    base_network_list_add_cidr(rfc_list, "10.0.0.0/8", BASE_TRUE);
    base_network_list_add_cidr(rfc_list, "172.16.0.0/12", BASE_TRUE);
    base_network_list_add_cidr(rfc_list, "192.168.0.0/16", BASE_TRUE);
    base_network_list_add_cidr(rfc_list, "fe80::/10", BASE_TRUE);
    base_core_hash_insert(IP_LIST.hash, tmp_name, rfc_list);

    tmp_name = "wan.auto";
    base_network_list_create(&rfc_list, tmp_name, BASE_TRUE, IP_LIST.pool);
    base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_NOTICE, "Created ip list %s default (allow)\n", tmp_name);
    base_network_list_add_cidr(rfc_list, "0.0.0.0/8", BASE_FALSE);
    base_network_list_add_cidr(rfc_list, "10.0.0.0/8", BASE_FALSE);
    base_network_list_add_cidr(rfc_list, "172.16.0.0/12", BASE_FALSE);
    base_network_list_add_cidr(rfc_list, "192.168.0.0/16", BASE_FALSE);
    base_network_list_add_cidr(rfc_list, "169.254.0.0/16", BASE_FALSE);
    base_network_list_add_cidr(rfc_list, "100.64.0.0/10", BASE_FALSE);
    base_network_list_add_cidr(rfc_list, "fe80::/10", BASE_FALSE);
    base_core_hash_insert(IP_LIST.hash, tmp_name, rfc_list);

    tmp_name = "wan_v6.auto";
    base_network_list_create(&rfc_list, tmp_name, BASE_TRUE, IP_LIST.pool);
    base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_NOTICE, "Created ip list %s default (allow)\n", tmp_name);
    base_network_list_add_cidr(rfc_list, "0.0.0.0/0", BASE_FALSE);
    base_network_list_add_cidr(rfc_list, "fe80::/10", BASE_FALSE);
    base_core_hash_insert(IP_LIST.hash, tmp_name, rfc_list);


    tmp_name = "wan_v4.auto";
    base_network_list_create(&rfc_list, tmp_name, BASE_TRUE, IP_LIST.pool);
    base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_NOTICE, "Created ip list %s default (allow)\n", tmp_name);
    base_network_list_add_cidr(rfc_list, "0.0.0.0/8", BASE_FALSE);
    base_network_list_add_cidr(rfc_list, "10.0.0.0/8", BASE_FALSE);
    base_network_list_add_cidr(rfc_list, "172.16.0.0/12", BASE_FALSE);
    base_network_list_add_cidr(rfc_list, "192.168.0.0/16", BASE_FALSE);
    base_network_list_add_cidr(rfc_list, "169.254.0.0/16", BASE_FALSE);
    base_network_list_add_cidr(rfc_list, "100.64.0.0/10", BASE_FALSE);
    base_network_list_add_cidr(rfc_list, "::/0", BASE_FALSE);
    base_core_hash_insert(IP_LIST.hash, tmp_name, rfc_list);


    tmp_name = "any_v6.auto";
    base_network_list_create(&rfc_list, tmp_name, BASE_TRUE, IP_LIST.pool);
    base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_NOTICE, "Created ip list %s default (allow)\n", tmp_name);
    base_network_list_add_cidr(rfc_list, "0.0.0.0/0", BASE_FALSE);
    base_core_hash_insert(IP_LIST.hash, tmp_name, rfc_list);


    tmp_name = "any_v4.auto";
    base_network_list_create(&rfc_list, tmp_name, BASE_TRUE, IP_LIST.pool);
    base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_NOTICE, "Created ip list %s default (allow)\n", tmp_name);
    base_network_list_add_cidr(rfc_list, "::/0", BASE_FALSE);
    base_core_hash_insert(IP_LIST.hash, tmp_name, rfc_list);


    tmp_name = "nat.auto";
    base_network_list_create(&rfc_list, tmp_name, BASE_FALSE, IP_LIST.pool);
    base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_NOTICE, "Created ip list %s default (deny)\n", tmp_name);
    if (base_network_list_add_host_mask(rfc_list, guess_ip, guess_mask, BASE_FALSE) == BASE_STATUS_SUCCESS) {
        base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_NOTICE, "Adding %s/%s (deny) to list %s\n", guess_ip, guess_mask, tmp_name);
    }
    base_network_list_add_cidr(rfc_list, "10.0.0.0/8", BASE_TRUE);
    base_network_list_add_cidr(rfc_list, "172.16.0.0/12", BASE_TRUE);
    base_network_list_add_cidr(rfc_list, "192.168.0.0/16", BASE_TRUE);
    base_network_list_add_cidr(rfc_list, "100.64.0.0/10", BASE_TRUE);
    base_core_hash_insert(IP_LIST.hash, tmp_name, rfc_list);

    tmp_name = "loopback.auto";
    base_network_list_create(&rfc_list, tmp_name, BASE_FALSE, IP_LIST.pool);
    base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_NOTICE, "Created ip list %s default (deny)\n", tmp_name);
    base_network_list_add_cidr(rfc_list, "127.0.0.0/8", BASE_TRUE);
    base_network_list_add_cidr(rfc_list, "::1/128", BASE_TRUE);
    base_core_hash_insert(IP_LIST.hash, tmp_name, rfc_list);

    tmp_name = "localnet.auto";
    base_network_list_create(&list, tmp_name, BASE_FALSE, IP_LIST.pool);
    base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_NOTICE, "Created ip list %s default (deny)\n", tmp_name);

    if (base_network_list_add_host_mask(list, guess_ip, guess_mask, BASE_TRUE) == BASE_STATUS_SUCCESS) {
        base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_NOTICE, "Adding %s/%s (allow) to list %s\n", guess_ip, guess_mask, tmp_name);
    }
    base_core_hash_insert(IP_LIST.hash, tmp_name, list);


    if ((xml = base_xml_open_cfg("acl.conf", &cfg, NULL))) {
        if ((x_lists = base_xml_child(cfg, "network-lists"))) {
            for (x_list = base_xml_child(x_lists, "list"); x_list; x_list = x_list->next) {
                const char *name = base_xml_attr(x_list, "name");
                const char *dft = base_xml_attr(x_list, "default");
                base_bool_t default_type = BASE_TRUE;

                if (zstr(name)) {
                    continue;
                }

                if (dft) {
                    default_type = base_true(dft);
                }

                if (base_network_list_create(&list, name, default_type, IP_LIST.pool) != BASE_STATUS_SUCCESS) {
                    abort();
                }

                if (reload) {
                    base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_NOTICE, "Created ip list %s default (%s)\n", name, default_type ? "allow" : "deny");
                } else {
                    base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_CONSOLE, "Created ip list %s default (%s)\n", name, default_type ? "allow" : "deny");
                }


                for (x_node = base_xml_child(x_list, "node"); x_node; x_node = x_node->next) {
                    const char *cidr = NULL, *host = NULL, *mask = NULL, *domain = NULL, *port = NULL;
                    base_bool_t ok = default_type;
                    const char *type = base_xml_attr(x_node, "type");
                    base_network_port_range_t port_range;
                    char *argv[MAX_NETWORK_PORTS] = { 0 };
                    int argc = 0, i;

                    if (type) {
                        ok = base_true(type);
                    }

                    cidr = base_xml_attr(x_node, "cidr");
                    host = base_xml_attr(x_node, "host");
                    mask = base_xml_attr(x_node, "mask");
                    domain = base_xml_attr(x_node, "domain");

                    memset(&port_range, 0, sizeof(base_network_port_range_t));

                    if( (port = base_xml_attr(x_node, "port")) != NULL) {
                        port_range.port = atoi(port);
                    }

                    if( (port = base_xml_attr(x_node, "ports")) != NULL) {
                        argc = base_separate_string((char*)port, ',', argv, (sizeof(argv) / sizeof(argv[0])));
                        for(i=0; i < argc; i++) {
                            port_range.ports[i] = atoi(argv[i]);
                        }
                    }
                    if( (port = base_xml_attr(x_node, "port-min")) != NULL) {
                        port_range.min_port = atoi(port);
                    }
                    if( (port = base_xml_attr(x_node, "port-max")) != NULL) {
                        port_range.max_port = atoi(port);
                    }

                    if (domain) {
                        base_event_t *my_params = NULL;
                        base_xml_t x_domain, xml_root;
                        base_xml_t gt, gts, ut, uts;

                        base_event_create(&my_params, BASE_EVENT_GENERAL);
                        base_assert(my_params);
                        base_event_add_header_string(my_params, BASE_STACK_BOTTOM, "domain", domain);
                        base_event_add_header_string(my_params, BASE_STACK_BOTTOM, "purpose", "network-list");

                        if (base_xml_locate_domain(domain, my_params, &xml_root, &x_domain) != BASE_STATUS_SUCCESS) {
                            base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_WARNING, "Cannot locate domain %s\n", domain);
                            base_event_destroy(&my_params);
                            continue;
                        }

                        base_event_destroy(&my_params);

                        if ((ut = base_xml_child(x_domain, "users"))) {
                            x_domain = ut;
                        }

                        for (ut = base_xml_child(x_domain, "user"); ut; ut = ut->next) {
                            const char *user_cidr = base_xml_attr(ut, "cidr");
                            const char *id = base_xml_attr(ut, "id");

                            if (id && user_cidr) {
                                char *token = base_mprintf("%s@%s", id, domain);
                                base_assert(token);
                                base_network_list_add_cidr_token(list, user_cidr, ok, token);
                                free(token);
                            }
                        }

                        for (gts = base_xml_child(x_domain, "groups"); gts; gts = gts->next) {
                            for (gt = base_xml_child(gts, "group"); gt; gt = gt->next) {
                                for (uts = base_xml_child(gt, "users"); uts; uts = uts->next) {
                                    for (ut = base_xml_child(uts, "user"); ut; ut = ut->next) {
                                        const char *user_cidr = base_xml_attr(ut, "cidr");
                                        const char *id = base_xml_attr(ut, "id");

                                        if (id && user_cidr) {
                                            char *token = base_mprintf("%s@%s", id, domain);
                                            base_assert(token);
                                            base_network_list_add_cidr_port_token(list, user_cidr, ok, token, &port_range);
                                            free(token);
                                        }
                                    }
                                }
                            }
                        }

                        base_xml_free(xml_root);
                    } else if (cidr) {
                        base_network_list_add_cidr_port_token(list, cidr, ok, NULL, &port_range);
                    } else if (host && mask) {
                        base_network_list_add_host_port_mask(list, host, mask, ok, &port_range);
                    }
                }

                base_core_hash_insert(IP_LIST.hash, name, list);
            }
        }

        base_xml_free(xml);
    }

    base_mutex_unlock(runtime.global_mutex);
}

BASE_DECLARE(uint32_t) base_core_max_dtmf_duration(uint32_t duration)
{
    if (duration) {
        if (duration > BASE_MAX_DTMF_DURATION) {
            duration = BASE_MAX_DTMF_DURATION;
        }
        if (duration < BASE_MIN_DTMF_DURATION) {
            duration = BASE_MIN_DTMF_DURATION;
        }
        runtime.max_dtmf_duration = duration;
        if (duration < runtime.min_dtmf_duration) {
            runtime.min_dtmf_duration = duration;
        }
    }
    return runtime.max_dtmf_duration;
}

BASE_DECLARE(uint32_t) base_core_default_dtmf_duration(uint32_t duration)
{
    if (duration) {
        if (duration < BASE_MIN_DTMF_DURATION) {
            duration = BASE_MIN_DTMF_DURATION;
        }
        if (duration > BASE_MAX_DTMF_DURATION) {
            duration = BASE_MAX_DTMF_DURATION;
        }
        runtime.default_dtmf_duration = duration;

        if (duration < runtime.min_dtmf_duration) {
            runtime.min_dtmf_duration = duration;
        }

        if (duration > runtime.max_dtmf_duration) {
            runtime.max_dtmf_duration = duration;
        }

    }
    return runtime.default_dtmf_duration;
}

BASE_DECLARE(uint32_t) base_core_min_dtmf_duration(uint32_t duration)
{
    if (duration) {
        if (duration < BASE_MIN_DTMF_DURATION) {
            duration = BASE_MIN_DTMF_DURATION;
        }
        if (duration > BASE_MAX_DTMF_DURATION) {
            duration = BASE_MAX_DTMF_DURATION;
        }

        runtime.min_dtmf_duration = duration;

        if (duration > runtime.max_dtmf_duration) {
            runtime.max_dtmf_duration = duration;
        }
    }
    return runtime.min_dtmf_duration;
}

BASE_DECLARE(base_status_t) base_core_thread_set_cpu_affinity(int cpu)
{
    base_status_t status = BASE_STATUS_FALSE;

    if (cpu > -1) {

#ifdef HAVE_CPU_SET_MACROS
        cpu_set_t set;

        CPU_ZERO(&set);
        CPU_SET(cpu, &set);

        if (!sched_setaffinity(0, sizeof(set), &set)) {
            status = BASE_STATUS_SUCCESS;
        }

#else
#if WIN32
        if (SetThreadAffinityMask(GetCurrentThread(), (DWORD_PTR) cpu)) {
            status = BASE_STATUS_SUCCESS;
        }
#endif
#endif
    }

    return status;
}


BASE_DECLARE(int) base_core_test_flag(int flag)
{
    return base_test_flag((&runtime), flag);
}


BASE_DECLARE(base_status_t) base_core_init(base_core_flag_t flags, base_bool_t console, const char **err)
{
    base_uuid_t uuid;
    char guess_ip[256];
    int mask = 0;
    struct in_addr in;


    if (runtime.runlevel > 0) {
        /* one per customer */
        return BASE_STATUS_SUCCESS;
    }

    memset(&runtime, 0, sizeof(runtime));
    gethostname(runtime.hostname, sizeof(runtime.hostname));

    runtime.shutdown_cause = BASE_CAUSE_SYSTEM_SHUTDOWN;
    runtime.max_db_handles = 50;
    runtime.db_handle_timeout = 5000000;
    runtime.event_heartbeat_interval = 20;

    runtime.runlevel++;
    runtime.dummy_cng_frame.data = runtime.dummy_data;
    runtime.dummy_cng_frame.datalen = sizeof(runtime.dummy_data);
    runtime.dummy_cng_frame.buflen = sizeof(runtime.dummy_data);
    runtime.dbname = "core";
    base_set_flag((&runtime.dummy_cng_frame), SFF_CNG);
    base_set_flag((&runtime), SCF_AUTO_SCHEMAS);
    base_set_flag((&runtime), SCF_CLEAR_SQL);
    base_set_flag((&runtime), SCF_API_EXPANSION);
    base_set_flag((&runtime), SCF_SESSION_THREAD_POOL);
#ifdef WIN32
    base_set_flag((&runtime), SCF_THREADED_SYSTEM_EXEC);
#endif
    base_set_flag((&runtime), SCF_NO_NEW_SESSIONS);
    if (flags & SCF_LOG_DISABLE) {
        runtime.hard_log_level = BASE_LOG_DISABLE;
        flags &= ~SCF_LOG_DISABLE;
    } else {
        runtime.hard_log_level = BASE_LOG_DEBUG;
    }
    runtime.mailer_app = "sendmail";
    runtime.mailer_app_args = "-t";
    runtime.max_dtmf_duration = BASE_MAX_DTMF_DURATION;
    runtime.default_dtmf_duration = BASE_DEFAULT_DTMF_DURATION;
    runtime.min_dtmf_duration = BASE_MIN_DTMF_DURATION;
    runtime.odbc_dbtype = DBTYPE_DEFAULT;
    runtime.dbname = NULL;
#ifndef WIN32
    runtime.cpu_count = sysconf (_SC_NPROCESSORS_ONLN);
#else
    {
        SYSTEM_INFO sysinfo;
        GetSystemInfo( &sysinfo );
        runtime.cpu_count = sysinfo.dwNumberOfProcessors;
    }
#endif

    if (!runtime.cpu_count) runtime.cpu_count = 1;

    if (sqlite3_initialize() != SQLITE_OK) {
        *err = "FATAL ERROR! Could not initialize SQLite\n";
        return BASE_STATUS_MEMERR;
    }

    /* INIT APR and Create the pool context */
    if (fspr_initialize() != BASE_STATUS_SUCCESS) {
        *err = "FATAL ERROR! Could not initialize APR\n";
        return BASE_STATUS_MEMERR;
    }

    if (!(runtime.memory_pool = base_core_memory_init())) {
        *err = "FATAL ERROR! Could not allocate memory pool\n";
        return BASE_STATUS_MEMERR;
    }
    base_assert(runtime.memory_pool != NULL);

    base_dir_make_recursive(BASE_GLOBAL_dirs.base_dir, BASE_DEFAULT_DIR_PERMS, runtime.memory_pool);
    base_dir_make_recursive(BASE_GLOBAL_dirs.mod_dir, BASE_DEFAULT_DIR_PERMS, runtime.memory_pool);
    base_dir_make_recursive(BASE_GLOBAL_dirs.conf_dir, BASE_DEFAULT_DIR_PERMS, runtime.memory_pool);
    base_dir_make_recursive(BASE_GLOBAL_dirs.log_dir, BASE_DEFAULT_DIR_PERMS, runtime.memory_pool);
    base_dir_make_recursive(BASE_GLOBAL_dirs.run_dir, BASE_DEFAULT_DIR_PERMS, runtime.memory_pool);
    base_dir_make_recursive(BASE_GLOBAL_dirs.db_dir, BASE_DEFAULT_DIR_PERMS, runtime.memory_pool);
    base_dir_make_recursive(BASE_GLOBAL_dirs.script_dir, BASE_DEFAULT_DIR_PERMS, runtime.memory_pool);
    base_dir_make_recursive(BASE_GLOBAL_dirs.htdocs_dir, BASE_DEFAULT_DIR_PERMS, runtime.memory_pool);
    base_dir_make_recursive(BASE_GLOBAL_dirs.grammar_dir, BASE_DEFAULT_DIR_PERMS, runtime.memory_pool);
    base_dir_make_recursive(BASE_GLOBAL_dirs.fonts_dir, BASE_DEFAULT_DIR_PERMS, runtime.memory_pool);
    base_dir_make_recursive(BASE_GLOBAL_dirs.images_dir, BASE_DEFAULT_DIR_PERMS, runtime.memory_pool);
    base_dir_make_recursive(BASE_GLOBAL_dirs.recordings_dir, BASE_DEFAULT_DIR_PERMS, runtime.memory_pool);
    base_dir_make_recursive(BASE_GLOBAL_dirs.sounds_dir, BASE_DEFAULT_DIR_PERMS, runtime.memory_pool);
    base_dir_make_recursive(BASE_GLOBAL_dirs.temp_dir, BASE_DEFAULT_DIR_PERMS, runtime.memory_pool);
    base_dir_make_recursive(BASE_GLOBAL_dirs.certs_dir, BASE_DEFAULT_DIR_PERMS, runtime.memory_pool);

    base_mutex_init(&runtime.uuid_mutex, BASE_MUTEX_NESTED, runtime.memory_pool);

    base_mutex_init(&runtime.throttle_mutex, BASE_MUTEX_NESTED, runtime.memory_pool);

    base_mutex_init(&runtime.session_hash_mutex, BASE_MUTEX_NESTED, runtime.memory_pool);
    base_mutex_init(&runtime.global_mutex, BASE_MUTEX_NESTED, runtime.memory_pool);

    base_thread_rwlock_create(&runtime.global_var_rwlock, runtime.memory_pool);
    base_core_set_globals();
    base_core_session_init(runtime.memory_pool);
    base_event_create_plain(&runtime.global_vars, BASE_EVENT_CHANNEL_DATA);
    base_core_hash_init_case(&runtime.mime_types, BASE_FALSE);
    base_core_hash_init_case(&runtime.mime_type_exts, BASE_FALSE);
    base_core_hash_init_case(&runtime.ptimes, BASE_FALSE);
    load_mime_types();
    runtime.flags |= flags;
    runtime.sps_total = 30;

    *err = NULL;

    if (console) {
        runtime.console = stdout;
    }

    SSL_library_init();
    base_ssl_init_ssl_locks();
    OpenSSL_add_all_algorithms();
    base_curl_init();

    base_core_set_variable("hostname", runtime.hostname);
    base_find_local_ip(guess_ip, sizeof(guess_ip), &mask, AF_INET);
    base_core_set_variable("local_ip_v4", guess_ip);
    in.s_addr = mask;
    base_core_set_variable("local_mask_v4", inet_ntoa(in));


    base_find_local_ip(guess_ip, sizeof(guess_ip), NULL, AF_INET6);
    base_core_set_variable("local_ip_v6", guess_ip);
    base_core_set_variable("base_dir", BASE_GLOBAL_dirs.base_dir);
    base_core_set_variable("recordings_dir", BASE_GLOBAL_dirs.recordings_dir);
    base_core_set_variable("sound_prefix", BASE_GLOBAL_dirs.sounds_dir);
    base_core_set_variable("sounds_dir", BASE_GLOBAL_dirs.sounds_dir);
    base_core_set_variable("conf_dir", BASE_GLOBAL_dirs.conf_dir);
    base_core_set_variable("log_dir", BASE_GLOBAL_dirs.log_dir);
    base_core_set_variable("run_dir", BASE_GLOBAL_dirs.run_dir);
    base_core_set_variable("db_dir", BASE_GLOBAL_dirs.db_dir);
    base_core_set_variable("mod_dir", BASE_GLOBAL_dirs.mod_dir);
    base_core_set_variable("htdocs_dir", BASE_GLOBAL_dirs.htdocs_dir);
    base_core_set_variable("script_dir", BASE_GLOBAL_dirs.script_dir);
    base_core_set_variable("temp_dir", BASE_GLOBAL_dirs.temp_dir);
    base_core_set_variable("grammar_dir", BASE_GLOBAL_dirs.grammar_dir);
    base_core_set_variable("fonts_dir", BASE_GLOBAL_dirs.fonts_dir);
    base_core_set_variable("images_dir", BASE_GLOBAL_dirs.images_dir);
    base_core_set_variable("certs_dir", BASE_GLOBAL_dirs.certs_dir);
    base_core_set_variable("storage_dir", BASE_GLOBAL_dirs.storage_dir);
    base_core_set_variable("cache_dir", BASE_GLOBAL_dirs.cache_dir);
    base_core_set_variable("data_dir", BASE_GLOBAL_dirs.data_dir);
    base_core_set_variable("localstate_dir", BASE_GLOBAL_dirs.localstate_dir);
    base_console_init(runtime.memory_pool);
    base_event_init(runtime.memory_pool);
    base_channel_global_init(runtime.memory_pool);

    if (base_xml_init(runtime.memory_pool, err) != BASE_STATUS_SUCCESS) {
        /* allow missing configuration if MINIMAL */
        if (!(flags & SCF_MINIMAL)) {
            fspr_terminate();
            return BASE_STATUS_MEMERR;
        }
    }

    if (base_test_flag((&runtime), SCF_USE_AUTO_NAT)) {
        base_nat_init(runtime.memory_pool, base_test_flag((&runtime), SCF_USE_NAT_MAPPING));
    }

    base_log_init(runtime.memory_pool, runtime.colorize_console);

    runtime.tipping_point = 0;
    runtime.timer_affinity = -1;
    runtime.microseconds_per_tick = 20000;

    if (flags & SCF_MINIMAL) return BASE_STATUS_SUCCESS;

    base_load_core_config("base.conf");

    base_core_state_machine_init(runtime.memory_pool);

    base_core_media_init();
    base_scheduler_task_thread_start();

    base_nat_late_init();

    base_rtp_init(runtime.memory_pool);

    runtime.running = 1;
    runtime.initiated = base_mono_micro_time_now();

    base_scheduler_add_task(base_epoch_time_now(NULL), heartbeat_callback, "heartbeat", "core", 0, NULL, SSHF_NONE | SSHF_NO_DEL);

    base_scheduler_add_task(base_epoch_time_now(NULL), check_ip_callback, "check_ip", "core", 0, NULL, SSHF_NONE | SSHF_NO_DEL | SSHF_OWN_THREAD);

    base_uuid_get(&uuid);
    base_uuid_format(runtime.uuid_str, &uuid);
    base_core_set_variable("core_uuid", runtime.uuid_str);


    return BASE_STATUS_SUCCESS;
}


#ifdef TRAP_BUS
static void handle_SIGBUS(int sig)
{
    base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_DEBUG1, "Sig BUS!\n");
    return;
}
#endif

static void handle_SIGHUP(int sig)
{
    if (sig) {
        base_event_t *event;

        if (base_event_create(&event, BASE_EVENT_TRAP) == BASE_STATUS_SUCCESS) {
            base_event_add_header(event, BASE_STACK_BOTTOM, "Trapped-Signal", "HUP");
            base_event_fire(&event);
        }
    }
    return;
}


BASE_DECLARE(uint32_t) base_default_ptime(const char *name, uint32_t number)
{
    uint32_t *p;

    if ((p = base_core_hash_find(runtime.ptimes, name))) {
        return *p;
    }

    return 20;
}

BASE_DECLARE(uint32_t) base_default_rate(const char *name, uint32_t number)
{

    if (!strcasecmp(name, "opus")) {
        return 48000;
    } else if (!strncasecmp(name, "h26", 3)) { // h26x
        return 90000;
    } else if (!strncasecmp(name, "vp", 2)) {  // vp8, vp9
        return 90000;
    }

    return 8000;
}

static uint32_t d_30 = 30;

static void base_load_core_config(const char *file)
{
    base_xml_t xml = NULL, cfg = NULL;

    base_core_hash_insert(runtime.ptimes, "ilbc", &d_30);
    base_core_hash_insert(runtime.ptimes, "isac", &d_30);
    base_core_hash_insert(runtime.ptimes, "G723", &d_30);


    if ((xml = base_xml_open_cfg(file, &cfg, NULL))) {
        base_xml_t settings, param;

        if ((settings = base_xml_child(cfg, "default-ptimes"))) {
            for (param = base_xml_child(settings, "codec"); param; param = param->next) {
                const char *var = base_xml_attr_soft(param, "name");
                const char *val = base_xml_attr_soft(param, "ptime");

                if (!zstr(var) && !zstr(val)) {
                    uint32_t *p;
                    uint32_t v = base_atoul(val);

                    if (!strcasecmp(var, "G723") || !strcasecmp(var, "iLBC")) {
                        base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_CRIT, "Error adding %s, defaults cannot be changed\n", var);
                        continue;
                    }

                    if (v == 0) {
                        base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_CRIT, "Error adding %s, invalid ptime\n", var);
                        continue;
                    }

                    p = base_core_alloc(runtime.memory_pool, sizeof(*p));
                    *p = v;
                    base_core_hash_insert(runtime.ptimes, var, p);
                }

            }
        }

        if ((settings = base_xml_child(cfg, "settings"))) {
            for (param = base_xml_child(settings, "param"); param; param = param->next) {
                const char *var = base_xml_attr_soft(param, "name");
                const char *val = base_xml_attr_soft(param, "value");

                if (!strcasecmp(var, "loglevel")) {
                    int level;
                    if (*val > 47 && *val < 58) {
                        level = atoi(val);
                    } else {
                        level = base_log_str2level(val);
                    }

                    if (level != BASE_LOG_INVALID) {
                        base_core_session_ctl(SCSC_LOGLEVEL, &level);
                    }
#ifdef HAVE_SETRLIMIT
                } else if (!strcasecmp(var, "dump-cores") && base_true(val)) {
                    struct rlimit rlp;
                    memset(&rlp, 0, sizeof(rlp));
                    rlp.rlim_cur = RLIM_INFINITY;
                    rlp.rlim_max = RLIM_INFINITY;
                    setrlimit(RLIMIT_CORE, &rlp);
#endif
                } else if (!strcasecmp(var, "debug-level")) {
                    int tmp = atoi(val);
                    if (tmp > -1 && tmp < 11) {
                        base_core_session_ctl(SCSC_DEBUG_LEVEL, &tmp);
                    }
                } else if (!strcasecmp(var, "max-db-handles")) {
                    long tmp = atol(val);

                    if (tmp > 4 && tmp < 5001) {
                        runtime.max_db_handles = (uint32_t) tmp;
                    } else {
                        base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_ERROR, "max-db-handles must be between 5 and 5000\n");
                    }
                } else if (!strcasecmp(var, "odbc-skip-autocommit-flip")) {
                    if (base_true(val)) {
                        base_odbc_skip_autocommit_flip();
                    }
                } else if (!strcasecmp(var, "db-handle-timeout")) {
                    long tmp = atol(val);

                    if (tmp > 0 && tmp < 5001) {
                        runtime.db_handle_timeout = (uint32_t) tmp * 1000000;
                    } else {
                        base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_ERROR, "db-handle-timeout must be between 1 and 5000\n");
                    }

                } else if (!strcasecmp(var, "event-heartbeat-interval")) {
                    long tmp = atol(val);

                    if (tmp > 0) {
                        runtime.event_heartbeat_interval = (uint32_t) tmp;
                    } else {
                        base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_ERROR, "heartbeat-interval must be a greater than 0\n");
                    }

                } else if (!strcasecmp(var, "multiple-registrations")) {
                    runtime.multiple_registrations = base_true(val);
                } else if (!strcasecmp(var, "auto-create-schemas")) {
                    if (base_true(val)) {
                        base_set_flag((&runtime), SCF_AUTO_SCHEMAS);
                    } else {
                        base_clear_flag((&runtime), SCF_AUTO_SCHEMAS);
                    }
                } else if (!strcasecmp(var, "session-thread-pool")) {
                    if (base_true(val)) {
                        base_set_flag((&runtime), SCF_SESSION_THREAD_POOL);
                    } else {
                        base_clear_flag((&runtime), SCF_SESSION_THREAD_POOL);
                    }
                } else if (!strcasecmp(var, "auto-clear-sql")) {
                    if (base_true(val)) {
                        base_set_flag((&runtime), SCF_CLEAR_SQL);
                    } else {
                        base_clear_flag((&runtime), SCF_CLEAR_SQL);
                    }
                } else if (!strcasecmp(var, "api-expansion")) {
                    if (base_true(val)) {
                        base_set_flag((&runtime), SCF_API_EXPANSION);
                    } else {
                        base_clear_flag((&runtime), SCF_API_EXPANSION);
                    }
                } else if (!strcasecmp(var, "enable-early-hangup") && base_true(val)) {
                    base_set_flag((&runtime), SCF_EARLY_HANGUP);
                } else if (!strcasecmp(var, "colorize-console") && base_true(val)) {
                    runtime.colorize_console = BASE_TRUE;
                } else if (!strcasecmp(var, "core-db-pre-trans-execute") && !zstr(val)) {
                    runtime.core_db_pre_trans_execute = base_core_strdup(runtime.memory_pool, val);
                } else if (!strcasecmp(var, "core-db-post-trans-execute") && !zstr(val)) {
                    runtime.core_db_post_trans_execute = base_core_strdup(runtime.memory_pool, val);
                } else if (!strcasecmp(var, "core-db-inner-pre-trans-execute") && !zstr(val)) {
                    runtime.core_db_inner_pre_trans_execute = base_core_strdup(runtime.memory_pool, val);
                } else if (!strcasecmp(var, "core-db-inner-post-trans-execute") && !zstr(val)) {
                    runtime.core_db_inner_post_trans_execute = base_core_strdup(runtime.memory_pool, val);
                } else if (!strcasecmp(var, "dialplan-timestamps")) {
                    if (base_true(val)) {
                        base_set_flag((&runtime), SCF_DIALPLAN_TIMESTAMPS);
                    } else {
                        base_clear_flag((&runtime), SCF_DIALPLAN_TIMESTAMPS);
                    }
                } else if (!strcasecmp(var, "mailer-app") && !zstr(val)) {
                    runtime.mailer_app = base_core_strdup(runtime.memory_pool, val);
                } else if (!strcasecmp(var, "mailer-app-args") && val) {
                    runtime.mailer_app_args = base_core_strdup(runtime.memory_pool, val);
                } else if (!strcasecmp(var, "sessions-per-second") && !zstr(val)) {
                    base_core_sessions_per_second(atoi(val));
                } else if (!strcasecmp(var, "max-dtmf-duration") && !zstr(val)) {
                    int tmp = atoi(val);
                    if (tmp > 0) {
                        base_core_max_dtmf_duration((uint32_t) tmp);
                    }
                } else if (!strcasecmp(var, "min-dtmf-duration") && !zstr(val)) {
                    int tmp = atoi(val);
                    if (tmp > 0) {
                        base_core_min_dtmf_duration((uint32_t) tmp);
                    }
                } else if (!strcasecmp(var, "default-dtmf-duration") && !zstr(val)) {
                    int tmp = atoi(val);
                    if (tmp > 0) {
                        base_core_default_dtmf_duration((uint32_t) tmp);
                    }
                } else if (!strcasecmp(var, "enable-use-system-time")) {
                    base_time_set_use_system_time(base_true(val));
                } else if (!strcasecmp(var, "enable-monotonic-timing")) {
                    base_time_set_monotonic(base_true(val));
                } else if (!strcasecmp(var, "enable-softtimer-timerfd")) {
                    int ival = 0;
                    if (val) {
                        if (base_true(val)) {
                            ival = 2;
                        } else {
                            if (strcasecmp(val, "broadcast")) {
                                ival = 1;
                            } else if (strcasecmp(val, "fd-per-timer")) {
                                ival = 2;
                            }
                        }
                    }
                    base_time_set_timerfd(ival);
                } else if (!strcasecmp(var, "enable-clock-nanosleep")) {
                    base_time_set_nanosleep(base_true(val));
                } else if (!strcasecmp(var, "enable-cond-yield")) {
                    base_time_set_cond_yield(base_true(val));
                } else if (!strcasecmp(var, "enable-timer-matrix")) {
                    base_time_set_matrix(base_true(val));
                } else if (!strcasecmp(var, "max-sessions") && !zstr(val)) {
                    base_core_session_limit(atoi(val));
                } else if (!strcasecmp(var, "verbose-channel-events") && !zstr(val)) {
                    int v = base_true(val);
                    if (v) {
                        base_set_flag((&runtime), SCF_VERBOSE_EVENTS);
                    } else {
                        base_clear_flag((&runtime), SCF_VERBOSE_EVENTS);
                    }
                } else if (!strcasecmp(var, "threaded-system-exec") && !zstr(val)) {
#ifdef WIN32
                    base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_WARNING, "threaded-system-exec is not implemented on this platform\n");
#else
                    int v = base_true(val);
                    if (v) {
                        base_set_flag((&runtime), SCF_THREADED_SYSTEM_EXEC);
                    } else {
                        base_clear_flag((&runtime), SCF_THREADED_SYSTEM_EXEC);
                    }
#endif
                } else if (!strcasecmp(var, "spawn-instead-of-system") && !zstr(val)) {
#ifdef WIN32
                    base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_WARNING, "spawn-instead-of-system is not implemented on this platform\n");
#else
                    int v = base_true(val);
                    if (v) {
                        base_core_set_variable("spawn_instead_of_system", "true");
                    } else {
                        base_core_set_variable("spawn_instead_of_system", "false");
                    }
#endif
                } else if (!strcasecmp(var, "exclude-error-log-from-xml-cdr") && !zstr(val)) {
                    int v = base_true(val);
                    if (v) {
                        base_core_set_variable("exclude_error_log_from_xml_cdr", "true");
                    } else {
                        base_core_set_variable("exclude_error_log_from_xml_cdr", "false");
                    }
                } else if (!strcasecmp(var, "min-idle-cpu") && !zstr(val)) {
                    base_core_min_idle_cpu(atof(val));
                } else if (!strcasecmp(var, "tipping-point") && !zstr(val)) {
                    runtime.tipping_point = atoi(val);
                } else if (!strcasecmp(var, "cpu-idle-smoothing-depth") && !zstr(val)) {
                    runtime.cpu_idle_smoothing_depth = atoi(val);
                } else if (!strcasecmp(var, "events-use-dispatch") && !zstr(val)) {
                    runtime.events_use_dispatch = base_true(val);
                } else if (!strcasecmp(var, "initial-event-threads") && !zstr(val)) {
                    int tmp;

                    if (!runtime.events_use_dispatch) {
                        runtime.events_use_dispatch = 1;
                        base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_WARNING,
                            "Implicitly setting events-use-dispatch based on usage of this initial-event-threads parameter.\n");
                    }

                    tmp = atoi(val);

                    if (tmp > runtime.cpu_count / 2) {
                        tmp = runtime.cpu_count / 2;
                        base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_WARNING, "This value cannot be higher than %d so setting it to that value\n",
                            runtime.cpu_count / 2);
                    }

                    if (tmp < 1) {
                        tmp = 1;
                        base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_WARNING, "This value cannot be lower than 1 so setting it to that level\n");
                    }

                    base_event_launch_dispatch_threads(tmp);

                } else if (!strcasecmp(var, "1ms-timer") && base_true(val)) {
                    runtime.microseconds_per_tick = 1000;
                } else if (!strcasecmp(var, "timer-affinity") && !zstr(val)) {
                    if (!strcasecmp(val, "disabled")) {
                        runtime.timer_affinity = -1;
                    } else {
                        runtime.timer_affinity = atoi(val);
                    }
                } else if (!strcasecmp(var, "ice-resolve-candidate")) {
                    base_core_media_set_resolveice(base_true(val));
                } else if (!strcasecmp(var, "rtp-start-port") && !zstr(val)) {
                    base_rtp_set_start_port((base_port_t) atoi(val));
                } else if (!strcasecmp(var, "rtp-end-port") && !zstr(val)) {
                    base_rtp_set_end_port((base_port_t) atoi(val));
                } else if (!strcasecmp(var, "rtp-port-usage-robustness") && base_true(val)) {
                    runtime.port_alloc_flags |= SPF_ROBUST_UDP;
                } else if (!strcasecmp(var, "core-db-name") && !zstr(val)) {
                    runtime.dbname = base_core_strdup(runtime.memory_pool, val);
                } else if (!strcasecmp(var, "core-db-dsn") && !zstr(val)) {
                    runtime.odbc_dsn = base_core_strdup(runtime.memory_pool, val);
                } else if (!strcasecmp(var, "core-non-sqlite-db-required") && !zstr(val)) {
                    base_set_flag((&runtime), SCF_CORE_NON_SQLITE_DB_REQ);
                } else if (!strcasecmp(var, "core-dbtype") && !zstr(val)) {
                    if (!strcasecmp(val, "MSSQL")) {
                        runtime.odbc_dbtype = DBTYPE_MSSQL;
                    } else {
                        runtime.odbc_dbtype = DBTYPE_DEFAULT;
                    }
                } else if (!strcasecmp(var, "basename") && !zstr(val)) {
                    runtime.basename = base_core_strdup(runtime.memory_pool, val);
                    base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_NOTICE, "Set basename to %s\n", runtime.basename);
                } else if (!strcasecmp(var, "rtp-retain-crypto-keys")) {
                    if (base_true(val)) {
                        base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_WARNING,
                            "rtp-retain-crypto-keys enabled. Could be used to decrypt secure media.\n");
                    }
                    base_core_set_variable("rtp_retain_crypto_keys", val);
                } else if (!strcasecmp(var, "caller-profile-soft-variables-uses-prefix") && !zstr(val)) {
                    int v = base_true(val);
                    if (v) {
                        base_set_flag((&runtime), SCF_CPF_SOFT_PREFIX);
                    } else {
                        base_clear_flag((&runtime), SCF_CPF_SOFT_PREFIX);
                    }
                } else if (!strcasecmp(var, "caller-profile-soft-lookup-values") && !zstr(val)) {
                    int v = base_true(val);
                    if (v) {
                        base_set_flag((&runtime), SCF_CPF_SOFT_LOOKUP);
                    } else {
                        base_clear_flag((&runtime), SCF_CPF_SOFT_LOOKUP);
                    }
                } else if (!strcasecmp(var, "event-channel-key-separator") && !zstr(val)) {
                    runtime.event_channel_key_separator = base_core_strdup(runtime.memory_pool, val);
                } else if (!strcasecmp(var, "event-channel-enable-hierarchy-deliver") && !zstr(val)) {
                    int v = base_true(val);
                    if (v) {
                        base_set_flag((&runtime), SCF_EVENT_CHANNEL_ENABLE_HIERARCHY_DELIVERY);
                    } else {
                        base_clear_flag((&runtime), SCF_EVENT_CHANNEL_ENABLE_HIERARCHY_DELIVERY);
                    }
                } else if (!strcasecmp(var, "event-channel-hierarchy-deliver-once") && !zstr(val)) {
                    int v = base_true(val);
                    if (v) {
                        base_set_flag((&runtime), SCF_EVENT_CHANNEL_HIERARCHY_DELIVERY_ONCE);
                    } else {
                        base_clear_flag((&runtime), SCF_EVENT_CHANNEL_HIERARCHY_DELIVERY_ONCE);
                    }
                } else if (!strcasecmp(var, "event-channel-log-undeliverable-json") && !zstr(val)) {
                    int v = base_true(val);
                    if (v) {
                        base_set_flag((&runtime), SCF_EVENT_CHANNEL_LOG_UNDELIVERABLE_JSON);
                    } else {
                        base_clear_flag((&runtime), SCF_EVENT_CHANNEL_LOG_UNDELIVERABLE_JSON);
                    }
                } else if (!strcasecmp(var, "max-audio-channels") && !zstr(val)) {
                    base_core_max_audio_channels(atoi(val));
                }
            }
        }

        if (runtime.event_channel_key_separator == NULL) {
            runtime.event_channel_key_separator = base_core_strdup(runtime.memory_pool, ".");
        }

        if ((settings = base_xml_child(cfg, "variables"))) {
            for (param = base_xml_child(settings, "variable"); param; param = param->next) {
                const char *var = base_xml_attr_soft(param, "name");
                const char *val = base_xml_attr_soft(param, "value");
                if (var && val) {
                    base_core_set_variable(var, val);
                }
            }
        }

        base_xml_free(xml);
    }


}

BASE_DECLARE(const char *) base_core_banner(void)
{

    return ("\n"
        ".=============================================================.\n"
        "|   _____              ______        _____ _____ ____ _   _   |\n"
        "|  |  ___| __ ___  ___/ ___\\ \\      / /_ _|_   _/ ___| | | |  |\n"
        "|  | |_ | '__/ _ \\/ _ \\___ \\\\ \\ /\\ / / | |  | || |   | |_| |  |\n"
        "|  |  _|| | |  __/  __/___) |\\ V  V /  | |  | || |___|  _  |  |\n"
        "|  |_|  |_|  \\___|\\___|____/  \\_/\\_/  |___| |_| \\____|_| |_|  |\n"
        "|                                                             |\n"
        ".=============================================================."
        "\n"

        "|   Anthony Minessale II, Michael Jerris, Brian West, Others  |\n"
        "|   FreeBASE (http://www.freebase.org)                    |\n"
        "|   Paypal Donations Appreciated: paypal@freebase.org       |\n"
        "|   Brought to you by ClueCon http://www.cluecon.com/         |\n"
        ".=============================================================.\n"
        "\n");
}

base_status_t base_core_sqldb_init(const char **err)
{
    if (base_core_check_core_db_dsn() != BASE_STATUS_SUCCESS) {
        *err = "NO SUITABLE DATABASE INTERFACE IS AVAILABLE TO SERVE 'core-db-dsn'!\n";
        return BASE_STATUS_GENERR;
    }

    if (base_core_sqldb_start(runtime.memory_pool, base_test_flag((&runtime), SCF_USE_SQL) ? BASE_TRUE : BASE_FALSE) != BASE_STATUS_SUCCESS) {
        *err = "Error activating database";
        return BASE_STATUS_GENERR;
    }

    return BASE_STATUS_SUCCESS;
}

BASE_DECLARE(base_status_t) base_core_init_and_modload(base_core_flag_t flags, base_bool_t console, const char **err)
{
    base_event_t *event;
    char *cmd;
    int x = 0;
    const char *use = NULL;
#include "cc.h"


    if (base_core_init(flags, console, err) != BASE_STATUS_SUCCESS) {
        return BASE_STATUS_GENERR;
    }

    if (runtime.runlevel > 1) {
        /* one per customer */
        return BASE_STATUS_SUCCESS;
    }

    runtime.runlevel++;
    runtime.events_use_dispatch = 1;

    base_core_set_signal_handlers();
    base_load_network_lists(BASE_FALSE);

    base_msrp_init();

    base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_CONSOLE, "Bringing up environment.\n");
    base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_CONSOLE, "Loading Modules.\n");
    if (base_loadable_module_init(BASE_TRUE) != BASE_STATUS_SUCCESS) {
        *err = "Cannot load modules";
        base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_CONSOLE, "Error: %s\n", *err);
        return BASE_STATUS_GENERR;
    }

    base_load_network_lists(BASE_FALSE);

    base_load_core_config("post_load_base.conf");

    base_core_set_signal_handlers();

    if (base_event_create(&event, BASE_EVENT_STARTUP) == BASE_STATUS_SUCCESS) {
        base_event_add_header(event, BASE_STACK_BOTTOM, "Event-Info", "System Ready");
        base_event_fire(&event);
    }

    base_core_screen_size(&x, NULL);

    use = (x > 100) ? cc : cc_s;

#ifdef WIN32
    base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_CONSOLE, "%s%s\n\n", base_core_banner(), use);
#else
    base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_CONSOLE, "%s%s%s%s%s%s\n\n",
        BASE_SEQ_DEFAULT_COLOR,
        BASE_SEQ_FYELLOW, BASE_SEQ_BBLUE,
        base_core_banner(),
        use, BASE_SEQ_DEFAULT_COLOR);

#endif


    base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_INFO,
        "\nFreeBASE Version %s (%s)\n\nFreeBASE Started\nMax Sessions [%u]\nSession Rate [%d]\nSQL [%s]\n",
        base_version_full(), base_version_revision_human(),
        base_core_session_limit(0),
        base_core_sessions_per_second(0), base_test_flag((&runtime), SCF_USE_SQL) ? "Enabled" : "Disabled");


    if (x < 160) {
        base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_CONSOLE, "\n[This app Best viewed at 160x60 or more..]\n");
    }

    base_clear_flag((&runtime), SCF_NO_NEW_SESSIONS);

    if ((cmd = base_core_get_variable_dup("api_on_startup"))) {
        base_stream_handle_t stream = { 0 };
        BASE_STANDARD_STREAM(stream);
        base_console_execute(cmd, 0, &stream);
        base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_CONSOLE, "Startup command [%s] executed. Output:\n%s\n", cmd, (char *)stream.data);
        free(stream.data);
        free(cmd);
    }

#ifdef HAVE_SYSTEMD
    sd_notifyf(0, "READY=1\n"
        "MAINPID=%lu\n", (unsigned long) getpid());
#endif

    return BASE_STATUS_SUCCESS;

}

BASE_DECLARE(void) base_core_measure_time(base_time_t total_ms, base_core_time_duration_t *duration)
{
    base_time_t temp = total_ms / 1000;
    memset(duration, 0, sizeof(*duration));
    duration->mms = (uint32_t) (total_ms % 1000);
    duration->ms = (uint32_t) (temp % 1000);
    temp = temp / 1000;
    duration->sec = (uint32_t) (temp % 60);
    temp = temp / 60;
    duration->min = (uint32_t) (temp % 60);
    temp = temp / 60;
    duration->hr = (uint32_t) (temp % 24);
    temp = temp / 24;
    duration->day = (uint32_t) (temp % 365);
    duration->yr = (uint32_t) (temp / 365);
}

BASE_DECLARE(base_time_t) base_core_uptime(void)
{
    return base_mono_micro_time_now() - runtime.initiated;
}


#ifdef _MSC_VER
static void win_shutdown(void)
{

    HANDLE shutdown_event;
    char path[512];
    /* for windows we need the event to signal for shutting down a background FreeBASE */
    snprintf(path, sizeof(path), "Global\\Freebase.%d", getpid());

    /* open the event so we can signal it */
    shutdown_event = OpenEvent(EVENT_MODIFY_STATE, FALSE, path);

    if (shutdown_event) {
        /* signal the event to shutdown */
        SetEvent(shutdown_event);
        /* cleanup */
        CloseHandle(shutdown_event);
    }
}
#endif

BASE_DECLARE(void) base_core_set_signal_handlers(void)
{
    /* set signal handlers */
    signal(SIGINT, SIG_IGN);
#ifdef SIGPIPE
    signal(SIGPIPE, SIG_IGN);
#endif
#ifdef SIGALRM
    signal(SIGALRM, SIG_IGN);
#endif
#ifdef SIGQUIT
    signal(SIGQUIT, SIG_IGN);
#endif
#ifdef SIGPOLL
    signal(SIGPOLL, SIG_IGN);
#endif
#ifdef SIGIO
    signal(SIGIO, SIG_IGN);
#endif
#ifdef TRAP_BUS
    signal(SIGBUS, handle_SIGBUS);
#endif
#ifdef SIGUSR1
    signal(SIGUSR1, handle_SIGHUP);
#endif
    signal(SIGHUP, handle_SIGHUP);
}

BASE_DECLARE(uint32_t) base_core_debug_level(void)
{
    return runtime.debug_level;
}

BASE_DECLARE(int32_t) base_core_sps(void)
{
    return runtime.sps;
}

BASE_DECLARE(int32_t) base_core_sps_last(void)
{
    return runtime.sps_last;
}

BASE_DECLARE(int32_t) base_core_sps_peak(void)
{
    return runtime.sps_peak;
}

BASE_DECLARE(int32_t) base_core_sps_peak_fivemin(void)
{
    return runtime.sps_peak_fivemin;
}

BASE_DECLARE(int32_t) base_core_sessions_peak(void)
{
    return runtime.sessions_peak;
}

BASE_DECLARE(int32_t) base_core_sessions_peak_fivemin(void)
{
    return runtime.sessions_peak_fivemin;
}

BASE_DECLARE(uint32_t) base_core_max_audio_channels(uint32_t limit)
{
    if (limit) {
        runtime.max_audio_channels = limit;
    }

    return runtime.max_audio_channels;
}

BASE_DECLARE(int32_t) base_core_session_ctl(base_session_ctl_t cmd, void *val)
{
    int *intval = (int *) val;
    int oldintval = 0, newintval = 0;

    if (intval) {
        oldintval = *intval;
    }

    if (base_test_flag((&runtime), SCF_SHUTTING_DOWN)) {
        return -1;
    }

    base (cmd) {
    case SCSC_RECOVER:
    {
        char *arg = (char *) val;
        char *tech = NULL, *prof = NULL;
        int r, flush = 0;

        if (!zstr(arg)) {
            tech = strdup(arg);
            base_assert(tech);

            if ((prof = strchr(tech, ':'))) {
                *prof++ = '\0';
            }

            if (!strcasecmp(tech, "flush")) {
                flush++;

                if (prof) {
                    char *tech = prof;
                    if ((prof = strchr(tech, ':'))) {
                        *prof++ = '\0';
                    }
                }
            }

        }

        if (flush) {
            base_core_recovery_flush(tech, prof);
            r = -1;
        } else {
            r = base_core_recovery_recover(tech, prof);
        }

        base_safe_free(tech);
        return r;

    }
    break;
    case SCSC_DEBUG_SQL:
    {
        if (base_test_flag((&runtime), SCF_DEBUG_SQL)) {
            base_clear_flag((&runtime), SCF_DEBUG_SQL);
            newintval = 0;
        } else {
            base_set_flag((&runtime), SCF_DEBUG_SQL);
            newintval = 1;
        }
    }
    break;
    case SCSC_VERBOSE_EVENTS:
        if (intval) {
            if (oldintval > -1) {
                if (oldintval) {
                    base_set_flag((&runtime), SCF_VERBOSE_EVENTS);
                } else {
                    base_clear_flag((&runtime), SCF_VERBOSE_EVENTS);
                }
            }
            newintval = base_test_flag((&runtime), SCF_VERBOSE_EVENTS);
        }
        break;
    case SCSC_API_EXPANSION:
        if (intval) {
            if (oldintval > -1) {
                if (oldintval) {
                    base_set_flag((&runtime), SCF_API_EXPANSION);
                } else {
                    base_clear_flag((&runtime), SCF_API_EXPANSION);
                }
            }
            newintval = base_test_flag((&runtime), SCF_API_EXPANSION);
        }
        break;
    case SCSC_THREADED_SYSTEM_EXEC:
        if (intval) {
            if (oldintval > -1) {
                if (oldintval) {
                    base_set_flag((&runtime), SCF_THREADED_SYSTEM_EXEC);
                } else {
                    base_clear_flag((&runtime), SCF_THREADED_SYSTEM_EXEC);
                }
            }
            newintval = base_test_flag((&runtime), SCF_THREADED_SYSTEM_EXEC);
        }
        break;
    case SCSC_CALIBRATE_CLOCK:
        base_time_calibrate_clock();
        break;
    case SCSC_FLUSH_DB_HANDLES:
        base_cache_db_flush_handles();
        break;
    case SCSC_SEND_SIGHUP:
        handle_SIGHUP(1);
        break;
    case SCSC_SYNC_CLOCK:
        base_time_sync();
        newintval = 0;
        break;
    case SCSC_SYNC_CLOCK_WHEN_IDLE:
        newintval = base_core_session_sync_clock();
        break;
    case SCSC_SQL:
        if (oldintval) {
            base_core_sqldb_resume();
        } else {
            base_core_sqldb_pause();
        }
        break;
    case SCSC_PAUSE_ALL:
        if (oldintval) {
            base_set_flag((&runtime), SCF_NO_NEW_SESSIONS);
        } else {
            base_clear_flag((&runtime), SCF_NO_NEW_SESSIONS);
        }
        break;
    case SCSC_PAUSE_INBOUND:
        if (oldintval) {
            base_set_flag((&runtime), SCF_NO_NEW_INBOUND_SESSIONS);
        } else {
            base_clear_flag((&runtime), SCF_NO_NEW_INBOUND_SESSIONS);
        }
        break;
    case SCSC_PAUSE_OUTBOUND:
        if (oldintval) {
            base_set_flag((&runtime), SCF_NO_NEW_OUTBOUND_SESSIONS);
        } else {
            base_clear_flag((&runtime), SCF_NO_NEW_OUTBOUND_SESSIONS);
        }
        break;
    case SCSC_HUPALL:
        base_core_session_hupall(BASE_CAUSE_MANAGER_REQUEST);
        break;
    case SCSC_CANCEL_SHUTDOWN:
        base_clear_flag((&runtime), SCF_SHUTDOWN_REQUESTED);
        break;
    case SCSC_SAVE_HISTORY:
        base_console_save_history();
        break;
    case SCSC_CRASH:
        base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_CRIT, "Declinatio Mortuus Obfirmo!\n");
        base_console_save_history();
        abort();
        break;
    case SCSC_SHUTDOWN_NOW:
        base_console_save_history();
        exit(0);
        break;
    case SCSC_REINCARNATE_NOW:
        base_console_save_history();
        exit(BASE_STATUS_RESTART);
        break;
    case SCSC_SHUTDOWN_ELEGANT:
    case SCSC_SHUTDOWN_ASAP:
    {
        int x = 19;
        uint32_t count;
        base_event_t *shutdown_requested_event = NULL;
        if (base_event_create(&shutdown_requested_event, BASE_EVENT_SHUTDOWN_REQUESTED) == BASE_STATUS_SUCCESS) {
            base_event_add_header(shutdown_requested_event, BASE_STACK_BOTTOM, "Event-Info", "%s", cmd == SCSC_SHUTDOWN_ASAP ? "ASAP" : "elegant");
            base_event_fire(&shutdown_requested_event);
        }
        base_set_flag((&runtime), SCF_SHUTDOWN_REQUESTED);
        if (cmd == SCSC_SHUTDOWN_ASAP) {
            base_set_flag((&runtime), SCF_NO_NEW_SESSIONS);
        }

        while (runtime.running && base_test_flag((&runtime), SCF_SHUTDOWN_REQUESTED) && (count = base_core_session_count())) {
            base_yield(500000);
            if (++x == 20) {
                base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_WARNING,
                    "Shutdown in progress, %u session(s) remain.\nShutting down %s\n",
                    count, cmd == SCSC_SHUTDOWN_ASAP ? "ASAP" : "once there are no active calls.");
                x = 0;
            }
        }

        if (base_test_flag((&runtime), SCF_SHUTDOWN_REQUESTED)) {
            base_set_flag((&runtime), SCF_NO_NEW_SESSIONS);
#ifdef _MSC_VER
            win_shutdown();
#endif

            if (oldintval) {
                base_set_flag((&runtime), SCF_RESTART);
                base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_INFO, "Restarting\n");
            } else {
                base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_INFO, "Shutting down\n");
#ifdef HAVE_SYSTEMD
                sd_notifyf(0, "STOPPING=1\n");
#endif
#ifdef _MSC_VER
                fclose(stdin);
#endif
            }
            runtime.running = 0;
        } else {
            base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_INFO, "Shutdown Cancelled\n");
            base_clear_flag((&runtime), SCF_NO_NEW_SESSIONS);
        }
    }
    break;
    case SCSC_PAUSE_CHECK:
        newintval = !!(base_test_flag((&runtime), SCF_NO_NEW_SESSIONS) == SCF_NO_NEW_SESSIONS);
        break;
    case SCSC_PAUSE_INBOUND_CHECK:
        newintval = !!base_test_flag((&runtime), SCF_NO_NEW_INBOUND_SESSIONS);
        break;
    case SCSC_PAUSE_OUTBOUND_CHECK:
        newintval = !!base_test_flag((&runtime), SCF_NO_NEW_OUTBOUND_SESSIONS);
        break;
    case SCSC_READY_CHECK:
        newintval = base_core_ready();
        break;
    case SCSC_SHUTDOWN_CHECK:
        newintval = !!base_test_flag((&runtime), SCF_SHUTDOWN_REQUESTED);
        break;
    case SCSC_SHUTDOWN:

#ifdef _MSC_VER
        win_shutdown();
#endif

        if (oldintval) {
            base_set_flag((&runtime), SCF_RESTART);
            base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_INFO, "Restarting\n");
        } else {
            base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_INFO, "Shutting down\n");
#ifdef _MSC_VER
            fclose(stdin);
#endif
        }
        runtime.running = 0;
        break;
    case SCSC_CHECK_RUNNING:
        newintval = runtime.running;
        break;
    case SCSC_LOGLEVEL:
        if (oldintval >= BASE_LOG_DISABLE) {
            runtime.hard_log_level = oldintval;
        }

        if (runtime.hard_log_level > BASE_LOG_DEBUG) {
            runtime.hard_log_level = BASE_LOG_DEBUG;
        }
        newintval = runtime.hard_log_level;
        break;
    case SCSC_DEBUG_LEVEL:
        if (oldintval > -1) {
            if (oldintval > 10)
                oldintval = 10;
            runtime.debug_level = oldintval;
        }
        newintval = runtime.debug_level;
        break;
    case SCSC_MIN_IDLE_CPU:
    {
        double *dval = (double *) val;
        if (dval) {
            *dval = base_core_min_idle_cpu(*dval);
        }
        intval = NULL;
    }
    break;
    case SCSC_MAX_SESSIONS:
        newintval = base_core_session_limit(oldintval);
        break;
    case SCSC_LAST_SPS:
        newintval = runtime.sps_last;
        break;
    case SCSC_SPS_PEAK:
        if (oldintval == -1) {
            runtime.sps_peak = 0;
        }
        newintval = runtime.sps_peak;
        break;
    case SCSC_SPS_PEAK_FIVEMIN:
        newintval = runtime.sps_peak_fivemin;
        break;
    case SCSC_SESSIONS_PEAK:
        newintval = runtime.sessions_peak;
        break;
    case SCSC_SESSIONS_PEAK_FIVEMIN:
        newintval = runtime.sessions_peak_fivemin;
        break;
    case SCSC_MAX_DTMF_DURATION:
        newintval = base_core_max_dtmf_duration(oldintval);
        break;
    case SCSC_MIN_DTMF_DURATION:
        newintval = base_core_min_dtmf_duration(oldintval);
        break;
    case SCSC_DEFAULT_DTMF_DURATION:
        newintval = base_core_default_dtmf_duration(oldintval);
        break;
    case SCSC_SPS:
        base_mutex_lock(runtime.throttle_mutex);
        if (oldintval > 0) {
            runtime.sps_total = oldintval;
        }
        newintval = runtime.sps_total;
        base_mutex_unlock(runtime.throttle_mutex);
        break;

    case SCSC_RECLAIM:
        base_core_memory_reclaim_all();
        newintval = 0;
        break;
    case SCSC_MDNS_RESOLVE:
        base_core_media_set_resolveice(!!oldintval);
        break;
    case SCSC_SHUTDOWN_CAUSE:
        runtime.shutdown_cause = oldintval;
        break;
    }

    if (intval) {
        *intval = newintval;
    }


    return 0;
}

BASE_DECLARE(base_core_flag_t) base_core_flags(void)
{
    return runtime.flags;
}

BASE_DECLARE(base_bool_t) base_core_running(void)
{
    return runtime.running ? BASE_TRUE : BASE_FALSE;
}

BASE_DECLARE(base_bool_t) base_core_ready(void)
{
    return (base_test_flag((&runtime), SCF_SHUTTING_DOWN) || base_test_flag((&runtime), SCF_NO_NEW_SESSIONS) == SCF_NO_NEW_SESSIONS) ? BASE_FALSE : BASE_TRUE;
}

BASE_DECLARE(base_bool_t) base_core_ready_inbound(void)
{
    return (base_test_flag((&runtime), SCF_SHUTTING_DOWN) || base_test_flag((&runtime), SCF_NO_NEW_INBOUND_SESSIONS)) ? BASE_FALSE : BASE_TRUE;
}

BASE_DECLARE(base_bool_t) base_core_ready_outbound(void)
{
    return (base_test_flag((&runtime), SCF_SHUTTING_DOWN) || base_test_flag((&runtime), SCF_NO_NEW_OUTBOUND_SESSIONS)) ? BASE_FALSE : BASE_TRUE;
}

void base_core_sqldb_destroy(void)
{
    if (base_test_flag((&runtime), SCF_USE_SQL)) {
        base_core_sqldb_stop();
    }
}

BASE_DECLARE(base_status_t) base_core_destroy(void)
{
    base_event_t *event;

    if (base_event_create(&event, BASE_EVENT_SHUTDOWN) == BASE_STATUS_SUCCESS) {
        base_event_add_header(event, BASE_STACK_BOTTOM, "Event-Info", "System Shutting Down");
        base_event_fire(&event);
    }

    base_set_flag((&runtime), SCF_NO_NEW_SESSIONS);
    base_set_flag((&runtime), SCF_SHUTTING_DOWN);

    base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_CONSOLE, "End existing sessions\n");
    base_core_session_hupall(runtime.shutdown_cause);
    base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_CONSOLE, "Clean up modules.\n");

    base_loadable_module_shutdown();

    base_curl_destroy();

    base_ssl_destroy_ssl_locks();
    EVP_cleanup();

    base_scheduler_task_thread_stop();

    base_rtp_shutdown();
    base_msrp_destroy();

    if (base_test_flag((&runtime), SCF_USE_AUTO_NAT)) {
        base_nat_shutdown();
    }
    base_xml_destroy();
    base_console_shutdown();
    base_channel_global_uninit();

    base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_CONSOLE, "Closing Event Engine.\n");
    base_event_shutdown();

    base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_CONSOLE, "Finalizing Shutdown.\n");
    base_log_shutdown();

    base_core_session_uninit();
    base_core_unset_variables();
    base_core_memory_stop();

    if (runtime.console && runtime.console != stdout && runtime.console != stderr) {
        fclose(runtime.console);
        runtime.console = NULL;
    }

    base_safe_free(BASE_GLOBAL_dirs.base_dir);
    base_safe_free(BASE_GLOBAL_dirs.mod_dir);
    base_safe_free(BASE_GLOBAL_dirs.conf_dir);
    base_safe_free(BASE_GLOBAL_dirs.log_dir);
    base_safe_free(BASE_GLOBAL_dirs.db_dir);
    base_safe_free(BASE_GLOBAL_dirs.script_dir);
    base_safe_free(BASE_GLOBAL_dirs.htdocs_dir);
    base_safe_free(BASE_GLOBAL_dirs.grammar_dir);
    base_safe_free(BASE_GLOBAL_dirs.fonts_dir);
    base_safe_free(BASE_GLOBAL_dirs.images_dir);
    base_safe_free(BASE_GLOBAL_dirs.storage_dir);
    base_safe_free(BASE_GLOBAL_dirs.cache_dir);
    base_safe_free(BASE_GLOBAL_dirs.recordings_dir);
    base_safe_free(BASE_GLOBAL_dirs.sounds_dir);
    base_safe_free(BASE_GLOBAL_dirs.run_dir);
    base_safe_free(BASE_GLOBAL_dirs.temp_dir);
    base_safe_free(BASE_GLOBAL_dirs.data_dir);
    base_safe_free(BASE_GLOBAL_dirs.localstate_dir);
    base_safe_free(BASE_GLOBAL_dirs.certs_dir);
    base_safe_free(BASE_GLOBAL_dirs.lib_dir);

    base_safe_free(BASE_GLOBAL_filenames.conf_name);

    base_event_destroy(&runtime.global_vars);
    base_core_hash_destroy(&runtime.ptimes);
    base_core_hash_destroy(&runtime.mime_types);
    base_core_hash_destroy(&runtime.mime_type_exts);

    if (IP_LIST.hash) {
        base_core_hash_destroy(&IP_LIST.hash);
    }

    if (IP_LIST.pool) {
        base_core_destroy_memory_pool(&IP_LIST.pool);
    }

    base_core_media_deinit();

    if (runtime.memory_pool) {
        fspr_pool_destroy(runtime.memory_pool);
        fspr_terminate();
    }

    sqlite3_shutdown();

    return base_test_flag((&runtime), SCF_RESTART) ? BASE_STATUS_RESTART : BASE_STATUS_SUCCESS;
}

BASE_DECLARE(base_status_t) base_core_management_exec(char *relative_oid, base_management_action_t action, char *data, base_size_t datalen)
{
    const base_management_interface_t *ptr;
    base_status_t status = BASE_STATUS_FALSE;

    if ((ptr = base_loadable_module_get_management_interface(relative_oid))) {
        status = ptr->management_function(relative_oid, action, data, datalen);
    }

    return status;
}

BASE_DECLARE(void) base_core_memory_reclaim_all(void)
{
    base_core_memory_reclaim_logger();
    base_core_memory_reclaim_events();
    base_core_memory_reclaim();
}


struct system_thread_handle {
    const char *cmd;
    base_thread_cond_t *cond;
    base_mutex_t *mutex;
    base_memory_pool_t *pool;
    int ret;
    int *fds;
};

static void *BASE_THREAD_FUNC system_thread(base_thread_t *thread, void *obj)
{
    struct system_thread_handle *sth = (struct system_thread_handle *) obj;

#if defined(HAVE_SETRLIMIT) && !defined(__FreeBSD__)
    struct rlimit rlim;
    struct rlimit rlim_save;

    memset(&rlim, 0, sizeof(rlim));
    getrlimit(RLIMIT_STACK, &rlim);

    memset(&rlim_save, 0, sizeof(rlim_save));
    getrlimit(RLIMIT_STACK, &rlim_save);

    rlim.rlim_cur = rlim.rlim_max;
    if (setrlimit(RLIMIT_STACK, &rlim) < 0) {
        base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_ERROR, "Setting stack size failed! (%s)\n", strerror(errno));
    }
#endif

    if (sth->fds) {
        dup2(sth->fds[1], STDOUT_FILENO);
    }

    sth->ret = system(sth->cmd);

#if defined(HAVE_SETRLIMIT) && !defined(__FreeBSD__)
    if (setrlimit(RLIMIT_STACK, &rlim_save) < 0) {
        base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_ERROR, "Setting stack size failed! (%s)\n", strerror(errno));
    }
#endif

    base_mutex_lock(sth->mutex);
    base_thread_cond_signal(sth->cond);
    base_mutex_unlock(sth->mutex);

    base_core_destroy_memory_pool(&sth->pool);

    return NULL;
}


static int base_system_thread(const char *cmd, base_bool_t wait)
{
    base_thread_t *thread;
    base_threadattr_t *thd_attr;
    int ret = 0;
    struct system_thread_handle *sth;
    base_memory_pool_t *pool;

    if (base_core_new_memory_pool(&pool) != BASE_STATUS_SUCCESS) {
        base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_CRIT, "Pool Failure\n");
        return 1;
    }

    if (!(sth = base_core_alloc(pool, sizeof(struct system_thread_handle)))) {
        base_core_destroy_memory_pool(&pool);
        return 1;
    }

    sth->pool = pool;
    sth->cmd = base_core_strdup(pool, cmd);

    base_thread_cond_create(&sth->cond, sth->pool);
    base_mutex_init(&sth->mutex, BASE_MUTEX_NESTED, sth->pool);
    base_mutex_lock(sth->mutex);

    base_threadattr_create(&thd_attr, sth->pool);
    base_threadattr_stacksize_set(thd_attr, BASE_SYSTEM_THREAD_STACKSIZE);
    base_threadattr_detach_set(thd_attr, 1);
    base_thread_create(&thread, thd_attr, system_thread, sth, sth->pool);

    if (wait) {
        base_thread_cond_wait(sth->cond, sth->mutex);
        ret = sth->ret;
    }
    base_mutex_unlock(sth->mutex);

    return ret;
}

BASE_DECLARE(int) base_max_file_desc(void)
{
    int max = 0;

#ifndef WIN32
#if defined(HAVE_GETDTABLESIZE)
    max = getdtablesize();
#else
    max = sysconf(_SC_OPEN_MAX);
#endif
#endif

    return max;

}

BASE_DECLARE(void) base_close_extra_files(int *keep, int keep_ttl)
{
    int open_max = base_max_file_desc();
    int i, j;

    for (i = 3; i < open_max; i++) {
        if (keep) {
            for (j = 0; j < keep_ttl; j++) {
                if (i == keep[j]) {
                    goto skip;
                }
            }
        }

        close(i);

    skip:

        continue;

    }
}


#ifdef WIN32
static int base_system_fork(const char *cmd, base_bool_t wait)
{
    return base_system_thread(cmd, wait);
}

BASE_DECLARE(pid_t) base_fork(void)
{
    return -1;
}


#else

BASE_DECLARE(pid_t) base_fork(void)
{
    int i = fork();

    if (!i) {
        set_low_priority();
    }

    return i;
}



static int base_system_fork(const char *cmd, base_bool_t wait)
{
    int pid;
    char *dcmd = strdup(cmd);
#if defined(HAVE_SETRLIMIT) && !defined(__FreeBSD__)
    struct rlimit rlim;
    struct rlimit rlim_save;
#endif

    base_core_set_signal_handlers();

    pid = base_fork();

    if (pid) {
        if (wait) {
            waitpid(pid, NULL, 0);
        }
        free(dcmd);
    } else {
        base_close_extra_files(NULL, 0);

#if defined(HAVE_SETRLIMIT) && !defined(__FreeBSD__)
        memset(&rlim, 0, sizeof(rlim));
        getrlimit(RLIMIT_STACK, &rlim);

        memset(&rlim_save, 0, sizeof(rlim_save));
        getrlimit(RLIMIT_STACK, &rlim_save);

        rlim.rlim_cur = rlim.rlim_max;
        if (setrlimit(RLIMIT_STACK, &rlim) < 0) {
            base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_ERROR, "Setting stack size failed! (%s)\n", strerror(errno));
        }
#endif

        if (system(dcmd) == -1) {
            base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_ERROR, "Failed to execute because of a command error : %s\n", dcmd);
        }
        free(dcmd);
        exit(0);
    }

    return 0;
}
#endif



BASE_DECLARE(int) base_system(const char *cmd, base_bool_t wait)
{
    int retval = 0;
#ifdef __linux__
    base_bool_t spawn_instead_of_system = base_true(base_core_get_variable("spawn_instead_of_system"));
#else
    base_bool_t spawn_instead_of_system = BASE_FALSE;
#endif

    if (spawn_instead_of_system) {
        retval = base_stream_spawn(cmd, BASE_TRUE, wait, NULL);
    } else if (base_test_flag((&runtime), SCF_THREADED_SYSTEM_EXEC)) {
        retval = base_system_thread(cmd, wait);
    } else {
        retval = base_system_fork(cmd, wait);
    }
    return retval;
}



BASE_DECLARE(int) base_stream_system_fork(const char *cmd, base_stream_handle_t *stream)
{
    return base_stream_system(cmd, stream);
}

#ifdef __linux__
extern char **environ;
#endif

BASE_DECLARE(int) base_stream_spawn(const char *cmd, base_bool_t shell, base_bool_t wait, base_stream_handle_t *stream)
{
#ifndef __linux__
    base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_WARNING, "posix_spawn is unsupported on current platform\n");
    return 1;
#else
    int status = 0;
    char buffer[1024];
    pid_t pid;
    char *pdata = NULL, *argv[64];
    posix_spawn_file_actions_t action;
    posix_spawnattr_t *attr;
    int cout_pipe[2];
    int cerr_pipe[2];
    struct pollfd pfds[2] = { {0} };

    if (zstr(cmd)) {
        base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_WARNING, "Failed to execute base_spawn_stream because of empty command\n");
        return 1;
    }

    if (shell) {
        argv[0] = base_core_get_variable("spawn_system_shell");
        argv[1] = "-c";
        argv[2] = (char *)cmd;
        argv[3] = NULL;
        if (zstr(argv[0])) {
            argv[0] = "/bin/sh";
        }
    } else {
        if (!(pdata = strdup(cmd))) {
            return 1;
        }
        if (!base_separate_string(pdata, ' ', argv, (sizeof(argv) / sizeof(argv[0])))) {
            free(pdata);
            return 1;
        }
    }

    if (!(attr = malloc(sizeof(posix_spawnattr_t)))) {
        base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_ERROR, "Failed to execute base_spawn_stream because of a memory error: %s\n", cmd);
        base_safe_free(pdata);
        return 1;
    }

    if (stream) {
        if (pipe(cout_pipe)) {
            base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_ERROR, "Failed to execute base_spawn_stream because of a pipe error: %s\n", cmd);
            free(attr);
            base_safe_free(pdata);
            return 1;
        }

        if (pipe(cerr_pipe)) {
            base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_ERROR, "Failed to execute base_spawn_stream because of a pipe error: %s\n", cmd);
            close(cout_pipe[0]);
            close(cout_pipe[1]);
            free(attr);
            base_safe_free(pdata);
            return 1;
        }
    }

    memset(attr, 0, sizeof(posix_spawnattr_t));
    posix_spawnattr_init(attr);
    posix_spawnattr_setflags(attr, POSIX_SPAWN_USEVFORK);

    posix_spawn_file_actions_init(&action);

    if (stream) {
        posix_spawn_file_actions_addclose(&action, cout_pipe[0]);
        posix_spawn_file_actions_addclose(&action, cerr_pipe[0]);
        posix_spawn_file_actions_adddup2(&action, cout_pipe[1], 1);
        posix_spawn_file_actions_adddup2(&action, cerr_pipe[1], 2);

        posix_spawn_file_actions_addclose(&action, cout_pipe[1]);
        posix_spawn_file_actions_addclose(&action, cerr_pipe[1]);
    }

    if (posix_spawnp(&pid, argv[0], &action, attr, argv, environ) != 0) {
        status = 1;
        base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_WARNING, "Failed to execute posix_spawnp: %s\n", cmd);
        if (stream) {
            close(cout_pipe[0]), close(cerr_pipe[0]);
            close(cout_pipe[1]), close(cerr_pipe[1]);
        }
    } else {
        if (stream) {
            close(cout_pipe[1]), close(cerr_pipe[1]); /* close child-side of pipes */

            pfds[0] = (struct pollfd) {
                .fd = cout_pipe[0],
                .events = POLLIN,
                .revents = 0
            };

            pfds[1] = (struct pollfd) {
                .fd = cerr_pipe[0],
                .events = POLLIN,
                .revents = 0
            };

            while (poll(pfds, 2, /*timeout*/-1) > 0) {
                if (pfds[0].revents & POLLIN) {
                    int bytes_read = read(cout_pipe[0], buffer, sizeof(buffer));
                    stream->raw_write_function(stream, (unsigned char *)buffer, bytes_read);
                } else if (pfds[1].revents & POLLIN) {
                    int bytes_read = read(cerr_pipe[0], buffer, sizeof(buffer));
                    base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_WARNING, "STDERR of cmd (%s): %.*s\n", cmd, bytes_read, buffer);
                } else {
                    break; /* nothing left to read */
                }
            }

            close(cout_pipe[0]), close(cerr_pipe[0]);
        }

        if (wait) {
            if (waitpid(pid, &status, 0) != pid) {
                base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_WARNING, "waitpid failed: %s\n", cmd);
            } else if (status != 0) {
                base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_WARNING, "Exit status (%d): %s\n", status, cmd);
            }
        }
    }

    posix_spawnattr_destroy(attr);
    free(attr);
    posix_spawn_file_actions_destroy(&action);
    base_safe_free(pdata);

    return status;
#endif
}

BASE_DECLARE(int) base_spawn(const char *cmd, base_bool_t wait)
{
    return base_stream_spawn(cmd, BASE_FALSE, wait, NULL);
}

BASE_DECLARE(base_status_t) base_core_get_stacksizes(base_size_t *cur, base_size_t *max)
{
#ifdef HAVE_SETRLIMIT
    struct rlimit rlp;

    memset(&rlp, 0, sizeof(rlp));
    getrlimit(RLIMIT_STACK, &rlp);

    *cur = rlp.rlim_cur;
    *max = rlp.rlim_max;

    return BASE_STATUS_SUCCESS;

#else

    return BASE_STATUS_FALSE;

#endif



}


BASE_DECLARE(int) base_stream_system(const char *cmd, base_stream_handle_t *stream)
{
#ifdef __linux__
    base_bool_t spawn_instead_of_system = base_true(base_core_get_variable("spawn_instead_of_system"));
#else
    base_bool_t spawn_instead_of_system = BASE_FALSE;
#endif

    if (spawn_instead_of_system){
        return base_stream_spawn(cmd, BASE_TRUE, BASE_TRUE, stream);
    } else {
        char buffer[128];
        size_t bytes;
        FILE* pipe = popen(cmd, "r");
        if (!pipe) return 1;

        while (!feof(pipe)) {
            while ((bytes = fread(buffer, 1, 128, pipe)) > 0) {
                if (stream != NULL) {
                    stream->raw_write_function(stream, (unsigned char *)buffer, bytes);
                }
            }
        }

        if (ferror(pipe)) {
            pclose(pipe);
            return 1;
        }

        pclose(pipe);
        return 0;
    }
}

BASE_DECLARE(uint16_t) base_core_get_rtp_port_range_start_port(void)
{
    uint16_t start_port = 0;

    /* By default pass rtp port range start value as zero in order to get actual
    * RTP port range start value as configured */
    start_port = (uint16_t)base_rtp_set_start_port((base_port_t)start_port);

    return start_port;
}

BASE_DECLARE(uint16_t) base_core_get_rtp_port_range_end_port(void)
{
    uint16_t end_port = 0;

    /* By default pass rtp port range end value as zero in order to get actual
    * RTP port range end value as configured */
    end_port = (uint16_t)base_rtp_set_end_port((base_port_t)end_port);

    return end_port;
}

BASE_DECLARE(const char *) base_core_get_event_channel_key_separator(void)
{
    return runtime.event_channel_key_separator;
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
