/*
* FreeBASE Modular Media Switching Software Library / Soft-Switch Application
* Copyright (C) 2005-2021, Anthony Minessale II <anthm@freebase.org>
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
* Andrey Volk <andywolk@gmail.com>
*
*
* base_core.h -- Core Library Private Data (not to be installed into the system)
* If the last line didn't make sense, stop reading this file, go away!,
* this file does not exist!!!!
*
*/
#include "base_profile.h"

#ifndef WIN32
#include <base_private.h>
#endif

#ifdef HAVE_MLOCKALL
#include <sys/mman.h>
#endif

#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif

#ifndef WIN32
/* setuid, setgid */
#include <unistd.h>

/* getgrnam, getpwnam */
#include <pwd.h>
#include <grp.h>

#ifdef HAVE_SYS_RESOURCE_H
#include <sys/resource.h>
#endif
#endif

/* #define DEBUG_ALLOC */
#define DO_EVENTS

#define BASE_EVENT_QUEUE_LEN 256
#define BASE_MESSAGE_QUEUE_LEN 256

#define BASE_BUFFER_BLOCK_FRAMES 25
#define BASE_BUFFER_START_FRAMES 50

typedef enum {
    SSF_NONE = 0,
    SSF_DESTROYED = (1 << 0),
    SSF_WARN_TRANSCODE = (1 << 1),
    SSF_HANGUP = (1 << 2),
    SSF_THREAD_STARTED = (1 << 3),
    SSF_THREAD_RUNNING = (1 << 4),
    SSF_READ_TRANSCODE = (1 << 5),
    SSF_WRITE_TRANSCODE = (1 << 6),
    SSF_READ_CODEC_RESET = (1 << 7),
    SSF_WRITE_CODEC_RESET = (1 << 8),
    SSF_DESTROYABLE = (1 << 9),
    SSF_MEDIA_BUG_TAP_ONLY = (1 << 10)
} base_session_flag_t;

typedef enum {
    DBTYPE_DEFAULT = 0,
    DBTYPE_MSSQL = 1,
} base_dbtype_t;

struct base_runtime {
    base_time_t initiated;
    base_time_t reference;
    int64_t offset;
    base_event_t *global_vars;
    base_hash_t *mime_types;
    base_hash_t *mime_type_exts;
    base_hash_t *ptimes;
    base_memory_pool_t *memory_pool;
    const base_state_handler_table_t *state_handlers[BASE_MAX_STATE_HANDLERS];
    int state_handler_index;
    FILE *console;
    uint8_t running;
    char uuid_str[BASE_UUID_FORMATTED_LENGTH + 1];
    uint32_t flags;
    base_time_t timestamp;
    base_mutex_t *uuid_mutex;
    base_mutex_t *throttle_mutex;
    base_mutex_t *session_hash_mutex;
    base_mutex_t *global_mutex;
    base_thread_rwlock_t *global_var_rwlock;
    uint32_t sps_total;
    int32_t sps;
    int32_t sps_last;
    int32_t sps_peak;
    int32_t sps_peak_fivemin;
    int32_t sessions_peak;
    int32_t sessions_peak_fivemin;
    base_log_level_t hard_log_level;
    char *mailer_app;
    char *mailer_app_args;
    uint32_t max_dtmf_duration;
    uint32_t min_dtmf_duration;
    uint32_t default_dtmf_duration;
    char dummy_data[5];
    base_bool_t colorize_console;
    char *odbc_dsn;
    char *dbname;
    uint32_t debug_level;
    uint32_t runlevel;
    uint32_t tipping_point;
    uint32_t cpu_idle_smoothing_depth;
    uint32_t microseconds_per_tick;
    int32_t timer_affinity;
    base_profile_timer_t *profile_timer;
    double profile_time;
    double min_idle_time;
    base_dbtype_t odbc_dbtype;
    char hostname[256];
    char *basename;
    int multiple_registrations;
    uint32_t max_db_handles;
    uint32_t db_handle_timeout;
    uint32_t event_heartbeat_interval;
    int cpu_count;
    uint32_t time_sync;
    char *core_db_pre_trans_execute;
    char *core_db_post_trans_execute;
    char *core_db_inner_pre_trans_execute;
    char *core_db_inner_post_trans_execute;
    int events_use_dispatch;
    uint32_t port_alloc_flags;
    char *event_channel_key_separator;
    uint32_t max_audio_channels;
    base_call_cause_t shutdown_cause;
};

extern struct base_runtime runtime;


struct base_session_manager {
    base_memory_pool_t *memory_pool;
    base_hash_t *session_table;
    uint32_t session_count;
    uint32_t session_limit;
    base_size_t session_id;
    base_queue_t *thread_queue;
    base_mutex_t *mutex;
    base_thread_cond_t *cond;
    int running;
    int busy;
};

extern struct base_session_manager session_manager;



base_status_t base_core_sqldb_init(const char **err);
void base_core_sqldb_destroy(void);
base_status_t base_core_sqldb_start(base_memory_pool_t *pool, base_bool_t manage);
void base_core_sqldb_stop(void);
void base_core_session_init(base_memory_pool_t *pool);
void base_core_session_uninit(void);
void base_core_state_machine_init(base_memory_pool_t *pool);
base_memory_pool_t *base_core_memory_init(void);
void base_core_memory_stop(void);
