/*
* FreeBASE Modular Media Switching Software Library / Soft-Switch Application
* Copyright (C) 2005-2020, Anthony Minessale II <anthm@freebase.org>
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
* Luke Dashjr <luke@openmethods.com> (OpenMethods, LLC)
* Joseph Sullivan <jossulli@amazon.com>
* Emmanuel Schmidbauer <eschmidbauer@gmail.com>
* Andrey Volk <andywolk@gmail.com>
*
* base_core.h -- Core Library
*
*/
/*! \file base_core.h
\brief Core Library

This module is the main core library and is the intended location of all fundamental operations.
*/

#ifndef BASE_CORE_H
#define BASE_CORE_H

#include <base.h>

BASE_BEGIN_EXTERN_C
#define BASE_MAX_CORE_THREAD_SESSION_OBJS 128
#define BASE_MAX_STREAMS 128
struct base_core_time_duration {
    uint32_t mms;
    uint32_t ms;
    uint32_t sec;
    uint32_t min;
    uint32_t hr;
    uint32_t day;
    uint32_t yr;
};

struct base_app_log {
    char *app;
    char *arg;
    base_time_t stamp;
    struct base_app_log *next;
};

typedef struct base_thread_data_s {
    base_thread_start_t func;
    void *obj;
    int alloc;
    int running;
    base_memory_pool_t *pool;
} base_thread_data_t;

typedef struct base_hold_record_s {
    base_time_t on;
    base_time_t off;
    char *uuid;
    struct base_hold_record_s *next;
} base_hold_record_t;

typedef struct device_uuid_node_s {
    char *uuid;
    base_xml_t xml_cdr;
    base_event_t *event;
    base_channel_callstate_t callstate;
    base_hold_record_t *hold_record;
    base_caller_profile_t *hup_profile;
    base_call_direction_t direction;
    struct base_device_record_s *parent;
    struct device_uuid_node_s *next;
} base_device_node_t;

typedef struct base_device_stats_s {
    uint32_t total;
    uint32_t total_in;
    uint32_t total_out;
    uint32_t offhook;
    uint32_t offhook_in;
    uint32_t offhook_out;
    uint32_t active;
    uint32_t active_in;
    uint32_t active_out;
    uint32_t held;
    uint32_t held_in;
    uint32_t held_out;
    uint32_t unheld;
    uint32_t unheld_in;
    uint32_t unheld_out;
    uint32_t hup;
    uint32_t hup_in;
    uint32_t hup_out;
    uint32_t ringing;
    uint32_t ringing_in;
    uint32_t ringing_out;
    uint32_t early;
    uint32_t early_in;
    uint32_t early_out;
    uint32_t ring_wait;
} base_device_stats_t;


typedef struct base_device_record_s {
    char *device_id;
    char *uuid;
    int refs;
    base_device_stats_t stats;
    base_device_stats_t last_stats;
    base_device_state_t state;
    base_device_state_t last_state;
    base_time_t active_start;
    base_time_t active_stop;
    base_time_t last_call_time;
    base_time_t ring_start;
    base_time_t ring_stop;
    base_time_t hold_start;
    base_time_t hold_stop;
    base_time_t call_start;
    struct device_uuid_node_s *uuid_list;
    struct device_uuid_node_s *uuid_tail;
    base_mutex_t *mutex;
    base_memory_pool_t *pool;
    void *user_data;
} base_device_record_t;

typedef void(*base_device_state_function_t)(base_core_session_t *session, base_channel_callstate_t callstate, base_device_record_t *drec);


#define DTLS_SRTP_FNAME "dtls-srtp"
#define MAX_FPLEN 64
#define MAX_FPSTRLEN 192

typedef struct dtls_fp_s {
    uint32_t len;
    uint8_t data[MAX_FPLEN+1];
    char *type;
    char str[MAX_FPSTRLEN];
} dtls_fingerprint_t;

typedef enum {
    DTLS_TYPE_CLIENT = (1 << 0),
    DTLS_TYPE_SERVER = (1 << 1),
    DTLS_TYPE_RTP = (1 << 2),
    DTLS_TYPE_RTCP = (1 << 3)
} dtls_type_t;

typedef enum {
    DS_OFF,
    DS_HANDSHAKE,
    DS_SETUP,
    DS_READY,
    DS_FAIL,
    DS_INVALID,
} dtls_state_t;

#define MESSAGE_STAMP_FFL(_m) _m->_file = __FILE__; _m->_func = __BASE_FUNC__; _m->_line = __LINE__

#define MESSAGE_STRING_ARG_MAX 10
/*! \brief A message object designed to allow unlike technologies to exchange data */
struct base_core_session_message {
    /*! uuid of the sender (for replies) */
    char *from;
    /*! enumeration of the type of message */
    base_core_session_message_types_t message_id;

    /*! optional numeric arg */
    int numeric_arg;
    /*! optional string arg */
    const char *string_arg;
    /*! optional string arg */
    base_size_t string_arg_size;
    /*! optional arbitrary pointer arg */
    void *pointer_arg;
    /*! optional arbitrary pointer arg's size */
    base_size_t pointer_arg_size;

    /*! optional numeric reply */
    int numeric_reply;
    /*! optional string reply */
    char *string_reply;
    /*! optional string reply */
    base_size_t string_reply_size;
    /*! optional arbitrary pointer reply */
    void *pointer_reply;
    /*! optional arbitrary pointer reply's size */
    base_size_t pointer_reply_size;
    /*! message flags */
    base_core_session_message_flag_t flags;
    const char *_file;
    const char *_func;
    int _line;
    const char *string_array_arg[MESSAGE_STRING_ARG_MAX];
    time_t delivery_time;
};

/*! \brief A generic object to pass as a thread's session object to allow mutiple arguements and a pool */
struct base_core_thread_session {
    /*! status of the thread */
    int running;
    /*! mutex */
    base_mutex_t *mutex;
    /*! array of void pointers to pass mutiple data objects */
    void *objs[BASE_MAX_CORE_THREAD_SESSION_OBJS];
    /*! a pointer to a memory pool if the thread has it's own pool */
    base_input_callback_function_t input_callback;
    base_memory_pool_t *pool;
};

struct base_core_session;
struct base_core_runtime;
struct base_core_port_allocator;


static inline void *base_must_malloc(size_t _b)
{
    void *m = malloc(_b);
    base_assert(m);
    return m;
}

static inline void *base_must_realloc(void *_b, size_t _z)
{
    void *m = realloc(_b, _z);
    base_assert(m);
    return m;
}

/*!
\defgroup core1 Core Library
\ingroup FREEBASE
\{
*/

///\defgroup mb1 Media Bugs
///\ingroup core1
///\{


BASE_DECLARE(void) base_core_screen_size(int *x, int *y);
BASE_DECLARE(void) base_core_session_sched_heartbeat(base_core_session_t *session, uint32_t seconds);
BASE_DECLARE(void) base_core_session_unsched_heartbeat(base_core_session_t *session);

BASE_DECLARE(void) base_core_session_enable_heartbeat(base_core_session_t *session, uint32_t seconds);
BASE_DECLARE(void) base_core_session_disable_heartbeat(base_core_session_t *session);

#define base_core_session_get_name(_s) base_channel_get_name(base_core_session_get_channel(_s))

BASE_DECLARE(base_status_t) base_core_media_bug_pop(base_core_session_t *orig_session, const char *function, base_media_bug_t **pop);

BASE_DECLARE(base_status_t) base_core_media_bug_exec_all(base_core_session_t *orig_session,
    const char *function, base_media_bug_exec_cb_t cb, void *user_data);
BASE_DECLARE(uint32_t) base_core_media_bug_patch_video(base_core_session_t *orig_session, base_frame_t *frame);
BASE_DECLARE(uint32_t) base_core_media_bug_count(base_core_session_t *orig_session, const char *function);
BASE_DECLARE(void) base_media_bug_set_spy_fmt(base_media_bug_t *bug, base_vid_spy_fmt_t spy_fmt);
BASE_DECLARE(base_status_t) base_core_media_bug_push_spy_frame(base_media_bug_t *bug, base_frame_t *frame, base_rw_t rw);
BASE_DECLARE(base_vid_spy_fmt_t) base_media_bug_parse_spy_fmt(const char *name);

/*!
\brief Add a media bug to the session
\param session the session to add the bug to
\param function user defined module/function/reason identifying this bug
\param target user defined identification of the target of the bug
\param callback a callback for events
\param user_data arbitrary user data
\param stop_time absolute time at which the bug is automatically removed (or 0)
\param flags flags to choose the stream
\param new_bug pointer to assign new bug to
\return BASE_STATUS_SUCCESS if the operation was a success
*/
BASE_DECLARE(base_status_t) base_core_media_bug_add(_In_ base_core_session_t *session,
    _In_ const char *function,
    _In_ const char *target,
    _In_ base_media_bug_callback_t callback,
    _In_opt_ void *user_data,
    _In_ time_t stop_time, _In_ base_media_bug_flag_t flags, _Out_ base_media_bug_t **new_bug);

/*!
\brief Pause a media bug on the session
\param session the session to pause the bug on sets CF_PAUSE_BUGS flag
*/
BASE_DECLARE(void) base_core_media_bug_pause(base_core_session_t *session);

/*!
\brief Resume a media bug on the session
\param session the session to resume the bug on, clears CF_PAUSE_BUGS flag
*/
BASE_DECLARE(void) base_core_media_bug_resume(base_core_session_t *session);

BASE_DECLARE(void) base_core_media_bug_inuse(base_media_bug_t *bug, base_size_t *readp, base_size_t *writep);

/*!
\brief Obtain private data from a media bug
\param bug the bug to get the data from
\return the private data
*/
BASE_DECLARE(void *) base_core_media_bug_get_user_data(_In_ base_media_bug_t *bug);

/*!
\brief Obtain a replace frame from a media bug
\param bug the bug to get the data from
*/
BASE_DECLARE(base_frame_t *) base_core_media_bug_get_write_replace_frame(_In_ base_media_bug_t *bug);


BASE_DECLARE(base_frame_t *) base_core_media_bug_get_native_read_frame(base_media_bug_t *bug);
BASE_DECLARE(base_frame_t *) base_core_media_bug_get_native_write_frame(base_media_bug_t *bug);


BASE_DECLARE(base_frame_t *) base_core_media_bug_get_video_ping_frame(base_media_bug_t *bug);

/*!
\brief Set a return replace frame
\param bug the bug to set the frame on
\param frame the frame to set
*/
BASE_DECLARE(void) base_core_media_bug_set_write_replace_frame(_In_ base_media_bug_t *bug, _In_ base_frame_t *frame);

/*!
\brief Obtain a replace frame from a media bug
\param bug the bug to get the data from
*/
BASE_DECLARE(base_frame_t *) base_core_media_bug_get_read_replace_frame(_In_ base_media_bug_t *bug);
BASE_DECLARE(void) base_core_media_bug_set_read_demux_frame(_In_ base_media_bug_t *bug, _In_ base_frame_t *frame);
/*!
\brief Obtain the session from a media bug
\param bug the bug to get the data from
*/
BASE_DECLARE(base_core_session_t *) base_core_media_bug_get_session(_In_ base_media_bug_t *bug);

BASE_DECLARE(const char *) base_core_media_bug_get_text(base_media_bug_t *bug);

/*!
\brief Test for the existance of a flag on an media bug
\param bug the object to test
\param flag the or'd list of flags to test
\return true value if the object has the flags defined
*/
BASE_DECLARE(uint32_t) base_core_media_bug_test_flag(_In_ base_media_bug_t *bug, _In_ uint32_t flag);
BASE_DECLARE(uint32_t) base_core_media_bug_set_flag(_In_ base_media_bug_t *bug, _In_ uint32_t flag);
BASE_DECLARE(uint32_t) base_core_media_bug_clear_flag(_In_ base_media_bug_t *bug, _In_ uint32_t flag);

/*!
\brief Set a return replace frame
\param bug the bug to set the frame on
\param frame the frame to set
*/
BASE_DECLARE(void) base_core_media_bug_set_read_replace_frame(_In_ base_media_bug_t *bug, _In_ base_frame_t *frame);

BASE_DECLARE(uint32_t) base_core_cpu_count(void);

/*!
\brief Remove a media bug from the session
\param session the session to remove the bug from
\param bug bug to remove
\return BASE_STATUS_SUCCESS if the operation was a success
*/

BASE_DECLARE(base_status_t) base_core_media_bug_remove(_In_ base_core_session_t *session, _Inout_ base_media_bug_t **bug);
BASE_DECLARE(uint32_t) base_core_media_bug_prune(base_core_session_t *session);

/*!
\brief Remove media bug callback
\param bug bug to remove
\param callback callback to remove
\return BASE_STATUS_SUCCESS if the operation was a success
*/
BASE_DECLARE(base_status_t) base_core_media_bug_remove_callback(base_core_session_t *session, base_media_bug_callback_t callback);

/*!
\brief Close and destroy a media bug
\param bug bug to remove
\return BASE_STATUS_SUCCESS if the operation was a success
*/
BASE_DECLARE(base_status_t) base_core_media_bug_close(_Inout_ base_media_bug_t **bug, base_bool_t destroy);
/*!
\brief Remove all media bugs from the session
\param session the session to remove the bugs from
\return BASE_STATUS_SUCCESS if the operation was a success
*/
BASE_DECLARE(base_status_t) base_core_media_bug_remove_all_function(_In_ base_core_session_t *session, const char *function);

#define base_core_media_bug_remove_all(_s) base_core_media_bug_remove_all_function(_s, NULL)

BASE_DECLARE(base_status_t) base_core_media_bug_enumerate(base_core_session_t *session, base_stream_handle_t *stream);

BASE_DECLARE(base_status_t) base_core_media_bug_transfer_callback(base_core_session_t *orig_session, base_core_session_t *new_session,
    base_media_bug_callback_t callback, void * (*user_data_dup_func) (base_core_session_t *, void *));


/*!
\brief Read a frame from the bug
\param bug the bug to read from
\param frame the frame to write the data to
\return BASE_STATUS_SUCCESS if the operation was a success
*/
BASE_DECLARE(base_status_t) base_core_media_bug_read(_In_ base_media_bug_t *bug, _In_ base_frame_t *frame, base_bool_t fill);

/*!
\brief Flush the read and write buffers for the bug
\param bug the bug to flush the read and write buffers on
*/
BASE_DECLARE(void) base_core_media_bug_flush(_In_ base_media_bug_t *bug);

/*!
\brief Flush the read/write buffers for all media bugs on the session
\param session the session to flush the read/write buffers for all media bugs on the session
*/
BASE_DECLARE(base_status_t) base_core_media_bug_flush_all(_In_ base_core_session_t *session);

BASE_DECLARE(base_status_t) base_core_media_bug_set_pre_buffer_framecount(base_media_bug_t *bug, uint32_t framecount);

///\}

///\defgroup pa1 Port Allocation
///\ingroup core1
///\{

/*!
\brief Initilize the port allocator
\param start the starting port
\param end the ending port
\param flags flags to change allocator behaviour (e.g. only even/odd portnumbers)
\param new_allocator new pointer for the return value
\return BASE_STATUS_SUCCESS if the operation was a success
*/
BASE_DECLARE(base_status_t) base_core_port_allocator_new(_In_ const char *ip,
    _In_ base_port_t start,
    _In_ base_port_t end,
    _In_ base_port_flag_t flags, _Out_ base_core_port_allocator_t **new_allocator);

/*!
\brief Get a port from the port allocator
\param alloc the allocator object
\param port_ptr a pointer to the port
\return SUCCESS
*/
BASE_DECLARE(base_status_t) base_core_port_allocator_request_port(_In_ base_core_port_allocator_t *alloc, _Out_ base_port_t *port_ptr);

/*!
\brief Return unused port to the port allocator
\param alloc the allocator object
\param port the port
\return SUCCESS
*/
BASE_DECLARE(base_status_t) base_core_port_allocator_free_port(_In_ base_core_port_allocator_t *alloc, _In_ base_port_t port);

/*!
\brief destroythe port allocator
\param alloc the allocator object
*/
BASE_DECLARE(void) base_core_port_allocator_destroy(_Inout_ base_core_port_allocator_t **alloc);
///\}


BASE_DECLARE(int) base_core_test_flag(int flag);

///\defgroup ss Startup/Shutdown
///\ingroup core1
///\{
/*!
\brief Initilize the core
\param console optional FILE stream for output
\param flags core flags
\param err a pointer to set any errors to
\note to be called at application startup
*/
BASE_DECLARE(base_status_t) base_core_init(_In_ base_core_flag_t flags, _In_ base_bool_t console, _Out_ const char **err);

/*!
\brief Initilize the core and load modules
\param console optional FILE stream for output
\param flags core flags
\param err a pointer to set any errors to
\note to be called at application startup instead of base_core_init.  Includes module loading.
*/
BASE_DECLARE(base_status_t) base_core_init_and_modload(_In_ base_core_flag_t flags, _In_ base_bool_t console, _Out_ const char **err);

/*!
\brief Set/Get Session Limit
\param new_limit new value (if > 0)
\return the current session limit
*/
BASE_DECLARE(uint32_t) base_core_session_limit(_In_ uint32_t new_limit);

/*!
\brief Set/Get Session Rate Limit
\param new_limit new value (if > 0)
\return the current session rate limit
*/
BASE_DECLARE(uint32_t) base_core_sessions_per_second(_In_ uint32_t new_limit);

/*!
\brief Destroy the core
\note to be called at application shutdown
*/
BASE_DECLARE(base_status_t) base_core_destroy(void);
///\}


///\defgroup rwl Read/Write Locking
///\ingroup core1
///\{


BASE_DECLARE(base_status_t) base_core_session_io_read_lock(base_core_session_t *session);
BASE_DECLARE(base_status_t) base_core_session_io_write_lock(base_core_session_t *session);
BASE_DECLARE(base_status_t) base_core_session_io_rwunlock(base_core_session_t *session);

#ifdef BASE_DEBUG_RWLOCKS
BASE_DECLARE(base_status_t) base_core_session_perform_read_lock(_In_ base_core_session_t *session, const char *file, const char *func, int line);
#endif

/*!
\brief Acquire a read lock on the session
\param session the session to acquire from
\return success if it is safe to read from the session
*/
#ifdef BASE_DEBUG_RWLOCKS
#define base_core_session_read_lock(session) base_core_session_perform_read_lock(session, __FILE__, __BASE_FUNC__, __LINE__)
#else
BASE_DECLARE(base_status_t) base_core_session_read_lock(_In_ base_core_session_t *session);
#endif


#ifdef BASE_DEBUG_RWLOCKS
BASE_DECLARE(base_status_t) base_core_session_perform_read_lock_hangup(_In_ base_core_session_t *session, const char *file, const char *func,
    int line);
#endif

/*!
\brief Acquire a read lock on the session
\param session the session to acquire from
\return success if it is safe to read from the session
*/
#ifdef BASE_DEBUG_RWLOCKS
#define base_core_session_read_lock_hangup(session) base_core_session_perform_read_lock_hangup(session, __FILE__, __BASE_FUNC__, __LINE__)
#else
BASE_DECLARE(base_status_t) base_core_session_read_lock_hangup(_In_ base_core_session_t *session);
#endif


#ifdef BASE_DEBUG_RWLOCKS
BASE_DECLARE(void) base_core_session_perform_write_lock(_In_ base_core_session_t *session, const char *file, const char *func, int line);
#endif

/*!
\brief Acquire a write lock on the session
\param session the session to acquire from
*/
#ifdef BASE_DEBUG_RWLOCKS
#define base_core_session_write_lock(session) base_core_session_perform_write_lock(session, __FILE__, __BASE_FUNC__, __LINE__)
#else
BASE_DECLARE(void) base_core_session_write_lock(_In_ base_core_session_t *session);
#endif

#ifdef BASE_DEBUG_RWLOCKS
BASE_DECLARE(void) base_core_session_perform_rwunlock(_In_ base_core_session_t *session, const char *file, const char *func, int line);
#endif

/*!
\brief Unlock a read or write lock on as given session
\param session the session
*/
#ifdef BASE_DEBUG_RWLOCKS
#define base_core_session_rwunlock(session) base_core_session_perform_rwunlock(session, __FILE__, __BASE_FUNC__, __LINE__)
#else
BASE_DECLARE(void) base_core_session_rwunlock(_In_ base_core_session_t *session);
#endif

///\}

///\defgroup sh State Handlers
///\ingroup core1
///\{
/*!
\brief Add a global state handler
\param state_handler a state handler to add
\return the current index/priority of this handler
*/
BASE_DECLARE(int) base_core_add_state_handler(_In_ const base_state_handler_table_t *state_handler);

/*!
\brief Remove a global state handler
\param state_handler the state handler to remove
*/
BASE_DECLARE(void) base_core_remove_state_handler(_In_ const base_state_handler_table_t *state_handler);

/*!
\brief Access a state handler
\param index the desired index to access
\return the desired state handler table or NULL when it does not exist.
*/
BASE_DECLARE(const base_state_handler_table_t *) base_core_get_state_handler(_In_ int index);
///\}

BASE_DECLARE(void) base_core_memory_pool_tag(base_memory_pool_t *pool, const char *tag);

BASE_DECLARE(void) base_core_pool_stats(base_stream_handle_t *stream);

BASE_DECLARE(base_status_t) base_core_perform_new_memory_pool(_Out_ base_memory_pool_t **pool,
    _In_z_ const char *file, _In_z_ const char *func, _In_ int line);

///\defgroup memp Memory Pooling/Allocation
///\ingroup core1
///\{
/*!
\brief Create a new sub memory pool from the core's master pool
\return BASE_STATUS_SUCCESS on success
*/
#define base_core_new_memory_pool(p) base_core_perform_new_memory_pool(p, __FILE__, __BASE_FUNC__, __LINE__)

BASE_DECLARE(int) base_core_session_sync_clock(void);
BASE_DECLARE(base_status_t) base_core_perform_destroy_memory_pool(_Inout_ base_memory_pool_t **pool,
    _In_z_ const char *file, _In_z_ const char *func, _In_ int line);
/*!
\brief Returns a subpool back to the main pool
\return BASE_STATUS_SUCCESS on success
*/
#define base_core_destroy_memory_pool(p) base_core_perform_destroy_memory_pool(p, __FILE__, __BASE_FUNC__, __LINE__)


BASE_DECLARE(void) base_core_memory_pool_set_data(base_memory_pool_t *pool, const char *key, void *data);
BASE_DECLARE(void *) base_core_memory_pool_get_data(base_memory_pool_t *pool, const char *key);


/*!
\brief Start the session's state machine
\param session the session on which to start the state machine
*/
BASE_DECLARE(void) base_core_session_run(_In_ base_core_session_t *session);

/*!
\brief determine if the session's state machine is running
\param session the session on which to check
*/
BASE_DECLARE(unsigned int) base_core_session_running(_In_ base_core_session_t *session);
BASE_DECLARE(unsigned int) base_core_session_started(_In_ base_core_session_t *session);

BASE_DECLARE(void *) base_core_perform_permanent_alloc(_In_ base_size_t memory, _In_z_ const char *file, _In_z_ const char *func, _In_ int line);


/*!
\brief Allocate memory from the main pool with no intention of returning it
\param _memory the number of bytes to allocate
\return a void pointer to the allocated memory
\note this memory never goes out of scope until the core is destroyed
*/
#define base_core_permanent_alloc(_memory) base_core_perform_permanent_alloc(_memory, __FILE__, __BASE_FUNC__, __LINE__)


BASE_DECLARE(void *) base_core_perform_alloc(_In_ base_memory_pool_t *pool, _In_ base_size_t memory, _In_z_ const char *file,
    _In_z_ const char *func, _In_ int line);

/*!
\brief Allocate memory directly from a memory pool
\param _pool the memory pool to allocate from
\param _mem the number of bytes to allocate
\remark the memory returned has been memset to zero
\return a void pointer to the allocated memory
*/
#define base_core_alloc(_pool, _mem) base_core_perform_alloc(_pool, _mem, __FILE__, __BASE_FUNC__, __LINE__)

_Ret_ BASE_DECLARE(void *) base_core_perform_session_alloc(_In_ base_core_session_t *session, _In_ base_size_t memory, const char *file,
    const char *func, int line);

/*!
\brief Allocate memory from a session's pool
\param _session the session to request memory from
\param _memory the amount of memory to allocate
\return a void pointer to the newly allocated memory
\note the memory will be in scope as long as the session exists
*/
#define base_core_session_alloc(_session, _memory) base_core_perform_session_alloc(_session, _memory, __FILE__, __BASE_FUNC__, __LINE__)



BASE_DECLARE(char *) base_core_perform_permanent_strdup(_In_z_ const char *todup, _In_z_ const char *file, _In_z_ const char *func, _In_ int line);

/*!
\brief Copy a string using permanent memory allocation
\param _todup the string to duplicate
\return a pointer to the newly duplicated string
*/
#define base_core_permanent_strdup(_todup) base_core_perform_permanent_strdup(_todup, __FILE__, __BASE_FUNC__, __LINE__)


BASE_DECLARE(char *) base_core_perform_session_strdup(_In_ base_core_session_t *session, _In_z_ const char *todup, _In_z_ const char *file,
    _In_z_ const char *func, _In_ int line);

/*!
\brief Copy a string using memory allocation from a session's pool
\param _session a session to use for allocation
\param _todup the string to duplicate
\return a pointer to the newly duplicated string
*/
#define base_core_session_strdup(_session, _todup) base_core_perform_session_strdup(_session, _todup, __FILE__, __BASE_FUNC__, __LINE__)


BASE_DECLARE(char *) base_core_perform_strdup(_In_ base_memory_pool_t *pool, _In_z_ const char *todup, _In_z_ const char *file,
    _In_z_ const char *func, _In_ int line);
BASE_DECLARE(char *) base_core_perform_strndup(_In_ base_memory_pool_t *pool, _In_z_ const char *todup, size_t len, _In_z_ const char *file,
    _In_z_ const char *func, _In_ int line);

/*!
\brief Copy a string using memory allocation from a given pool
\param _pool the pool to use for allocation
\param _todup the string to duplicate
\return a pointer to the newly duplicated string
*/
#define base_core_strdup(_pool, _todup)  base_core_perform_strdup(_pool, _todup, __FILE__, __BASE_FUNC__, __LINE__)

#define base_core_strndup(_pool, _todup, _len)  base_core_perform_strndup(_pool, _todup, _len, __FILE__, __BASE_FUNC__, __LINE__)

/*!
\brief printf-style style printing routine.  The data is output to a string allocated from the session
\param session a session to use for allocation
\param fmt The format of the string
\param ... The arguments to use while printing the data
\return The new string
*/
BASE_DECLARE(char *) base_core_session_sprintf(_In_ base_core_session_t *session, _In_z_ _Printf_format_string_ const char *fmt, ...);

/*!
\brief printf-style style printing routine.  The data is output to a string allocated from the session
\param session a session to use for allocation
\param fmt The format of the string
\param ap The arguments to use while printing the data
\return The new string
*/
#ifndef SWIG
BASE_DECLARE(char *) base_core_session_vsprintf(base_core_session_t *session, const char *fmt, va_list ap);
#endif

/*!
\brief printf-style style printing routine.  The data is output to a string allocated from the pool
\param pool a pool to use for allocation
\param fmt The format of the string
\param ... The arguments to use while printing the data
\return The new string
*/
BASE_DECLARE(char *) base_core_sprintf(_In_ base_memory_pool_t *pool, _In_z_ _Printf_format_string_ const char *fmt, ...);

/*!
\brief printf-style style printing routine.  The data is output to a string allocated from the pool
\param pool a pool to use for allocation
\param fmt The format of the string
\param ap The arguments to use while printing the data
\return The new string
*/
#ifndef SWIG
BASE_DECLARE(char *) base_core_vsprintf(base_memory_pool_t *pool, _In_z_ _Printf_format_string_ const char *fmt, va_list ap);
#endif

/*!
\brief Retrieve the memory pool from a session
\param session the session to retrieve the pool from
\return the session's pool
\note to be used sparingly
*/
BASE_DECLARE(base_memory_pool_t *) base_core_session_get_pool(_In_ base_core_session_t *session);
///\}

BASE_DECLARE(base_core_session_t *) base_core_session_request_xml(base_endpoint_interface_t *endpoint_interface,
    base_memory_pool_t **pool, base_xml_t xml);

///\defgroup sessm Session Creation / Management
///\ingroup core1
///\{
/*!
\brief Allocate and return a new session from the core
\param endpoint_interface the endpoint interface the session is to be based on
\param pool the pool to use for the allocation (a new one will be used if NULL)
\return the newly created session
*/
BASE_DECLARE(base_core_session_t *) base_core_session_request_uuid(_In_ base_endpoint_interface_t *endpoint_interface,
    _In_ base_call_direction_t direction,
    base_originate_flag_t originate_flags,
    _Inout_opt_ base_memory_pool_t **pool,
    _In_opt_z_ const char *use_uuid);
#define base_core_session_request(_ep, _d, _f, _p) base_core_session_request_uuid(_ep, _d, _f, _p, NULL)

BASE_DECLARE(base_status_t) base_core_session_set_uuid(_In_ base_core_session_t *session, _In_z_ const char *use_uuid);

BASE_DECLARE(base_status_t) base_core_session_set_external_id(_In_ base_core_session_t *session, _In_z_ const char *use_external_id);

BASE_DECLARE(void) base_core_session_perform_destroy(_Inout_ base_core_session_t **session,
    _In_z_ const char *file, _In_z_ const char *func, _In_ int line);

/*!
\brief Destroy a session and return the memory pool to the core
\param session pointer to a pointer of the session to destroy
\return
*/
#define base_core_session_destroy(session) base_core_session_perform_destroy(session, __FILE__, __BASE_FUNC__, __LINE__)

BASE_DECLARE(void) base_core_session_destroy_state(base_core_session_t *session);
BASE_DECLARE(void) base_core_session_reporting_state(base_core_session_t *session);
BASE_DECLARE(void) base_core_session_hangup_state(base_core_session_t *session, base_bool_t force);

/*!
\brief Provide the total number of sessions
\return the total number of allocated sessions
*/
BASE_DECLARE(uint32_t) base_core_session_count(void);

BASE_DECLARE(base_size_t) base_core_session_get_id(_In_ base_core_session_t *session);

/*!
\brief Provide the current session_id
\return the total number of allocated sessions since core startup
*/
BASE_DECLARE(base_size_t) base_core_session_id(void);
BASE_DECLARE(base_size_t) base_core_session_id_dec(void);

/*!
\brief Allocate and return a new session from the core based on a given endpoint module name
\param endpoint_name the name of the endpoint module
\param pool the pool to use
\return the newly created session
*/
BASE_DECLARE(base_core_session_t *) base_core_session_request_by_name(_In_z_ const char *endpoint_name,
    _In_ base_call_direction_t direction, _Inout_ base_memory_pool_t **pool);

/*!
\brief Launch the session thread (state machine) on a given session
\param session the session to activate the state machine on
\return BASE_STATUS_SUCCESS if the thread was launched
*/
BASE_DECLARE(base_status_t) base_core_session_thread_launch(_In_ base_core_session_t *session);


BASE_DECLARE(base_status_t) base_thread_pool_launch_thread(base_thread_data_t **tdp);
BASE_DECLARE(base_status_t) base_core_session_thread_pool_launch(base_core_session_t *session);
BASE_DECLARE(base_status_t) base_thread_pool_wait(base_thread_data_t *td, int ms);

/*!
\brief Retrieve a pointer to the channel object associated with a given session
\param session the session to retrieve from
\return a pointer to the channel object
*/
_Ret_ BASE_DECLARE(base_channel_t *) base_core_session_get_channel(_In_ base_core_session_t *session);

/*!
\brief Retrieve the unique identifier from a session
\param session the session to retrieve the uuid from
\return a string representing the uuid
*/
BASE_DECLARE(char *) base_core_session_get_uuid(_In_ base_core_session_t *session);

/*!
\brief Retrieve the unique external identifier from a session
\param session the session to retrieve the uuid from
\return a string representing the uuid
*/
BASE_DECLARE(const char *) base_core_session_get_external_id(_In_ base_core_session_t *session);


/*!
\brief Sets the log level for a session
\param session the session to set the log level on
\return BASE_STATUS_SUCCESS
*/
BASE_DECLARE(base_status_t) base_core_session_set_loglevel(base_core_session_t *session, base_log_level_t loglevel);


/*!
\brief Get the log level for a session
\param session the session to get the log level from
\return the log level
*/
BASE_DECLARE(base_log_level_t) base_core_session_get_loglevel(base_core_session_t *session);

BASE_DECLARE(base_jb_t *) base_core_session_get_jb(base_core_session_t *session, base_media_type_t type);
BASE_DECLARE(void) base_core_session_soft_lock(base_core_session_t *session, uint32_t sec);
BASE_DECLARE(void) base_core_session_soft_unlock(base_core_session_t *session);
BASE_DECLARE(void) base_core_session_set_dmachine(base_core_session_t *session, base_ivr_dmachine_t *dmachine, base_digit_action_target_t target);
BASE_DECLARE(base_ivr_dmachine_t *) base_core_session_get_dmachine(base_core_session_t *session, base_digit_action_target_t target);
BASE_DECLARE(base_digit_action_target_t) base_ivr_dmachine_get_target(base_ivr_dmachine_t *dmachine);
BASE_DECLARE(void) base_ivr_dmachine_set_target(base_ivr_dmachine_t *dmachine, base_digit_action_target_t target);
BASE_DECLARE(base_status_t) base_ivr_dmachine_set_terminators(base_ivr_dmachine_t *dmachine, const char *terminators);
BASE_DECLARE(base_status_t) base_core_session_set_codec_slin(base_core_session_t *session, base_slin_data_t *data);
BASE_DECLARE(void) base_core_session_raw_read(base_core_session_t *session);

/*!
\brief Retrieve the unique identifier from the core
\return a string representing the uuid
*/
BASE_DECLARE(char *) base_core_get_uuid(void);


BASE_DECLARE(base_core_session_t *) base_core_session_perform_locate(const char *uuid_str, const char *file, const char *func, int line);
BASE_DECLARE(base_core_session_t *) base_core_session_perform_force_locate(const char *uuid_str, const char *file, const char *func, int line);


/*!
\brief Locate a session based on it's uuid
\param uuid_str the unique id of the session you want to find
\return the session or NULL
\note if the session was located it will have a read lock obtained which will need to be released with base_core_session_rwunlock()
*/

#define base_core_session_locate(uuid_str) base_core_session_perform_locate(uuid_str, __FILE__, __BASE_FUNC__, __LINE__)

/*!
\brief Locate a session based on it's uuid even if the channel is not ready
\param uuid_str the unique id of the session you want to find
\return the session or NULL
\note if the session was located it will have a read lock obtained which will need to be released with base_core_session_rwunlock()
*/

#define base_core_session_force_locate(uuid_str) base_core_session_perform_force_locate(uuid_str, __FILE__, __BASE_FUNC__, __LINE__)


/*!
\brief Retrieve a global variable from the core
\param varname the name of the variable
\return the value of the desired variable
*/
BASE_DECLARE(char *) base_core_get_variable(_In_z_ const char *varname);
BASE_DECLARE(char *) base_core_get_variable_dup(_In_z_ const char *varname);
BASE_DECLARE(char *) base_core_get_variable_pdup(_In_z_ const char *varname, base_memory_pool_t *pool);
BASE_DECLARE(const char *) base_core_get_hostname(void);
BASE_DECLARE(const char *) base_core_get_basename(void);

BASE_DECLARE(char *) base_core_get_domain(base_bool_t dup);

/*!
\brief Add a global variable to the core
\param varname the name of the variable
\param value the value of the variable
\     If \p value is NULL, then \p varname is deleted.
*/
BASE_DECLARE(void) base_core_set_variable(_In_z_ const char *varname, _In_opt_z_ const char *value);
BASE_DECLARE(base_status_t) base_core_get_variables(base_event_t **event);

/*!
\brief Conditionally add a global variable to the core
\param varname the name of the variable
\param value the value of the variable
\param val2 the value of the variable to verify against
\     If the global did not exist and val2=="", add global with value, return true
\     If the global exists with the value of val2, replace it, return true
\     If the global exists with a value other than val2, return false
*/
BASE_DECLARE(base_bool_t) base_core_set_var_conditional(_In_z_ const char *varname, _In_opt_z_ const char *value, _In_opt_z_ const char *val2);

BASE_DECLARE(void) base_core_dump_variables(_In_ base_stream_handle_t *stream);

/*!
\brief Hangup all sessions
\param cause the hangup cause to apply to the hungup channels
*/
BASE_DECLARE(void) base_core_session_hupall(_In_ base_call_cause_t cause);

typedef enum {
    SHT_NONE = 0,
    SHT_UNANSWERED = (1 << 0),
    SHT_ANSWERED = (1 << 1)
} base_hup_type_t;

/*!
\brief Hangup all sessions which match a specific channel variable
\param var_name The variable name to look for
\param var_val The value to look for
\param cause the hangup cause to apply to the hungup channels
*/
BASE_DECLARE(uint32_t) base_core_session_hupall_matching_var_ans(_In_ const char *var_name, _In_ const char *var_val, _In_
    base_call_cause_t cause, base_hup_type_t type);
BASE_DECLARE(base_console_callback_match_t *) base_core_session_findall_matching_var(const char *var_name, const char *var_val);
#define base_core_session_hupall_matching_var(_vn, _vv, _c) base_core_session_hupall_matching_var_ans(_vn, _vv, _c, SHT_UNANSWERED | SHT_ANSWERED)
BASE_DECLARE(base_console_callback_match_t *) base_core_session_findall(void);
/*!
\brief Hangup all sessions which match specific channel variable(s)
\param var_name The variable name to look for
\param var_val The value to look for
\param cause the hangup cause to apply to the hungup channels
*/
BASE_DECLARE(uint32_t) base_core_session_hupall_matching_vars_ans(_In_ base_event_t *vars, _In_
    base_call_cause_t cause, base_hup_type_t type);
#define base_core_session_hupall_matching_vars(_vs, _c) base_core_session_hupall_matching_vars_ans(_vs, _c, SHT_UNANSWERED | SHT_ANSWERED)

/*!
\brief Hangup all sessions that belong to an endpoint
\param endpoint_interface The endpoint interface
\param cause the hangup cause to apply to the hungup channels
*/
BASE_DECLARE(void) base_core_session_hupall_endpoint(const base_endpoint_interface_t *endpoint_interface, base_call_cause_t cause);

/*!
\brief Get the session's partner (the session its bridged to)
\param session The session we're searching with
\param partner [out] The session's partner, or NULL if it wasnt found
\return BASE_STATUS_SUCCESS or BASE_STATUS_FALSE if this session isn't bridged
*/
BASE_DECLARE(base_status_t) base_core_session_perform_get_partner(base_core_session_t *session, base_core_session_t **partner,
    const char *file, const char *func, int line);

#define base_core_session_get_partner(_session, _partner) base_core_session_perform_get_partner(_session, _partner, __FILE__, __BASE_FUNC__, __LINE__)

/*!
\brief Send a message to another session using it's uuid
\param uuid_str the unique id of the session you want to send a message to
\param message the base_core_session_message_t object to send
\return the status returned by the message handler
*/
BASE_DECLARE(base_status_t) base_core_session_message_send(_In_z_ const char *uuid_str, _In_ base_core_session_message_t *message);

/*!
\brief Queue a message on a session
\param session the session to queue the message to
\param message the message to queue
\return BASE_STATUS_SUCCESS if the message was queued
*/
BASE_DECLARE(base_status_t) base_core_session_queue_message(_In_ base_core_session_t *session, _In_ base_core_session_message_t *message);

BASE_DECLARE(void) base_core_session_free_message(base_core_session_message_t **message);


BASE_DECLARE(base_status_t) base_core_session_queue_signal_data(base_core_session_t *session, void *signal_data);
BASE_DECLARE(base_status_t) base_core_session_dequeue_signal_data(base_core_session_t *session, void **signal_data);

/*!
\brief pass an indication message on a session
\param session the session to pass the message across
\param indication the indication message to pass
\return BASE_STATUS_SUCCESS if the message was passed
*/
BASE_DECLARE(base_status_t) base_core_session_pass_indication(_In_ base_core_session_t *session,
    _In_ base_core_session_message_types_t indication);

/*!
\brief Queue an indication message on a session
\param session the session to queue the message to
\param indication the indication message to queue
\return BASE_STATUS_SUCCESS if the message was queued
*/
BASE_DECLARE(base_status_t) base_core_session_queue_indication(_In_ base_core_session_t *session,
    _In_ base_core_session_message_types_t indication);

/*!
\brief DE-Queue an message on a given session
\param session the session to de-queue the message on
\param message the de-queued message
\return BASE_STATUS_SUCCESS if the message was de-queued
*/
BASE_DECLARE(base_status_t) base_core_session_dequeue_message(_In_ base_core_session_t *session, _Out_ base_core_session_message_t **message);

/*!
\brief Flush a message queue on a given session
\param session the session to de-queue the message on
\return BASE_STATUS_SUCCESS if the message was de-queued
*/
BASE_DECLARE(base_status_t) base_core_session_flush_message(_In_ base_core_session_t *session);

/*!
\brief Queue an event on another session using its uuid
\param uuid_str the unique id of the session you want to send a message to
\param event the event to send
\return the status returned by the message handler
*/
BASE_DECLARE(base_status_t) base_core_session_event_send(_In_z_ const char *uuid_str, _Inout_ base_event_t **event);

BASE_DECLARE(base_app_log_t *) base_core_session_get_app_log(_In_ base_core_session_t *session);

/*!
\brief Execute an application on a session
\param session the current session
\param application_interface the interface of the application to execute
\param arg application arguments
\warning Has to be called from the session's thread
\return the application's return value
*/
BASE_DECLARE(base_status_t) base_core_session_exec(_In_ base_core_session_t *session,
    _In_ const base_application_interface_t *application_interface, _In_opt_z_ const char *arg);

BASE_DECLARE(void) base_core_session_video_reset(base_core_session_t *session);
/*!
\brief Execute an application on a session
\param session the current session
\param app the application's name
\param arg application arguments
\param flags pointer to a flags variable to set the applications flags to
\return the application's return value
*/
BASE_DECLARE(base_status_t) base_core_session_execute_application_get_flags(_In_ base_core_session_t *session,
    _In_ const char *app, _In_opt_z_ const char *arg, _Out_opt_ int32_t *flags);

BASE_DECLARE(base_status_t) base_core_session_execute_application_async(base_core_session_t *session, const char *app, const char *arg);

BASE_DECLARE(base_status_t) base_core_session_get_app_flags(const char *app, int32_t *flags);

/*!
\brief Execute an application on a session
\param session the current session
\param app the application's name
\param arg application arguments
\return the application's return value
*/
#define base_core_session_execute_application(_a, _b, _c) base_core_session_execute_application_get_flags(_a, _b, _c, NULL)

BASE_DECLARE(uint32_t) base_core_session_stack_count(base_core_session_t *session, int x);

/*!
\brief Run a dialplan and execute an extension
\param session the current session
\param exten the interface of the application to execute
\param arg application arguments
\note It does not change the channel back to CS_ROUTING, it manually calls the dialplan and executes the applications
\warning Has to be called from the session's thread
\return the application's return value
*/
BASE_DECLARE(base_status_t) base_core_session_execute_exten(_In_ base_core_session_t *session,
    _In_z_ const char *exten,
    _In_opt_z_ const char *dialplan, _In_opt_z_ const char *context);

/*!
\brief Send an event to a session translating it to it's native message format
\param session the session to receive the event
\param event the event to receive
\return the status returned by the handler
*/
BASE_DECLARE(base_status_t) base_core_session_receive_event(_In_ base_core_session_t *session, _Inout_ base_event_t **event);

/*!
\brief Retrieve private user data from a session
\param session the session to retrieve from
\return a pointer to the private data
*/
BASE_DECLARE(void *) base_core_session_get_private_class(_In_ base_core_session_t *session, _In_ base_pvt_class_t index);
#define base_core_session_get_private(_s) base_core_session_get_private_class(_s, BASE_PVT_PRIMARY)

/*!
\brief Add private user data to a session
\param session the session to add used data to
\param private_info the used data to add
\return BASE_STATUS_SUCCESS if data is added
*/
BASE_DECLARE(base_status_t) base_core_session_set_private_class(_In_ base_core_session_t *session, _In_ void *private_info, _In_ base_pvt_class_t index);
#define base_core_session_set_private(_s, _p) base_core_session_set_private_class(_s, _p, BASE_PVT_PRIMARY)

/*!
\brief Add a logical stream to a session
\param session the session to add the stream to
\param private_info an optional pointer to private data for the new stream
\return the stream id of the new stream
*/
BASE_DECLARE(int) base_core_session_add_stream(_In_ base_core_session_t *session, _In_opt_ void *private_info);

/*!
\brief Retreive a logical stream from a session
\param session the session to add the stream to
\param index the index to retrieve
\return the stream
*/
BASE_DECLARE(void *) base_core_session_get_stream(_In_ base_core_session_t *session, _In_ int index);

/*!
\brief Determine the number of logical streams a session has
\param session the session to query
\return the total number of logical streams
*/
BASE_DECLARE(int) base_core_session_get_stream_count(_In_ base_core_session_t *session);

BASE_DECLARE(const char *) base_core_session_get_text_buffer(base_core_session_t *session);

/*!
\brief Signal a thread using a thread session to terminate
\param session the session to indicate to
*/
BASE_DECLARE(void) base_core_thread_session_end(_In_ base_core_session_t *session);

/*!
\brief Launch a service thread on a session to drop inbound data
\param session the session the launch thread on
*/
BASE_DECLARE(void) base_core_service_session_av(_In_ base_core_session_t *session, base_bool_t audio, base_bool_t video);
#define base_core_service_session(_s) base_core_service_session_av(_s, BASE_TRUE, BASE_FALSE)


/*!
\brief Request an outgoing session spawned from an existing session using a desired endpoing module
\param session the originating session
\param var_event base_event_t containing paramaters
\param endpoint_name the name of the module to use for the new session
\param caller_profile the originator's caller profile
\param new_session a NULL pointer to aim at the newly created session
\param pool optional existing memory pool to donate to the session
\param flags flags to use
\return the cause code of the attempted call
*/
BASE_DECLARE(base_call_cause_t) base_core_session_outgoing_channel(_In_opt_ base_core_session_t *session,
    _In_opt_ base_event_t *var_event,
    _In_z_ const char *endpoint_name,
    _In_ base_caller_profile_t *caller_profile,
    _Inout_ base_core_session_t **new_session,
    _Inout_ base_memory_pool_t **pool, _In_ base_originate_flag_t flags,
    base_call_cause_t *cancel_cause);

/*!
\brief Receive a message on a given session
\param session the session to receive the message from
\param message the message to recieve
\return the status returned by the message handler
*/
BASE_DECLARE(base_status_t) base_core_session_perform_receive_message(_In_ base_core_session_t *session,
    _In_ base_core_session_message_t *message,
    const char *file, const char *func, int line);
#define base_core_session_receive_message(_session, _message) base_core_session_perform_receive_message(_session, _message, \
																											__FILE__, __BASE_FUNC__, __LINE__)

/*!
\brief Queue an event on a given session
\param session the session to queue the message on
\param event the event to queue
\return the status returned by the message handler
*/
BASE_DECLARE(base_status_t) base_core_session_queue_event(_In_ base_core_session_t *session, _Inout_ base_event_t **event);


/*!
\brief Indicate the number of waiting events on a session
\param session the session to check
\return the number of events
*/
BASE_DECLARE(uint32_t) base_core_session_event_count(_In_ base_core_session_t *session);

/*
Number of parsable messages waiting on the session.
*/
BASE_DECLARE(uint32_t) base_core_session_messages_waiting(base_core_session_t *session);

/*!
\brief DE-Queue an event on a given session
\param session the session to de-queue the message on
\param event the de-queued event
\param force force the dequeue
\return the  BASE_STATUS_SUCCESS if the event was de-queued
*/
BASE_DECLARE(base_status_t) base_core_session_dequeue_event(_In_ base_core_session_t *session, _Out_ base_event_t **event, base_bool_t force);

/*!
\brief Queue a private event on a given session
\param session the session to queue the message on
\param event the event to queue
\param priority event has high priority
\return the status returned by the message handler
*/
BASE_DECLARE(base_status_t) base_core_session_queue_private_event(_In_ base_core_session_t *session, _Inout_ base_event_t **event,
    base_bool_t priority);


/*!
\brief Indicate the number of waiting private events on a session
\param session the session to check
\return the number of events
*/
BASE_DECLARE(uint32_t) base_core_session_private_event_count(_In_ base_core_session_t *session);

/*!
\brief DE-Queue a private event on a given session
\param session the session to de-queue the message on
\param event the de-queued event
\return the  BASE_STATUS_SUCCESS if the event was de-queued
*/
BASE_DECLARE(base_status_t) base_core_session_dequeue_private_event(_In_ base_core_session_t *session, _Out_ base_event_t **event);


/*!
\brief Flush the private event queue of a session
\param session the session to flush
\return BASE_STATUS_SUCCESS if the events have been flushed
*/
BASE_DECLARE(uint32_t) base_core_session_flush_private_events(base_core_session_t *session);


/*!
\brief Read a frame from a session
\param session the session to read from
\param frame a NULL pointer to a frame to aim at the newly read frame
\param flags I/O flags to modify behavior (i.e. non blocking)
\param stream_id which logical media channel to use
\return BASE_STATUS_SUCCESS a the frame was read
*/
BASE_DECLARE(base_status_t) base_core_session_read_frame(_In_ base_core_session_t *session, base_frame_t **frame, base_io_flag_t flags,
    int stream_id);

BASE_DECLARE(base_bool_t) base_core_session_transcoding(base_core_session_t *session_a, base_core_session_t *session_b, base_media_type_t type);
BASE_DECLARE(void) base_core_session_passthru(base_core_session_t *session, base_media_type_t type, base_bool_t on);

/*!
\brief Read a video frame from a session
\param session the session to read from
\param frame a NULL pointer to a frame to aim at the newly read frame
\param flags I/O flags to modify behavior (i.e. non blocking)
\param stream_id which logical media channel to use
\return BASE_STATUS_SUCCESS a if the frame was read
*/
BASE_DECLARE(base_status_t) base_core_session_read_video_frame(_In_ base_core_session_t *session, base_frame_t **frame, base_io_flag_t flags,
    int stream_id);
/*!
\brief Write a video frame to a session
\param session the session to write to
\param frame a pointer to a frame to write
\param flags I/O flags to modify behavior (i.e. non blocking)
\param stream_id which logical media channel to use
\return BASE_STATUS_SUCCESS a if the frame was written
*/
BASE_DECLARE(base_status_t) base_core_session_write_video_frame(_In_ base_core_session_t *session, base_frame_t *frame, base_io_flag_t flags,
    int stream_id);

BASE_DECLARE(base_status_t) base_core_session_write_encoded_video_frame(base_core_session_t *session,
    base_frame_t *frame, base_io_flag_t flags, int stream_id);

BASE_DECLARE(base_status_t) base_core_session_set_read_impl(base_core_session_t *session, const base_codec_implementation_t *impp);
BASE_DECLARE(base_status_t) base_core_session_set_write_impl(base_core_session_t *session, const base_codec_implementation_t *impp);
BASE_DECLARE(base_status_t) base_core_session_set_video_read_impl(base_core_session_t *session, const base_codec_implementation_t *impp);
BASE_DECLARE(base_status_t) base_core_session_set_video_write_impl(base_core_session_t *session, const base_codec_implementation_t *impp);

/*!
\brief Reset the buffers and resampler on a session
\param session the session to reset
\param flush_dtmf flush all queued dtmf events too
*/
BASE_DECLARE(void) base_core_session_reset(_In_ base_core_session_t *session, base_bool_t flush_dtmf, base_bool_t reset_read_codec);

/*!
\brief Reset the buffers and resampler on a session, fail if can not lock codec mutexes
\param session the session to reset
\param flush_dtmf flush all queued dtmf events too
\return BASE_STATUS_SUCCESS if the session was reset
*/
BASE_DECLARE(base_status_t) base_core_session_try_reset(base_core_session_t* session, base_bool_t flush_dtmf, base_bool_t reset_read_codec);

/*!
\brief Write a frame to a session
\param session the session to write to
\param frame the frame to write
\param flags I/O flags to modify behavior (i.e. non blocking)
\param stream_id which logical media channel to use
\return BASE_STATUS_SUCCESS a the frame was written
*/
BASE_DECLARE(base_status_t) base_core_session_write_frame(_In_ base_core_session_t *session, base_frame_t *frame, base_io_flag_t flags,
    int stream_id);


BASE_DECLARE(base_status_t) base_core_session_perform_kill_channel(_In_ base_core_session_t *session,
    const char *file, const char *func, int line, base_signal_t sig);
/*!
\brief Send a signal to a channel
\param session session to send signal to
\param sig signal to send
\return status returned by the session's signal handler
*/
#define base_core_session_kill_channel(session, sig) base_core_session_perform_kill_channel(session, __FILE__, __BASE_FUNC__, __LINE__, sig)

/*!
\brief Send DTMF to a session
\param session session to send DTMF to
\param dtmf dtmf to send to the session
\return BASE_STATUS_SUCCESS if the dtmf was written
*/
BASE_DECLARE(base_status_t) base_core_session_send_dtmf(_In_ base_core_session_t *session, const base_dtmf_t *dtmf);
/*!
\brief Send DTMF to a session
\param session session to send DTMF to
\param dtmf_string string to send to the session
\return BASE_STATUS_SUCCESS if the dtmf was written
*/
BASE_DECLARE(base_status_t) base_core_session_send_dtmf_string(base_core_session_t *session, const char *dtmf_string);

/*!
\brief RECV DTMF on a session
\param session session to recv DTMF from
\param dtmf string to recv from the session
\return BASE_STATUS_SUCCESS if the dtmf is ok to queue
*/
BASE_DECLARE(base_status_t) base_core_session_recv_dtmf(_In_ base_core_session_t *session, const base_dtmf_t *dtmf);

///\}


///\defgroup hashf Hash Functions
///\ingroup core1
///\{
/*!
\brief Initialize a hash table
\param hash a NULL pointer to a hash table to aim at the new hash
\param pool the pool to use for the new hash
\return BASE_STATUS_SUCCESS if the hash is created
*/
BASE_DECLARE(base_status_t) base_core_hash_init_case(_Out_ base_hash_t **hash, base_bool_t case_sensitive);
#define base_core_hash_init(_hash) base_core_hash_init_case(_hash, BASE_TRUE)
#define base_core_hash_init_nocase(_hash) base_core_hash_init_case(_hash, BASE_FALSE)


/*!
\brief Destroy an existing hash table
\param hash the hash to destroy
\return BASE_STATUS_SUCCESS if the hash is destroyed
*/
BASE_DECLARE(base_status_t) base_core_hash_destroy(_Inout_ base_hash_t **hash);

/*!
\brief Insert data into a hash with an auto-generated key based on the data pointer
\param hash the hash to add data to
\param data unique pointer to add
\return BASE_STATUS_SUCCESS if the data is added
*/
BASE_DECLARE(base_status_t) base_core_hash_insert_pointer(base_hash_t *hash, const void *data);

/*!
\brief Insert data into a hash and set flags so the value is automatically freed on delete
\param hash the hash to add data to
\param key the name of the key to add the data to
\param data the data to add
\return BASE_STATUS_SUCCESS if the data is added
\note the string key must be a constant or a dynamic string
*/
BASE_DECLARE(base_status_t) base_core_hash_insert_auto_free(base_hash_t *hash, const char *key, const void *data);

/*!
\brief Insert strdup(str) into a hash and set flags so the value is automatically freed on delete
\param hash the hash to add str to
\param key the name of the key to add the str to
\param str string to strdup and add
\return BASE_STATUS_SUCCESS if the data is added
\note the string key must be a constant or a dynamic string
*/
BASE_DECLARE(base_status_t) base_core_hash_insert_dup_auto_free(base_hash_t *hash, const char *key, const char *str);

/*!
\brief Insert data into a hash
\param hash the hash to add data to
\param key the name of the key to add the data to
\param data the data to add
\return BASE_STATUS_SUCCESS if the data is added
\note the string key must be a constant or a dynamic string
*/
BASE_DECLARE(base_status_t) base_core_hash_insert_destructor(_In_ base_hash_t *hash, _In_z_ const char *key, _In_opt_ const void *data, hashtable_destructor_t destructor);
#define base_core_hash_insert(_h, _k, _d) base_core_hash_insert_destructor(_h, _k, _d, NULL)

/*!
\brief Allocate memory and insert into a hash
\param hash the hash to add data to
\param key the name of the key to add the data to
\param size the size in bytes to allocate
\return pointer to the allocated memory
\note the string key must be a constant or a dynamic string
*/
BASE_DECLARE(void *) base_core_hash_insert_alloc_destructor(_In_ base_hash_t *hash, _In_z_ const char *key, _In_opt_ size_t size, hashtable_destructor_t destructor);
#define base_core_hash_insert_alloc(_h, _k, _s) base_core_hash_insert_alloc_destructor(_h, _k, _s, NULL)

/*!
\brief Insert strdup(str) into a hash
\param hash the hash to add str to
\param key the name of the key to add the str to
\param str string to strdup and add
\return BASE_STATUS_SUCCESS if the data is added
\note the string key must be a constant or a dynamic string
*/
BASE_DECLARE(base_status_t) base_core_hash_insert_dup_destructor(_In_ base_hash_t *hash, _In_z_ const char *key, _In_opt_ const char *str, hashtable_destructor_t destructor);
#define base_core_hash_insert_dup(_h, _k, _d) base_core_hash_insert_dup_destructor(_h, _k, _d, NULL)


/*!
\brief Insert data into a hash
\param hash the hash to add data to
\param key the name of the key to add the data to
\param data the data to add
\param mutex optional mutex to lock
\return BASE_STATUS_SUCCESS if the data is added
\note the string key must be a constant or a dynamic string
*/
BASE_DECLARE(base_status_t) base_core_hash_insert_locked(_In_ base_hash_t *hash, _In_z_ const char *key, _In_opt_ const void *data,
    _In_opt_ base_mutex_t *mutex);
/*!
\brief Retrieve data from a given hash
\param hash the hash to retrieve from
\param key the key to retrieve
\param mutex optional rwlock to wrlock
\return a pointer to the data held in the key
*/
BASE_DECLARE(base_status_t) base_core_hash_insert_wrlock(base_hash_t *hash, const char *key, const void *data, base_thread_rwlock_t *rwlock);

/*!
\brief Delete data from a hash based on desired key
\param hash the hash to delete from
\param key the key from which to delete the data
\return The value stored if the data is deleted otherwise NULL
*/
BASE_DECLARE(void *) base_core_hash_delete(_In_ base_hash_t *hash, _In_z_ const char *key);

/*!
\brief Delete data from a hash based on desired key
\param hash the hash to delete from
\param key the key from which to delete the data
\param mutex optional mutex to lock
\return a pointer to the deleted data
*/
BASE_DECLARE(void *) base_core_hash_delete_locked(_In_ base_hash_t *hash, _In_z_ const char *key, _In_opt_ base_mutex_t *mutex);

/*!
\brief Delete data from a hash based on desired key
\param hash the hash to delete from
\param key the key from which to delete the data
\param mutex optional rwlock to wrlock
\return a pointer to the deleted data
*/
BASE_DECLARE(void *) base_core_hash_delete_wrlock(_In_ base_hash_t *hash, _In_z_ const char *key, _In_opt_ base_thread_rwlock_t *rwlock);

/*!
\brief Delete data from a hash based on callback function
\param hash the hash to delete from
\param callback the function to call which returns BASE_TRUE to delete, BASE_FALSE to preserve
\return BASE_STATUS_SUCCESS if any data is deleted
*/
BASE_DECLARE(base_status_t) base_core_hash_delete_multi(_In_ base_hash_t *hash, _In_ base_hash_delete_callback_t callback, _In_opt_ void *pData);

/*!
\brief Retrieve data from a given hash
\param hash the hash to retrieve from
\param key the key to retrieve
\return a pointer to the data held in the key
*/
BASE_DECLARE(void *) base_core_hash_find(_In_ base_hash_t *hash, _In_z_ const char *key);


/*!
\brief Retrieve data from a given hash
\param hash the hash to retrieve from
\param key the key to retrieve
\param mutex optional mutex to lock
\return a pointer to the data held in the key
*/
BASE_DECLARE(void *) base_core_hash_find_locked(_In_ base_hash_t *hash, _In_z_ const char *key, _In_ base_mutex_t *mutex);

/*!
\brief Retrieve data from a given hash
\param hash the hash to retrieve from
\param key the key to retrieve
\param mutex optional rwlock to rdlock
\return a pointer to the data held in the key
*/
BASE_DECLARE(void *) base_core_hash_find_rdlock(_In_ base_hash_t *hash, _In_z_ const char *key, _In_ base_thread_rwlock_t *rwlock);

/*!
\brief Gets the first element of a hashtable
\param deprecate_me [deprecated] NULL
\param hash the hashtable to use
\return The element, or NULL if it wasn't found
*/
BASE_DECLARE(base_hash_index_t *) base_core_hash_first_iter(_In_ base_hash_t *hash, base_hash_index_t *hi);
#define base_core_hash_first(_h) base_core_hash_first_iter(_h, NULL)

/*!
\brief tells if a hash is empty
\param hash the hashtable
\return TRUE or FALSE depending on if the hash is empty
*/
BASE_DECLARE(base_bool_t) base_core_hash_empty(base_hash_t *hash);

/*!
\brief Gets the next element of a hashtable
\param hi The current element
\return The next element, or NULL if there are no more
*/
BASE_DECLARE(base_hash_index_t *) base_core_hash_next(_In_ base_hash_index_t **hi);

/*!
\brief Gets the key and value of the current hash element
\param hi The current element
\param key [out] the key
\param klen [out] the key's size
\param val [out] the value
*/
BASE_DECLARE(void) base_core_hash_this(_In_ base_hash_index_t *hi, _Out_opt_ptrdiff_cap_(klen)
    const void **key, _Out_opt_ base_ssize_t *klen, _Out_ void **val);

BASE_DECLARE(void) base_core_hash_this_val(base_hash_index_t *hi, void *val);

BASE_DECLARE(base_status_t) base_core_inthash_init(base_inthash_t **hash);
BASE_DECLARE(base_status_t) base_core_inthash_destroy(base_inthash_t **hash);
BASE_DECLARE(base_status_t) base_core_inthash_insert(base_inthash_t *hash, uint32_t key, const void *data);
BASE_DECLARE(void *) base_core_inthash_delete(base_inthash_t *hash, uint32_t key);
BASE_DECLARE(void *) base_core_inthash_find(base_inthash_t *hash, uint32_t key);

///\}

///\defgroup timer Timer Functions
///\ingroup core1
///\{
/*!
\brief Request a timer handle using given time module
\param timer a timer object to allocate to
\param timer_name the name of the timer module to use
\param interval desired interval
\param samples the number of samples to increment on each cycle
\param pool the memory pool to use for allocation
\return
*/
BASE_DECLARE(base_status_t) base_core_timer_init(base_timer_t *timer, const char *timer_name, int interval, int samples,
    base_memory_pool_t *pool);

BASE_DECLARE(void) base_time_calibrate_clock(void);

/*!
\brief Wait for one cycle on an existing timer
\param timer the timer to wait on
\return the newest sample count
*/
BASE_DECLARE(base_status_t) base_core_timer_next(base_timer_t *timer);

/*!
\brief Step the timer one step
\param timer the timer to wait on
\return the newest sample count
*/
BASE_DECLARE(base_status_t) base_core_timer_step(base_timer_t *timer);

BASE_DECLARE(base_status_t) base_core_timer_sync(base_timer_t *timer);

/*!
\brief Check if the current step has been exceeded
\param timer the timer to wait on
\param step increment timer if a tick was detected
\return the newest sample count
*/
BASE_DECLARE(base_status_t) base_core_timer_check(base_timer_t *timer, base_bool_t step);

/*!
\brief Destroy an allocated timer
\param timer timer to destroy
\return BASE_STATUS_SUCCESS after destruction
*/
BASE_DECLARE(base_status_t) base_core_timer_destroy(base_timer_t *timer);
///\}

///\defgroup codecs Codec Functions
///\ingroup core1
///\{
/*!
\brief Initialize a codec handle
\param codec the handle to initilize
\param codec_name the name of the codec module to use
\param fmtp codec parameters to send
\param rate the desired rate (0 for any)
\param ms the desired number of milliseconds (0 for any)
\param channels the desired number of channels (0 for any)
\param flags flags to alter behaviour
\param codec_settings desired codec settings
\param pool the memory pool to use
\return BASE_STATUS_SUCCESS if the handle is allocated
*/
#define base_core_codec_init(_codec, _codec_name, _modname, _fmtp, _rate, _ms, _channels, _flags, _codec_settings, _pool) \
	base_core_codec_init_with_bitrate(_codec, _codec_name, _modname, _fmtp, _rate, _ms, _channels, 0, _flags, _codec_settings, _pool)
BASE_DECLARE(base_status_t) base_core_codec_init_with_bitrate(base_codec_t *codec,
    const char *codec_name,
    const char *fmtp,
    const char *modname,
    uint32_t rate,
    int ms,
    int channels,
    uint32_t bitrate,
    uint32_t flags,
    const base_codec_settings_t *codec_settings,
    base_memory_pool_t *pool);

BASE_DECLARE(base_status_t) base_core_codec_copy(base_codec_t *codec, base_codec_t *new_codec,
    const base_codec_settings_t *codec_settings, base_memory_pool_t *pool);
BASE_DECLARE(base_status_t) base_core_codec_parse_fmtp(const char *codec_name, const char *fmtp, uint32_t rate, base_codec_fmtp_t *codec_fmtp);
BASE_DECLARE(base_status_t) base_core_codec_reset(base_codec_t *codec);

/*!
\brief Encode data using a codec handle
\param codec the codec handle to use
\param other_codec the codec handle of the last codec used
\param decoded_data the raw data
\param decoded_data_len then length of the raw buffer
\param decoded_rate the rate of the decoded data
\param encoded_data the buffer to write the encoded data to
\param encoded_data_len the size of the encoded_data buffer
\param encoded_rate the new rate of the encoded data
\param flag flags to exchange
\return BASE_STATUS_SUCCESS if the data was encoded
\note encoded_data_len will be rewritten to the in-use size of encoded_data
*/
BASE_DECLARE(base_status_t) base_core_codec_encode(base_codec_t *codec,
    base_codec_t *other_codec,
    void *decoded_data,
    uint32_t decoded_data_len,
    uint32_t decoded_rate,
    void *encoded_data, uint32_t *encoded_data_len, uint32_t *encoded_rate, unsigned int *flag);

/*!
\brief Decode data using a codec handle
\param codec the codec handle to use
\param other_codec the codec handle of the last codec used
\param encoded_data the buffer to read the encoded data from
\param encoded_data_len the size of the encoded_data buffer
\param encoded_rate the rate of the encoded data
\param decoded_data the raw data buffer
\param decoded_data_len then length of the raw buffer
\param decoded_rate the new rate of the decoded data
\param flag flags to exchange
\return BASE_STATUS_SUCCESS if the data was decoded
\note decoded_data_len will be rewritten to the in-use size of decoded_data
*/
BASE_DECLARE(base_status_t) base_core_codec_decode(base_codec_t *codec,
    base_codec_t *other_codec,
    void *encoded_data,
    uint32_t encoded_data_len,
    uint32_t encoded_rate,
    void *decoded_data, uint32_t *decoded_data_len, uint32_t *decoded_rate, unsigned int *flag);

/*!
\brief Encode video data using a codec handle
\param codec the codec handle to use
\param frame the frame to encode
*/
BASE_DECLARE(base_status_t) base_core_codec_encode_video(base_codec_t *codec, base_frame_t *frame);


/*!
\brief send control data using a codec handle
\param codec the codec handle to use
\param cmd the command to send
\param ctype the type of the arguement
\param cmd_data a void pointer to the data matching the passed type
\param atype the type of the extra arguement
\param cmd_arg a void pointer to the data matching the passed type
\param rtype the type of the response if any
\param ret_data a void pointer to a pointer of return data
\return BASE_STATUS_SUCCESS if the command was received
*/
BASE_DECLARE(base_status_t) base_core_codec_control(base_codec_t *codec,
    base_codec_control_command_t cmd,
    base_codec_control_type_t ctype,
    void *cmd_data,
    base_codec_control_type_t atype,
    void *cmd_arg,
    base_codec_control_type_t *rtype,
    void **ret_data);

/*!
\brief Decode video data using a codec handle
\param codec the codec handle to use
\param frame the frame to be decoded
\param img the new image in I420 format, allocated by the codec
\param flag flags to exchange
\return BASE_STATUS_SUCCESS if the data was decoded, and a non-NULL img
*/
BASE_DECLARE(base_status_t) base_core_codec_decode_video(base_codec_t *codec, base_frame_t *frame);

/*!
\brief Destroy an initalized codec handle
\param codec the codec handle to destroy
\return BASE_STATUS_SUCCESS if the codec was destroyed
*/
BASE_DECLARE(base_status_t) base_core_codec_destroy(base_codec_t *codec);

/*!
\brief Assign the read codec to a given session
\param session session to add the codec to
\param codec the codec to add
\return BASE_STATUS_SUCCESS if successful
*/
BASE_DECLARE(base_status_t) base_core_session_set_read_codec(_In_ base_core_session_t *session, base_codec_t *codec);

/*!
\brief Assign the original read codec to a given session.  This is the read codec used by an endpoint.
\param session session to add the codec to
\param codec the codec to add
\return BASE_STATUS_SUCCESS if successful
*/
BASE_DECLARE(base_status_t) base_core_session_set_real_read_codec(_In_ base_core_session_t *session, base_codec_t *codec);

BASE_DECLARE(void) base_core_session_unset_read_codec(_In_ base_core_session_t *session);
BASE_DECLARE(void) base_core_session_unset_write_codec(_In_ base_core_session_t *session);


BASE_DECLARE(void) base_core_session_lock_codec_write(_In_ base_core_session_t *session);
BASE_DECLARE(void) base_core_session_unlock_codec_write(_In_ base_core_session_t *session);
BASE_DECLARE(void) base_core_session_lock_codec_read(_In_ base_core_session_t *session);
BASE_DECLARE(void) base_core_session_unlock_codec_read(_In_ base_core_session_t *session);

/*!
\brief Lock codec read mutex and codec write mutex using trylock in an infinite loop
\param session session to lock the codec in
*/
BASE_DECLARE(void) base_core_codec_lock_full(base_core_session_t *session);

/*!
\brief Unlock codec read mutex and codec write mutex
\param session session to unlock the codec in
*/
BASE_DECLARE(void) base_core_codec_unlock_full(base_core_session_t *session);

BASE_DECLARE(base_status_t) base_core_session_get_read_impl(base_core_session_t *session, base_codec_implementation_t *impp);
BASE_DECLARE(base_status_t) base_core_session_get_real_read_impl(base_core_session_t *session, base_codec_implementation_t *impp);
BASE_DECLARE(base_status_t) base_core_session_get_write_impl(base_core_session_t *session, base_codec_implementation_t *impp);
BASE_DECLARE(base_status_t) base_core_session_get_video_read_impl(base_core_session_t *session, base_codec_implementation_t *impp);
BASE_DECLARE(base_status_t) base_core_session_get_video_write_impl(base_core_session_t *session, base_codec_implementation_t *impp);


/*!
\brief Retrieve the read codec from a given session
\param session session to retrieve from
\return a pointer to the codec
*/
BASE_DECLARE(base_codec_t *) base_core_session_get_read_codec(_In_ base_core_session_t *session);

/*!
\brief Retrieve the effevtive read codec from a given session
\param session session to retrieve from
\return a pointer to the codec
*/
BASE_DECLARE(base_codec_t *) base_core_session_get_effective_read_codec(_In_ base_core_session_t *session);

/*!
\brief Assign the write codec to a given session
\param session session to add the codec to
\param codec the codec to add
\return BASE_STATUS_SUCCESS if successful
*/
BASE_DECLARE(base_status_t) base_core_session_set_write_codec(_In_ base_core_session_t *session, base_codec_t *codec);

/*!
\brief Retrieve the write codec from a given session
\param session session to retrieve from
\return a pointer to the codec
*/
BASE_DECLARE(base_codec_t *) base_core_session_get_write_codec(_In_ base_core_session_t *session);

/*!
\brief Retrieve the effevtive write codec from a given session
\param session session to retrieve from
\return a pointer to the codec
*/
BASE_DECLARE(base_codec_t *) base_core_session_get_effective_write_codec(_In_ base_core_session_t *session);

/*!
\brief Assign the video_read codec to a given session
\param session session to add the codec to
\param codec the codec to add
\return BASE_STATUS_SUCCESS if successful
*/
BASE_DECLARE(base_status_t) base_core_session_set_video_read_codec(_In_ base_core_session_t *session, base_codec_t *codec);

/*!
\brief Retrieve the video_read codec from a given session
\param session session to retrieve from
\return a pointer to the codec
*/
BASE_DECLARE(base_codec_t *) base_core_session_get_video_read_codec(_In_ base_core_session_t *session);

/*!
\brief Assign the video_write codec to a given session
\param session session to add the codec to
\param codec the codec to add
\return BASE_STATUS_SUCCESS if successful
*/
BASE_DECLARE(base_status_t) base_core_session_set_video_write_codec(_In_ base_core_session_t *session, base_codec_t *codec);

/*!
\brief Retrieve the video_write codec from a given session
\param session session to retrieve from
\return a pointer to the codec
*/
BASE_DECLARE(base_codec_t *) base_core_session_get_video_write_codec(_In_ base_core_session_t *session);

///\}
///\defgroup db Database Functions
///\ingroup core1
///\{
/*!
\brief Open a core db (SQLite) file
\param filename the path to the db file to open
\return the db handle
*/
BASE_DECLARE(base_core_db_t *) base_core_db_open_file(const char *filename);

/*!
\brief Open a core db (SQLite) in-memory
\param uri to the db to open
\return the db handle
*/
BASE_DECLARE(base_core_db_t *) base_core_db_open_in_memory(const char *uri);

/*!
\brief Execute a sql stmt until it is accepted
\param db the db handle
\param sql the sql to execute
\param retries the number of retries to use
\return BASE_STATUS_SUCCESS if successful

*/
BASE_DECLARE(base_status_t) base_core_db_persistant_execute(base_core_db_t *db, char *sql, uint32_t retries);
BASE_DECLARE(base_status_t) base_core_db_persistant_execute_trans(base_core_db_t *db, char *sql, uint32_t retries);
BASE_DECLARE(base_status_t) base_core_db_persistant_execute_trans(base_core_db_t *db, char *sql, uint32_t retries);



/*!
\brief perform a test query then perform a reactive query if the first one fails
\param db the db handle
\param test_sql the test sql
\param drop_sql the drop sql
\param reactive_sql the reactive sql
*/
BASE_DECLARE(void) base_core_db_test_reactive(base_core_db_t *db, char *test_sql, char *drop_sql, char *reactive_sql);

///\}

///\defgroup Media File Functions
///\ingroup core1
///\{

BASE_DECLARE(base_status_t) base_core_perform_file_open(const char *file, const char *func, int line,
    _In_ base_file_handle_t *fh,
    _In_opt_z_ const char *file_path,
    _In_ uint32_t channels,
    _In_ uint32_t rate, _In_ unsigned int flags, _In_opt_ base_memory_pool_t *pool);

/*!
\brief Open a media file using file format modules
\param _fh a file handle to use
\param _file_path the path to the file
\param _channels the number of channels
\param _rate the sample rate
\param _flags read/write flags
\param _pool the pool to use (NULL for new pool)
\return BASE_STATUS_SUCCESS if the file is opened
\note the loadable module used is chosen based on the file extension
*/
#define base_core_file_open(_fh, _file_path, _channels, _rate, _flags, _pool) \
	base_core_perform_file_open(__FILE__, __BASE_FUNC__, __LINE__, _fh, _file_path, _channels, _rate, _flags, _pool)

/*!
\brief Read media from a file handle
\param fh the file handle to read from (must be initilized by you memset all 0 for read, fill in channels and rate for write)
\param data the buffer to read the data to
\param len the max size of the buffer
\return BASE_STATUS_SUCCESS with len adjusted to the bytes read if successful
*/
BASE_DECLARE(base_status_t) base_core_file_read(_In_ base_file_handle_t *fh, void *data, base_size_t *len);

/*!
\brief Write media to a file handle
\param fh the file handle to write to
\param data the buffer to write
\param len the amount of data to write from the buffer
\return BASE_STATUS_SUCCESS with len adjusted to the bytes written if successful
*/
BASE_DECLARE(base_status_t) base_core_file_write(_In_ base_file_handle_t *fh, void *data, base_size_t *len);

/*!
\brief Write media to a file handle
\param fh the file handle to write to
\param data the buffer to write
\param len the amount of data to write from the buffer
\return BASE_STATUS_SUCCESS with len adjusted to the bytes written if successful
*/
BASE_DECLARE(base_status_t) base_core_file_write_video(_In_ base_file_handle_t *fh, base_frame_t *frame);
BASE_DECLARE(base_status_t) base_core_file_read_video(base_file_handle_t *fh, base_frame_t *frame, base_video_read_flag_t flags);

/*!
\brief Seek a position in a file
\param fh the file handle to seek
\param cur_pos the current position in the file
\param samples the amount of samples to seek from the beginning of the file
\param whence the indicator (see traditional seek)
\return BASE_STATUS_SUCCESS with cur_pos adjusted to new position
*/
BASE_DECLARE(base_status_t) base_core_file_seek(_In_ base_file_handle_t *fh, unsigned int *cur_pos, int64_t samples, int whence);

/*!
\brief Set metadata to the desired string
\param fh the file handle to set data to
\param col the enum of the col name
\param string the string to add
\return BASE_STATUS_SUCCESS with cur_pos adjusted to new position
*/
BASE_DECLARE(base_status_t) base_core_file_set_string(_In_ base_file_handle_t *fh, base_audio_col_t col, const char *string);

/*!
\brief get metadata of the desired string
\param fh the file handle to get data from
\param col the enum of the col name
\param string pointer to the string to fetch
\return BASE_STATUS_SUCCESS with cur_pos adjusted to new position
*/
BASE_DECLARE(base_status_t) base_core_file_get_string(_In_ base_file_handle_t *fh, base_audio_col_t col, const char **string);

/*!
\brief Pre close an open file handle, then can get file size etc., no more wirte to the file
\param fh the file handle to close
\return BASE_STATUS_SUCCESS if the file handle was pre closed
*/
BASE_DECLARE(base_status_t) base_core_file_pre_close(_In_ base_file_handle_t *fh);

/*!
\brief Duplicates a file handle using another pool
\param oldfh the file handle to duplicate
\param newfh pointer to assign new file handle to
\param pool the pool to use (NULL for new pool)
\return BASE_STATUS_SUCCESS if the file handle was duplicated
*/

BASE_DECLARE(base_status_t) base_core_file_handle_dup(base_file_handle_t *oldfh, base_file_handle_t **newfh, base_memory_pool_t *pool);

/*!
\brief Close an open file handle
\param fh the file handle to close
\return BASE_STATUS_SUCCESS if the file handle was closed
*/
BASE_DECLARE(base_status_t) base_core_file_close(_In_ base_file_handle_t *fh);

BASE_DECLARE(base_status_t) base_core_file_command(base_file_handle_t *fh, base_file_command_t command);

BASE_DECLARE(base_status_t) base_core_file_truncate(base_file_handle_t *fh, int64_t offset);
BASE_DECLARE(base_bool_t) base_core_file_has_video(base_file_handle_t *fh, base_bool_t CHECK_OPEN);


///\}

///\defgroup speech ASR/TTS Functions
///\ingroup core1
///\{
/*!
\brief Open a speech handle
\param sh a speech handle to use
\param module_name the speech module to use
\param voice_name the desired voice name
\param rate the sampling rate
\param interval the sampling interval
\param flags tts flags
\param pool the pool to use (NULL for new pool)
\return BASE_STATUS_SUCCESS if the handle is opened
*/
BASE_DECLARE(base_status_t) base_core_speech_open(_In_ base_speech_handle_t *sh,
    const char *module_name,
    const char *voice_name,
    _In_ unsigned int rate,
    _In_ unsigned int interval,
    _In_ unsigned int channels,
    base_speech_flag_t *flags, _In_opt_ base_memory_pool_t *pool);
/*!
\brief Feed text to the TTS module
\param sh the speech handle to feed
\param text the buffer to write
\param flags flags in/out for fine tuning
\return BASE_STATUS_SUCCESS with len adjusted to the bytes written if successful
*/
BASE_DECLARE(base_status_t) base_core_speech_feed_tts(base_speech_handle_t *sh, const char *text, base_speech_flag_t *flags);

/*!
\brief Flush TTS audio on a given handle
\param sh the speech handle
*/
BASE_DECLARE(void) base_core_speech_flush_tts(base_speech_handle_t *sh);

/*!
\brief Set a text parameter on a TTS handle
\param sh the speech handle
\param param the parameter
\param val the value
*/
BASE_DECLARE(void) base_core_speech_text_param_tts(base_speech_handle_t *sh, char *param, const char *val);

/*!
\brief Set a numeric parameter on a TTS handle
\param sh the speech handle
\param param the parameter
\param val the value
*/
BASE_DECLARE(void) base_core_speech_numeric_param_tts(base_speech_handle_t *sh, char *param, int val);

/*!
\brief Set a float parameter on a TTS handle
\param sh the speech handle
\param param the parameter
\param val the value
*/
BASE_DECLARE(void) base_core_speech_float_param_tts(base_speech_handle_t *sh, char *param, double val);

/*!
\brief Read rendered audio from the TTS module
\param sh the speech handle to read
\param data the buffer to read to
\param datalen the max size / written size of the data
\param rate the rate of the read audio
\param flags flags in/out for fine tuning
\return BASE_STATUS_SUCCESS with len adjusted to the bytes written if successful
*/
BASE_DECLARE(base_status_t) base_core_speech_read_tts(base_speech_handle_t *sh, void *data, base_size_t *datalen, base_speech_flag_t *flags);
/*!
\brief Close an open speech handle
\param sh the speech handle to close
\param flags flags in/out for fine tuning
\return BASE_STATUS_SUCCESS if the file handle was closed
*/
BASE_DECLARE(base_status_t) base_core_speech_close(base_speech_handle_t *sh, base_speech_flag_t *flags);


/*!
\brief Open an asr handle
\param ah the asr handle to open
\param module_name the name of the asr module
\param codec the preferred codec
\param rate the preferred rate
\param dest the destination address
\param flags flags to influence behaviour
\param pool the pool to use (NULL for new pool)
\return BASE_STATUS_SUCCESS if the asr handle was opened
*/
BASE_DECLARE(base_status_t) base_core_asr_open(base_asr_handle_t *ah,
    const char *module_name,
    const char *codec, int rate, const char *dest, base_asr_flag_t *flags, base_memory_pool_t *pool);

/*!
\brief Close an asr handle
\param ah the handle to close
\param flags flags to influence behaviour
\return BASE_STATUS_SUCCESS
*/
BASE_DECLARE(base_status_t) base_core_asr_close(base_asr_handle_t *ah, base_asr_flag_t *flags);

/*!
\brief Feed audio data to an asr handle
\param ah the handle to feed data to
\param data a pointer to the data
\param len the size in bytes of the data
\param flags flags to influence behaviour
\return BASE_STATUS_SUCCESS
*/
BASE_DECLARE(base_status_t) base_core_asr_feed(base_asr_handle_t *ah, void *data, unsigned int len, base_asr_flag_t *flags);

/*!
\brief Feed DTMF to an asr handle
\param ah the handle to feed data to
\param dtmf a string of DTMF digits
\param flags flags to influence behaviour
\return BASE_STATUS_SUCCESS
*/
BASE_DECLARE(base_status_t) base_core_asr_feed_dtmf(base_asr_handle_t *ah, const base_dtmf_t *dtmf, base_asr_flag_t *flags);

/*!
\brief Check an asr handle for results
\param ah the handle to check
\param flags flags to influence behaviour
\return BASE_STATUS_SUCCESS
*/
BASE_DECLARE(base_status_t) base_core_asr_check_results(base_asr_handle_t *ah, base_asr_flag_t *flags);

/*!
\brief Get results from an asr handle
\param ah the handle to get results from
\param xmlstr a pointer to dynamically allocate an xml result string to
\param flags flags to influence behaviour
\return BASE_STATUS_SUCCESS
*/
BASE_DECLARE(base_status_t) base_core_asr_get_results(base_asr_handle_t *ah, char **xmlstr, base_asr_flag_t *flags);

/*!
\brief Get result headers from an asr handle
\param ah the handle to get results from
\param headers a pointer to dynamically allocate an base_event_t result to
\param flags flags to influence behaviour
\return BASE_STATUS_SUCCESS
*/
BASE_DECLARE(base_status_t) base_core_asr_get_result_headers(base_asr_handle_t *ah, base_event_t **headers, base_asr_flag_t *flags);

/*!
\brief Load a grammar to an asr handle
\param ah the handle to load to
\param grammar the grammar text, file path, or URI
\param name the grammar name
\return BASE_STATUS_SUCCESS
*/
BASE_DECLARE(base_status_t) base_core_asr_load_grammar(base_asr_handle_t *ah, const char *grammar, const char *name);

/*!
\brief Unload a grammar from an asr handle
\param ah the handle to unload the grammar from
\param name the name of the grammar to unload
\return BASE_STATUS_SUCCESS
*/
BASE_DECLARE(base_status_t) base_core_asr_unload_grammar(base_asr_handle_t *ah, const char *name);

/*!
\brief Enable a grammar from an asr handle
\param ah the handle to enable the grammar from
\param name the name of the grammar to enable
\return BASE_STATUS_SUCCESS
*/
BASE_DECLARE(base_status_t) base_core_asr_enable_grammar(base_asr_handle_t *ah, const char *name);

/*!
\brief Disable a grammar from an asr handle
\param ah the handle to disable the grammar from
\param name the name of the grammar to disable
\return BASE_STATUS_SUCCESS
*/
BASE_DECLARE(base_status_t) base_core_asr_disable_grammar(base_asr_handle_t *ah, const char *name);

/*!
\brief Disable all grammars from an asr handle
\param ah the handle to disable the grammars from
\return BASE_STATUS_SUCCESS
*/
BASE_DECLARE(base_status_t) base_core_asr_disable_all_grammars(base_asr_handle_t *ah);

/*!
\brief Pause detection on an asr handle
\param ah the handle to pause
\return BASE_STATUS_SUCCESS
*/
BASE_DECLARE(base_status_t) base_core_asr_pause(base_asr_handle_t *ah);

/*!
\brief Resume detection on an asr handle
\param ah the handle to resume
\return BASE_STATUS_SUCCESS
*/
BASE_DECLARE(base_status_t) base_core_asr_resume(base_asr_handle_t *ah);

/*!
\brief Start input timers on an asr handle
\param ah the handle to start timers on
\return BASE_STATUS_SUCCESS
*/
BASE_DECLARE(base_status_t) base_core_asr_start_input_timers(base_asr_handle_t *ah);

/*!
\brief Set a text parameter on an asr handle
\param sh the asr handle
\param param the parameter
\param val the value
*/
BASE_DECLARE(void) base_core_asr_text_param(base_asr_handle_t *ah, char *param, const char *val);

/*!
\brief Set a numeric parameter on an asr handle
\param sh the asr handle
\param param the parameter
\param val the value
*/
BASE_DECLARE(void) base_core_asr_numeric_param(base_asr_handle_t *ah, char *param, int val);

/*!
\brief Set a float parameter on an asr handle
\param sh the asr handle
\param param the parameter
\param val the value
*/
BASE_DECLARE(void) base_core_asr_float_param(base_asr_handle_t *ah, char *param, double val);

///\}


///\defgroup dir Directory Service Functions
///\ingroup core1
///\{
/*!
\brief Open a directory handle
\param dh a directory handle to use
\param module_name the directory module to use
\param source the source of the db (ip, hostname, path etc)
\param dsn the username or designation of the lookup
\param passwd the password
\param pool the pool to use (NULL for new pool)
\return BASE_STATUS_SUCCESS if the handle is opened
*/
BASE_DECLARE(base_status_t) base_core_directory_open(base_directory_handle_t *dh,
    char *module_name, char *source, char *dsn, char *passwd, base_memory_pool_t *pool);

/*!
\brief Query a directory handle
\param dh a directory handle to use
\param base the base to query against
\param query a string of filters or query data
\return BASE_STATUS_SUCCESS if the query is successful
*/
BASE_DECLARE(base_status_t) base_core_directory_query(base_directory_handle_t *dh, char *base, char *query);

/*!
\brief Obtain the next record in a lookup
\param dh a directory handle to use
\return BASE_STATUS_SUCCESS if another record exists
*/
BASE_DECLARE(base_status_t) base_core_directory_next(base_directory_handle_t *dh);

/*!
\brief Obtain the next name/value pair in the current record
\param dh a directory handle to use
\param var a pointer to pointer of the name to fill in
\param val a pointer to pointer of the value to fill in
\return BASE_STATUS_SUCCESS if an item exists
*/
BASE_DECLARE(base_status_t) base_core_directory_next_pair(base_directory_handle_t *dh, char **var, char **val);

/*!
\brief Close an open directory handle
\param dh a directory handle to close
\return BASE_STATUS_SUCCESS if handle was closed
*/
BASE_DECLARE(base_status_t) base_core_directory_close(base_directory_handle_t *dh);
///\}


///\defgroup misc Misc
///\ingroup core1
///\{
/*!
\brief Retrieve a FILE stream of a given text channel name
\param channel text channel enumeration
\return a FILE stream
*/
BASE_DECLARE(FILE *) base_core_data_channel(base_text_channel_t channel);

/*!
\brief Determines if the core is ready to take calls
\return BASE_TRUE or BASE_FALSE
*/
BASE_DECLARE(base_bool_t) base_core_ready(void);

BASE_DECLARE(base_bool_t) base_core_running(void);

/*!
\brief Determines if the core is ready to take inbound calls
\return BASE_TRUE or BASE_FALSE
*/
BASE_DECLARE(base_bool_t) base_core_ready_inbound(void);

/*!
\brief Determines if the core is ready to place outbound calls
\return BASE_TRUE or BASE_FALSE
*/
BASE_DECLARE(base_bool_t) base_core_ready_outbound(void);

/*!
\brief return core flags
\return core flags
*/
BASE_DECLARE(base_core_flag_t) base_core_flags(void);

/*!
\brief Execute a management operation.
\param relative_oid the relative oid of the operation.
\param action the action to perform.
\param data input/output string.
\param datalen size in bytes of data.
\return SUCCESS on sucess.
*/
BASE_DECLARE(base_status_t) base_core_management_exec(char *relative_oid, base_management_action_t action, char *data, base_size_t datalen);

/*!
\brief Switch on the privilege awareness for the process and request required privileges
\return 0 on success
*/

BASE_DECLARE(int32_t) base_core_set_process_privileges(void);

/*!
\brief Set the maximum priority the process can obtain
\return 0 on success
*/

BASE_DECLARE(int32_t) set_normal_priority(void);
BASE_DECLARE(int32_t) set_auto_priority(void);
BASE_DECLARE(int32_t) set_realtime_priority(void);
BASE_DECLARE(int32_t) set_low_priority(void);

/*!
\brief Change user and/or group of the running process
\param user name of the user to base to (or NULL)
\param group name of the group to base to (or NULL)
\return 0 on success, -1 otherwise

Several possible combinations:
- user only (group NULL): base to user and his primary group (and supplementary groups, if supported)
- user and group: base to user and specified group (only)
- group only (user NULL): base group only
*/
BASE_DECLARE(int32_t) change_user_group(const char *user, const char *group);

/*!
\brief Run endlessly until the system is shutdown
\param bg divert console to the background
*/
BASE_DECLARE(void) base_core_runtime_loop(int bg);

/*!
\brief Set the output console to the desired file
\param console the file path
*/
BASE_DECLARE(base_status_t) base_core_set_console(const char *console);

/*!
\brief Breakdown a number of milliseconds into various time spec
\param total_ms a number of milliseconds
\param duration an object to store the results
*/
BASE_DECLARE(void) base_core_measure_time(base_time_t total_ms, base_core_time_duration_t *duration);

/*!
\brief Number of microseconds the system has been up
\return a number of microseconds
*/
BASE_DECLARE(base_time_t) base_core_uptime(void);

/*!
\brief send a control message to the core
\param cmd the command
\param val the command arguement (if needed)
\return 0 on success nonzero on error
*/
BASE_DECLARE(int32_t) base_core_session_ctl(base_session_ctl_t cmd, void *val);

/*!
\brief Get the output console
\return the FILE stream
*/
BASE_DECLARE(FILE *) base_core_get_console(void);

#ifndef SWIG
/*!
\brief Launch a thread
*/
BASE_DECLARE(base_thread_t *) base_core_launch_thread(void *(BASE_THREAD_FUNC * func) (base_thread_t *, void *),
    void *obj, base_memory_pool_t *pool);
#endif

/*!
\brief Initiate Globals
*/
BASE_DECLARE(void) base_core_set_globals(void);

/*!
\brief Checks if 2 sessions are using the same endpoint module
\param a the first session
\param b the second session
\return TRUE or FALSE
*/
BASE_DECLARE(uint8_t) base_core_session_compare(base_core_session_t *a, base_core_session_t *b);
/*!
\brief Checks if a session is using a specific endpoint
\param session the session
\param endpoint_interface interface of the endpoint to check
\return TRUE or FALSE
*/
BASE_DECLARE(uint8_t) base_core_session_check_interface(base_core_session_t *session, const base_endpoint_interface_t *endpoint_interface);

BASE_DECLARE(base_status_t) base_core_session_set_video_read_callback(base_core_session_t *session,
    base_core_video_thread_callback_func_t func, void *user_data);

BASE_DECLARE(base_status_t) base_core_session_video_read_callback(base_core_session_t *session, base_frame_t *frame);

BASE_DECLARE(base_hash_index_t *) base_core_mime_index(void);
BASE_DECLARE(const char *) base_core_mime_ext2type(const char *ext);
BASE_DECLARE(const char *) base_core_mime_type2ext(const char *type);
BASE_DECLARE(base_status_t) base_core_mime_add_type(const char *type, const char *ext);

BASE_DECLARE(base_loadable_module_interface_t *) base_loadable_module_create_module_interface(base_memory_pool_t *pool, const char *name);
BASE_DECLARE(void *) base_loadable_module_create_interface(base_loadable_module_interface_t *mod, base_module_interface_name_t iname);
/*!
\brief Get the current epoch time in microseconds
\return the current epoch time in microseconds
*/
BASE_DECLARE(base_time_t) base_micro_time_now(void);
BASE_DECLARE(base_time_t) base_mono_micro_time_now(void);
BASE_DECLARE(void) base_core_memory_reclaim(void);
BASE_DECLARE(void) base_core_memory_reclaim_events(void);
BASE_DECLARE(void) base_core_memory_reclaim_logger(void);
BASE_DECLARE(void) base_core_memory_reclaim_all(void);
BASE_DECLARE(void) base_core_setrlimits(void);
BASE_DECLARE(base_time_t) base_time_ref(void);
BASE_DECLARE(void) base_time_sync(void);
/*!
\brief Get the current epoch time
\param [out] (optional) The current epoch time
\return The current epoch time
*/
BASE_DECLARE(time_t) base_epoch_time_now(time_t *t);
BASE_DECLARE(const char *) base_lookup_timezone(const char *tz_name);
BASE_DECLARE(base_status_t) base_strftime_tz(const char *tz, const char *format, char *date, size_t len, base_time_t thetime);
BASE_DECLARE(base_status_t) base_time_exp_tz_name(const char *tz, base_time_exp_t *tm, base_time_t thetime);
BASE_DECLARE(void) base_load_network_lists(base_bool_t reload);
BASE_DECLARE(base_bool_t) base_check_network_list_ip_port_token(const char *ip_str, int port, const char *list_name, const char **token);
BASE_DECLARE(base_bool_t) base_check_network_list_ip_token(const char *ip_str, const char *list_name, const char **token);
#define base_check_network_list_ip(_ip_str, _list_name) base_check_network_list_ip_token(_ip_str, _list_name, NULL)
BASE_DECLARE(void) base_time_set_monotonic(base_bool_t enable);
BASE_DECLARE(void) base_time_set_timerfd(int enable);
BASE_DECLARE(void) base_time_set_nanosleep(base_bool_t enable);
BASE_DECLARE(void) base_time_set_matrix(base_bool_t enable);
BASE_DECLARE(void) base_time_set_cond_yield(base_bool_t enable);
BASE_DECLARE(void) base_time_set_use_system_time(base_bool_t enable);
BASE_DECLARE(uint32_t) base_core_min_dtmf_duration(uint32_t duration);
BASE_DECLARE(uint32_t) base_core_max_dtmf_duration(uint32_t duration);
BASE_DECLARE(double) base_core_min_idle_cpu(double new_limit);
BASE_DECLARE(double) base_core_idle_cpu(void);
BASE_DECLARE(uint32_t) base_core_default_dtmf_duration(uint32_t duration);
BASE_DECLARE(base_status_t) base_console_set_complete(const char *string);
BASE_DECLARE(base_status_t) base_console_set_alias(const char *string);
BASE_DECLARE(int) base_system(const char *cmd, base_bool_t wait);
BASE_DECLARE(int) base_stream_system(const char *cmd, base_stream_handle_t *stream);
BASE_DECLARE(void) base_cond_yield(base_interval_time_t t);
BASE_DECLARE(void) base_cond_next(void);
BASE_DECLARE(base_status_t) base_core_chat_send_args(const char *dest_proto, const char *proto, const char *from, const char *to,
    const char *subject, const char *body, const char *type, const char *hint, base_bool_t blocking);
BASE_DECLARE(base_status_t) base_core_chat_send(const char *dest_proto, base_event_t *message_event);
BASE_DECLARE(base_status_t) base_core_chat_deliver(const char *dest_proto, base_event_t **message_event);

BASE_DECLARE(base_status_t) base_ivr_preprocess_session(base_core_session_t *session, const char *cmds);
BASE_DECLARE(void) base_core_sqldb_pause(void);
BASE_DECLARE(void) base_core_sqldb_resume(void);


///\}

/*!
\}
*/

typedef int (*base_core_db_event_callback_func_t) (void *pArg, base_event_t *event);

#define CACHE_DB_LEN 256
typedef enum {
    CDF_INUSE = (1 << 0),
    CDF_PRUNE = (1 << 1),
    CDF_NONEXPIRING = (1 << 2)
} cache_db_flag_t;

typedef enum {
    SCDB_TYPE_CORE_DB,
    SCDB_TYPE_ODBC,
    SCDB_TYPE_DATABASE_INTERFACE
} base_cache_db_handle_type_t;

typedef union {
    base_coredb_handle_t *core_db_dbh;
    base_odbc_handle_t *odbc_dbh;
    base_database_interface_handle_t *database_interface_dbh;
} base_cache_db_native_handle_t;

typedef struct {
    char *db_path;
    base_bool_t in_memory;
} base_cache_db_core_db_options_t;

typedef struct {
    char *dsn;
    char *user;
    char *pass;
} base_cache_db_odbc_options_t;

typedef struct {
    const char *original_dsn;
    char *connection_string;
    char prefix[16];
    base_database_interface_t *database_interface;
    base_bool_t make_module_no_unloadable;
} base_cache_db_database_interface_options_t;

typedef union {
    base_cache_db_core_db_options_t core_db_options;
    base_cache_db_odbc_options_t odbc_options;
    base_cache_db_database_interface_options_t database_interface_options;
} base_cache_db_connection_options_t;

struct base_cache_db_handle;
typedef struct base_cache_db_handle base_cache_db_handle_t;

static inline const char *base_cache_db_type_name(base_cache_db_handle_type_t type)
{
    const char *type_str = "INVALID";

    switch (type) {
    case SCDB_TYPE_DATABASE_INTERFACE:
    {
        type_str = "DATABASE_INTERFACE";
    }
    break;
    case SCDB_TYPE_ODBC:
    {
        type_str = "ODBC";
    }
    break;
    case SCDB_TYPE_CORE_DB:
    {
        type_str = "CORE_DB";
    }
    break;
    }

    return type_str;
}

BASE_DECLARE(base_cache_db_handle_type_t) base_cache_db_get_type(base_cache_db_handle_t *dbh);

/*!
\brief Returns the handle to the pool, immediately available for other
threads to use.
\param [in] The handle
*/
BASE_DECLARE(void) base_cache_db_dismiss_db_handle(base_cache_db_handle_t ** dbh);
/*!
\brief Returns the handle to the pool, handle is NOT available to
other threads until the allocating thread actually terminates.
\param [in] The handle
*/
BASE_DECLARE(void) base_cache_db_release_db_handle(base_cache_db_handle_t ** dbh);
/*!
\brief Gets a new cached handle from the pool, potentially creating a new connection.
The connection is bound to the thread until it (the thread) terminates unless
you dismiss rather than release.
\param [out] dbh The handle
\param [in] type - ODBC or SQLLITE
\param [in] connection_options (userid, password, etc)
*/
BASE_DECLARE(base_status_t) _base_cache_db_get_db_handle(base_cache_db_handle_t ** dbh,
    base_cache_db_handle_type_t type,
    base_cache_db_connection_options_t *connection_options,
    const char *file, const char *func, int line);
#define base_cache_db_get_db_handle(_a, _b, _c) _base_cache_db_get_db_handle(_a, _b, _c, __FILE__, __BASE_FUNC__, __LINE__)

BASE_DECLARE(base_status_t) _base_cache_db_get_db_handle_dsn_ex(base_cache_db_handle_t **dbh, const char *dsn, base_bool_t make_module_no_unloadable,
    const char *file, const char *func, int line);
BASE_DECLARE(base_status_t) _base_cache_db_get_db_handle_dsn(base_cache_db_handle_t **dbh, const char *dsn,
    const char *file, const char *func, int line);
#define base_cache_db_get_db_handle_dsn(_a, _b) _base_cache_db_get_db_handle_dsn(_a, _b, __FILE__, __BASE_FUNC__, __LINE__)

/*!
\brief Executes the create schema sql
\param [in] dbh The handle
\param [in] sql - sql to run
\param [out] err - Error if it exists
*/
BASE_DECLARE(base_status_t) base_cache_db_create_schema(base_cache_db_handle_t *dbh, char *sql, char **err);
/*!
\brief Executes the sql and returns the result as a string
\param [in] dbh The handle
\param [in] sql - sql to run
\param [out] str - buffer for result
\param [in] len - length of str buffer
\param [out] err - Error if it exists
*/
BASE_DECLARE(char *) base_cache_db_execute_sql2str(base_cache_db_handle_t *dbh, char *sql, char *str, size_t len, char **err);
/*!
\brief Executes the sql
\param [in] dbh The handle
\param [in] sql - sql to run
\param [out] err - Error if it exists
*/
BASE_DECLARE(base_status_t) base_cache_db_execute_sql(base_cache_db_handle_t *dbh, char *sql, char **err);
/*!
\brief Executes the sql and uses callback for row-by-row processing
\param [in] dbh The handle
\param [in] sql - sql to run
\param [in] callback - function pointer to callback
\param [in] pdata - data to pass to callback
\param [out] err - Error if it exists
*/
BASE_DECLARE(base_status_t) base_cache_db_execute_sql_callback(base_cache_db_handle_t *dbh, const char *sql,
    base_core_db_callback_func_t callback, void *pdata, char **err);

/*!
\brief Executes the sql and uses callback for row-by-row processing
\param [in] dbh The handle
\param [in] sql - sql to run
\param [in] callback - function pointer to callback
\param [in] err_callback - function pointer to callback when error occurs
\param [in] pdata - data to pass to callback
\param [out] err - Error if it exists
*/
BASE_DECLARE(base_status_t) base_cache_db_execute_sql_callback_err(base_cache_db_handle_t *dbh, const char *sql,
    base_core_db_callback_func_t callback,
    base_core_db_err_callback_func_t err_callback,
    void *pdata, char **err);

/*!
\brief Get the affected rows of the last performed query
\param [in] dbh The handle
\param [out] the number of affected rows
*/
BASE_DECLARE(int) base_cache_db_affected_rows(base_cache_db_handle_t *dbh);

/*!
\brief load an external extension to db
\param [in] dbh The handle
\param [out] the path to the extension
*/
BASE_DECLARE(int) base_cache_db_load_extension(base_cache_db_handle_t *dbh, const char *extension);

/*!
\brief Provides some feedback as to the status of the db connection pool
\param [in] stream stream for status
*/
BASE_DECLARE(void) base_cache_db_status(base_stream_handle_t *stream);
BASE_DECLARE(base_status_t) _base_core_db_handle(base_cache_db_handle_t ** dbh, const char *file, const char *func, int line);
#define base_core_db_handle(_a) _base_core_db_handle(_a, __FILE__, __BASE_FUNC__, __LINE__)

BASE_DECLARE(base_bool_t) base_cache_db_test_reactive(base_cache_db_handle_t *db,
    const char *test_sql, const char *drop_sql, const char *reactive_sql);
BASE_DECLARE(base_bool_t) base_cache_db_test_reactive_ex(base_cache_db_handle_t *db,
    const char *test_sql, const char *drop_sql, const char *reactive_sql, const char *row_size_limited_reactive_sql);
BASE_DECLARE(base_status_t) base_cache_db_persistant_execute(base_cache_db_handle_t *dbh, const char *sql, uint32_t retries);
BASE_DECLARE(base_status_t) base_cache_db_persistant_execute_trans_full(base_cache_db_handle_t *dbh, char *sql, uint32_t retries,
    const char *pre_trans_execute,
    const char *post_trans_execute,
    const char *inner_pre_trans_execute,
    const char *inner_post_trans_execute);
#define base_cache_db_persistant_execute_trans(_d, _s, _r) base_cache_db_persistant_execute_trans_full(_d, _s, _r, NULL, NULL, NULL, NULL)

BASE_DECLARE(void) base_cache_db_database_interface_flush_handles(base_database_interface_t *database_interface);

/*!
\brief Returns error if no suitable database interface found to serve core db dsn.
*/
BASE_DECLARE(base_status_t) base_core_check_core_db_dsn(void);

/*!
\brief Returns error if no suitable database interface found for a dsn.
*/
BASE_DECLARE(base_status_t) base_database_available(char* dsn);

BASE_DECLARE(void) base_core_set_signal_handlers(void);
BASE_DECLARE(uint32_t) base_core_debug_level(void);
BASE_DECLARE(int32_t) base_core_sps(void);
BASE_DECLARE(int32_t) base_core_sps_last(void);
BASE_DECLARE(int32_t) base_core_sps_peak(void);
BASE_DECLARE(int32_t) base_core_sps_peak_fivemin(void);
BASE_DECLARE(int32_t) base_core_sessions_peak(void);
BASE_DECLARE(int32_t) base_core_sessions_peak_fivemin(void);
BASE_DECLARE(void) base_cache_db_flush_handles(void);
BASE_DECLARE(const char *) base_core_banner(void);
BASE_DECLARE(base_bool_t) base_core_session_in_thread(base_core_session_t *session);
BASE_DECLARE(uint32_t) base_default_ptime(const char *name, uint32_t number);
BASE_DECLARE(uint32_t) base_default_rate(const char *name, uint32_t number);
BASE_DECLARE(uint32_t) base_core_max_audio_channels(uint32_t limit);

/*!
\brief Add user registration
\param [in] user
\param [in] realm
\param [in] token
\param [in] url - a freebase dial string
\param [in] expires
\param [in] network_ip
\param [in] network_port
\param [in] network_proto - one of tls, tcp, udp
\param [in] metadata - generic metadata supplied by module
\param [out] err - Error if it exists
*/
BASE_DECLARE(base_status_t) base_core_add_registration(const char *user, const char *realm, const char *token, const char *url, uint32_t expires,
    const char *network_ip, const char *network_port, const char *network_proto,
    const char *metadata);
/*!
\brief Delete user registration
\param [in] user
\param [in] realm
\param [in] token
\param [out] err - Error if it exists
*/
BASE_DECLARE(base_status_t) base_core_del_registration(const char *user, const char *realm, const char *token);
/*!
\brief Expire user registrations
\param [in] force delete all registrations
\param [out] err - Error if it exists
*/
BASE_DECLARE(base_status_t) base_core_expire_registration(int force);

/*!
\brief Get RTP port range start value
\param[in] void
\param[out] RTP port range start value
*/
BASE_DECLARE(uint16_t) base_core_get_rtp_port_range_start_port(void);

/*!
\brief Get RTP port range end value
\param[in] void
\param[out] RTP port range end value
*/
BASE_DECLARE(uint16_t) base_core_get_rtp_port_range_end_port(void);

BASE_DECLARE(char *) base_say_file_handle_get_variable(base_say_file_handle_t *sh, const char *var);
BASE_DECLARE(char *) base_say_file_handle_get_path(base_say_file_handle_t *sh);
BASE_DECLARE(char *) base_say_file_handle_detach_path(base_say_file_handle_t *sh);
BASE_DECLARE(void) base_say_file_handle_destroy(base_say_file_handle_t **sh);
BASE_DECLARE(base_status_t) base_say_file_handle_create(base_say_file_handle_t **sh, const char *ext, base_event_t **var_event);
BASE_DECLARE(void) base_say_file(base_say_file_handle_t *sh, const char *fmt, ...);
BASE_DECLARE(int) base_max_file_desc(void);
BASE_DECLARE(void) base_close_extra_files(int *keep, int keep_ttl);
BASE_DECLARE(base_status_t) base_core_thread_set_cpu_affinity(int cpu);
BASE_DECLARE(void) base_os_yield(void);
BASE_DECLARE(base_status_t) base_core_get_stacksizes(base_size_t *cur, base_size_t *max);
BASE_DECLARE(void) base_core_gen_encoded_silence(unsigned char *data, const base_codec_implementation_t *read_impl, base_size_t len);

BASE_DECLARE(base_cache_db_handle_type_t) base_core_dbtype(void);
BASE_DECLARE(void) base_core_sql_exec(const char *sql);
BASE_DECLARE(int) base_core_recovery_recover(const char *technology, const char *profile_name);
BASE_DECLARE(void) base_core_recovery_untrack(base_core_session_t *session, base_bool_t force);
BASE_DECLARE(void) base_core_recovery_track(base_core_session_t *session);
BASE_DECLARE(void) base_core_recovery_flush(const char *technology, const char *profile_name);

BASE_DECLARE(void) base_sql_queue_manager_pause(base_sql_queue_manager_t *qm, base_bool_t flush);
BASE_DECLARE(void) base_sql_queue_manager_resume(base_sql_queue_manager_t *qm);

BASE_DECLARE(int) base_sql_queue_manager_size(base_sql_queue_manager_t *qm, uint32_t index);
BASE_DECLARE(base_status_t) base_sql_queue_manager_push_confirm(base_sql_queue_manager_t *qm, const char *sql, uint32_t pos, base_bool_t dup);
BASE_DECLARE(base_status_t) base_sql_queue_manager_push(base_sql_queue_manager_t *qm, const char *sql, uint32_t pos, base_bool_t dup);
BASE_DECLARE(base_status_t) base_sql_queue_manager_destroy(base_sql_queue_manager_t **qmp);
BASE_DECLARE(base_status_t) base_sql_queue_manager_init_name(const char *name,
    base_sql_queue_manager_t **qmp,
    uint32_t numq, const char *dsn, uint32_t max_trans,
    const char *pre_trans_execute,
    const char *post_trans_execute,
    const char *inner_pre_trans_execute,
    const char *inner_post_trans_execute);

#define base_sql_queue_manager_init(_q, _n, _d, _m, _p1, _p2, _ip1, _ip2) base_sql_queue_manager_init_name(__FILE__, _q, _n, _d, _m, _p1, _p2, _ip1, _ip2)

BASE_DECLARE(base_status_t) base_sql_queue_manager_start(base_sql_queue_manager_t *qm);
BASE_DECLARE(base_status_t) base_sql_queue_manager_stop(base_sql_queue_manager_t *qm);
BASE_DECLARE(base_status_t) base_cache_db_execute_sql_event_callback(base_cache_db_handle_t *dbh,
    const char *sql, base_core_db_event_callback_func_t callback, void *pdata, char **err);

BASE_DECLARE(void) base_sql_queue_manager_execute_sql_callback(base_sql_queue_manager_t *qm, const char *sql,
    base_core_db_callback_func_t callback,
    void *pdata);
BASE_DECLARE(void) base_sql_queue_manager_execute_sql_callback_err(base_sql_queue_manager_t *qm, const char *sql,
    base_core_db_callback_func_t callback,
    base_core_db_err_callback_func_t err_callback,
    void *pdata);

BASE_DECLARE(void) base_sql_queue_manager_execute_sql_event_callback(base_sql_queue_manager_t *qm, const char *sql,
    base_core_db_event_callback_func_t callback,
    void *pdata);
BASE_DECLARE(void) base_sql_queue_manager_execute_sql_event_callback_err(base_sql_queue_manager_t *qm, const char *sql,
    base_core_db_event_callback_func_t callback,
    base_core_db_err_callback_func_t err_callback,
    void *pdata);

BASE_DECLARE(pid_t) base_fork(void);

BASE_DECLARE(int) base_core_gen_certs(const char *prefix);
BASE_DECLARE(base_bool_t) base_core_check_dtls_pem(const char *file);
BASE_DECLARE(int) base_core_cert_gen_fingerprint(const char *prefix, dtls_fingerprint_t *fp);
BASE_DECLARE(int) base_core_cert_expand_fingerprint(dtls_fingerprint_t *fp, const char *str);
BASE_DECLARE(int) base_core_cert_verify(dtls_fingerprint_t *fp);
BASE_DECLARE(base_status_t) _base_core_session_request_video_refresh(base_core_session_t *session, int force, const char *file, const char *func, int line);
#define base_core_session_request_video_refresh(_s) _base_core_session_request_video_refresh(_s, 0, __FILE__, __BASE_FUNC__, __LINE__)
#define base_core_session_force_request_video_refresh(_s) _base_core_session_request_video_refresh(_s, 1, __FILE__, __BASE_FUNC__, __LINE__)
BASE_DECLARE(base_status_t) base_core_session_send_and_request_video_refresh(base_core_session_t *session);
BASE_DECLARE(int) base_system(const char *cmd, base_bool_t wait);
BASE_DECLARE(int) base_stream_system_fork(const char *cmd, base_stream_handle_t *stream);
BASE_DECLARE(int) base_stream_system(const char *cmd, base_stream_handle_t *stream);

BASE_DECLARE(int) base_spawn(const char *cmd, base_bool_t wait);
BASE_DECLARE(int) base_stream_spawn(const char *cmd, base_bool_t shell, base_bool_t wait, base_stream_handle_t *stream);

BASE_DECLARE(void) base_core_session_debug_pool(base_stream_handle_t *stream);

BASE_DECLARE(base_status_t) base_core_session_override_io_routines(base_core_session_t *session, base_io_routines_t *ior);

BASE_DECLARE(const char *)base_version_major(void);
BASE_DECLARE(const char *)base_version_minor(void);
BASE_DECLARE(const char *)base_version_micro(void);

BASE_DECLARE(const char *)base_version_revision(void);
BASE_DECLARE(const char *)base_version_revision_human(void);
BASE_DECLARE(const char *)base_version_full(void);
BASE_DECLARE(const char *)base_version_full_human(void);

BASE_DECLARE(void) base_core_autobind_cpu(void);

BASE_DECLARE(base_status_t) base_core_session_start_text_thread(base_core_session_t *session);

BASE_DECLARE(const char *) base_core_get_event_channel_key_separator(void);

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
