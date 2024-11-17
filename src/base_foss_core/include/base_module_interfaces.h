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
* Luke Dashjr <luke@openmethods.com> (OpenMethods, LLC)
*
*
* base_module_interfaces.h -- Module Interface Definitions
*
*/
/*! \file base_module_interfaces.h
\brief Module Interface Definitions

This module holds the definition of data abstractions used to implement various pluggable
interfaces and pluggable event handlers.

*/
#ifndef BASE_MODULE_INTERFACES_H
#define BASE_MODULE_INTERFACES_H

#include <base.h>

BASE_BEGIN_EXTERN_C

struct base_stream_handle {
    base_stream_handle_read_function_t read_function;
    base_stream_handle_write_function_t write_function;
    base_stream_handle_raw_write_function_t raw_write_function;
    void *data;
    void *end;
    base_size_t data_size;
    base_size_t data_len;
    base_size_t alloc_len;
    base_size_t alloc_chunk;
    base_event_t *param_event;
};

/*! \brief Abstract handler to a timer module */
struct base_timer {
    /*! time interval expressed in milliseconds */
    int interval;
    /*! flags to control behaviour */
    uint32_t flags;
    /*! sample count to increment by on each cycle */
    unsigned int samples;
    /*! current sample count based on samples parameter */
    uint32_t samplecount;
    /*! the timer interface provided from a loadable module */
    base_timer_interface_t *timer_interface;
    /*! the timer's memory pool */
    base_memory_pool_t *memory_pool;
    /*! private data for loadable modules to store information */
    void *private_info;
    /*! remaining time from last call to _check() */
    base_size_t diff;
    base_time_t start;
    uint64_t tick;
};

typedef enum {
    BASE_TIMER_FUNC_TIMER_INIT,
    BASE_TIMER_FUNC_TIMER_NEXT,
    BASE_TIMER_FUNC_TIMER_STEP,
    BASE_TIMER_FUNC_TIMER_SYNC,
    BASE_TIMER_FUNC_TIMER_CHECK,
    BASE_TIMER_FUNC_TIMER_DESTROY
} base_timer_func_name_t;

/*! \brief A table of functions that a timer module implements */
struct base_timer_interface {
    /*! the name of the interface */
    const char *interface_name;
    /*! function to allocate the timer */
    base_status_t (*timer_init) (base_timer_t *);
    /*! function to wait for one cycle to pass */
    base_status_t (*timer_next) (base_timer_t *);
    /*! function to step the timer one step */
    base_status_t (*timer_step) (base_timer_t *);
    /*! function to reset the timer  */
    base_status_t (*timer_sync) (base_timer_t *);
    /*! function to check if the current step has expired */
    base_status_t (*timer_check) (base_timer_t *, base_bool_t);
    /*! function to deallocate the timer */
    base_status_t (*timer_destroy) (base_timer_t *);
    base_thread_rwlock_t *rwlock;
    int refs;
    base_mutex_t *reflock;
    base_loadable_module_interface_t *parent;
    struct base_timer_interface *next;
};

#if 0

/*! \brief A module interface to implement an application */
struct base_application_interface {
    /*! the name of the interface */
    const char *interface_name;
    /*! function the application implements */
    base_application_function_t application_function;
    /*! the long winded description of the application */
    const char *long_desc;
    /*! the short and sweet description of the application */
    const char *short_desc;
    /*! an example of the application syntax */
    const char *syntax;
    /*! flags to control behaviour */
    uint32_t flags;
    base_thread_rwlock_t *rwlock;
    int refs;
    base_mutex_t *reflock;
    base_loadable_module_interface_t *parent;
    struct base_application_interface *next;
};

#endif

/*! \brief A module interface to implement an api function */
struct base_api_interface {
    /*! the name of the interface */
    const char *interface_name;
    /*! a description of the api function */
    const char *desc;
    /*! function the api call uses */
    base_api_function_t function;
    /*! an example of the api syntax */
    const char *syntax;
    base_thread_rwlock_t *rwlock;
    int refs;
    base_mutex_t *reflock;
    base_loadable_module_interface_t *parent;
    struct base_api_interface *next;
};

struct base_general_interface {
    base_general_interface_t *next;
    const char *interface_name;
    base_thread_rwlock_t *rwlock;
};



#if 0
/*! \brief A module interface to implement a json api function */
struct base_json_api_interface {
    /*! the name of the interface */
    const char *interface_name;
    /*! a description of the api function */
    const char *desc;
    /*! function the api call uses */
    base_json_api_function_t function;
    /*! an example of the api syntax */
    const char *syntax;
    base_thread_rwlock_t *rwlock;
    int refs;
    base_mutex_t *reflock;
    base_loadable_module_interface_t *parent;
    struct base_json_api_interface *next;
};
#endif

#define PROTECT_INTERFACE(_it) if (_it) {base_mutex_lock(_it->reflock); base_thread_rwlock_rdlock(_it->parent->rwlock); base_thread_rwlock_rdlock(_it->rwlock); _it->refs++; _it->parent->refs++; base_mutex_unlock(_it->reflock);} //if (!strcmp(_it->interface_name, "user")) base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_CRIT, "+++++++++++LOCK %s %d/%d\n", _it->interface_name, _it->refs, _it->parent->refs);
#define UNPROTECT_INTERFACE(_it) if (_it) {base_mutex_lock(_it->reflock); base_thread_rwlock_unlock(_it->rwlock); base_thread_rwlock_unlock(_it->parent->rwlock); _it->refs--; _it->parent->refs--; base_mutex_unlock(_it->reflock);}   //if (!strcmp(_it->interface_name, "user")) base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_CRIT, "---------UNLOCK %s %d/%d\n", _it->interface_name, _it->refs, _it->parent->refs);
/*
#include "base_frame.h"

struct base_slin_data {
base_core_session_t *session;
base_frame_t write_frame;
base_codec_t codec;
char frame_data[BASE_RECOMMENDED_BUFFER_SIZE];
};
*/
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
