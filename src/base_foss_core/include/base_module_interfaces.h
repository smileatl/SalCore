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
* Andrey Volk <andywolk@gmail.com>
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
/*! \brief A table of functions to execute at various states
*/
typedef enum {
    BASE_SHN_ON_INIT,
    BASE_SHN_ON_ROUTING,
    BASE_SHN_ON_EXECUTE,
    BASE_SHN_ON_HANGUP,
    BASE_SHN_ON_EXCHANGE_MEDIA,
    BASE_SHN_ON_SOFT_EXECUTE,
    BASE_SHN_ON_CONSUME_MEDIA,
    BASE_SHN_ON_HIBERNATE,
    BASE_SHN_ON_RESET,
    BASE_SHN_ON_PARK,
    BASE_SHN_ON_REPORTING,
    BASE_SHN_ON_DESTROY
} base_state_handler_name_t;

struct base_state_handler_table {
    /*! executed when the state changes to init */
    base_state_handler_t on_init;
    /*! executed when the state changes to routing */
    base_state_handler_t on_routing;
    /*! executed when the state changes to execute */
    base_state_handler_t on_execute;
    /*! executed when the state changes to hangup */
    base_state_handler_t on_hangup;
    /*! executed when the state changes to exchange_media */
    base_state_handler_t on_exchange_media;
    /*! executed when the state changes to soft_execute */
    base_state_handler_t on_soft_execute;
    /*! executed when the state changes to consume_media */
    base_state_handler_t on_consume_media;
    /*! executed when the state changes to hibernate */
    base_state_handler_t on_hibernate;
    /*! executed when the state changes to reset */
    base_state_handler_t on_reset;
    /*! executed when the state changes to park */
    base_state_handler_t on_park;
    /*! executed when the state changes to reporting */
    base_state_handler_t on_reporting;
    /*! executed when the state changes to destroy */
    base_state_handler_t on_destroy;
    int flags;
    void *padding[10];
};

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

struct base_io_event_hooks;
struct base_say_file_handle;

typedef base_call_cause_t (*base_io_outgoing_channel_t)
(base_core_session_t *, base_event_t *, base_caller_profile_t *, base_core_session_t **, base_memory_pool_t **, base_originate_flag_t,
    base_call_cause_t *);
typedef base_status_t (*base_io_read_frame_t) (base_core_session_t *, base_frame_t **, base_io_flag_t, int);
typedef base_status_t (*base_io_write_frame_t) (base_core_session_t *, base_frame_t *, base_io_flag_t, int);
typedef base_status_t (*base_io_kill_channel_t) (base_core_session_t *, int);
typedef base_status_t (*base_io_send_dtmf_t) (base_core_session_t *, const base_dtmf_t *);
typedef base_status_t (*base_io_receive_message_t) (base_core_session_t *, base_core_session_message_t *);
typedef base_status_t (*base_io_receive_event_t) (base_core_session_t *, base_event_t *);
typedef base_status_t (*base_io_state_change_t) (base_core_session_t *);
typedef base_status_t (*base_io_state_run_t) (base_core_session_t *);
typedef base_status_t (*base_io_read_video_frame_t) (base_core_session_t *, base_frame_t **, base_io_flag_t, int);
typedef base_status_t (*base_io_write_video_frame_t) (base_core_session_t *, base_frame_t *, base_io_flag_t, int);
typedef base_status_t (*base_io_read_text_frame_t) (base_core_session_t *, base_frame_t **, base_io_flag_t, int);
typedef base_status_t (*base_io_write_text_frame_t) (base_core_session_t *, base_frame_t *, base_io_flag_t, int);
typedef base_jb_t *(*base_io_get_jb_t) (base_core_session_t *, base_media_type_t);

typedef enum {
    BASE_IO_OUTGOING_CHANNEL,
    BASE_IO_READ_FRAME,
    BASE_IO_WRITE_FRAME,
    BASE_IO_KILL_CHANNEL,
    BASE_IO_SEND_DTMF,
    BASE_IO_RECEIVE_MESSAGE,
    BASE_IO_RECEIVE_EVENT,
    BASE_IO_STATE_CHANGE,
    BASE_IO_READ_VIDEO_FRAME,
    BASE_IO_WRITE_VIDEO_FRAME,
    BASE_IO_READ_TEXT_FRAME,
    BASE_IO_WRITE_TEXT_FRAME,
    BASE_IO_GET_JB,
} base_io_routine_name_t;

/*! \brief A table of i/o routines that an endpoint interface can implement */
struct base_io_routines {
    /*! creates an outgoing session from given session, caller profile */
    base_io_outgoing_channel_t outgoing_channel;
    /*! read a frame from a session */
    base_io_read_frame_t read_frame;
    /*! write a frame to a session */
    base_io_write_frame_t write_frame;
    /*! send a kill signal to the session's channel */
    base_io_kill_channel_t kill_channel;
    /*! send a string of DTMF digits to a session's channel */
    base_io_send_dtmf_t send_dtmf;
    /*! receive a message from another session */
    base_io_receive_message_t receive_message;
    /*! queue a message for another session */
    base_io_receive_event_t receive_event;
    /*! change a sessions channel state */
    base_io_state_change_t state_change;
    /*! read a video frame from a session */
    base_io_read_video_frame_t read_video_frame;
    /*! write a video frame to a session */
    base_io_write_video_frame_t write_video_frame;
    /*! read a video frame from a session */
    base_io_read_text_frame_t read_text_frame;
    /*! write a video frame to a session */
    base_io_write_text_frame_t write_text_frame;
    /*! change a sessions channel run state */
    base_io_state_run_t state_run;
    /*! get sessions jitterbuffer */
    base_io_get_jb_t get_jb;
    void *padding[10];
};

/*! \brief Abstraction of an module endpoint interface
This is the glue between the abstract idea of a "channel" and what is really going on under the
hood.	 Each endpoint module fills out one of these tables and makes it available when a channel
is created of it's paticular type.
*/

struct base_endpoint_interface {
    /*! the interface's name */
    const char *interface_name;

    /*! channel abstraction methods */
    base_io_routines_t *io_routines;

    /*! state machine methods */
    base_state_handler_table_t *state_handler;

    /*! private information */
    void *private_info;

    base_thread_rwlock_t *rwlock;
    int refs;
    base_mutex_t *reflock;

    /* parent */
    base_loadable_module_interface_t *parent;

    /* to facilitate linking */
    struct base_endpoint_interface *next;

    base_core_recover_callback_t recover_callback;

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
    uint32_t last_samplecount;
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

/*! \brief Abstract interface to a dialplan module */
struct base_dialplan_interface {
    /*! the name of the interface */
    const char *interface_name;
    /*! the function to read an extension and set a channels dialpan */
    base_dialplan_hunt_function_t hunt_function;
    base_thread_rwlock_t *rwlock;
    int refs;
    base_mutex_t *reflock;
    base_loadable_module_interface_t *parent;
    struct base_dialplan_interface *next;
};

/*! \brief Abstract interface to a file format module */
struct base_file_interface {
    /*! the name of the interface */
    const char *interface_name;
    /*! function to open the file */
    base_status_t (*file_open) (base_file_handle_t *, const char *file_path);
    /*! function to close the file */
    base_status_t (*file_close) (base_file_handle_t *);
    /*! function to close the file */
    base_status_t (*file_truncate) (base_file_handle_t *, int64_t offset);
    /*! function to read from the file */
    base_status_t (*file_read) (base_file_handle_t *, void *data, base_size_t *len);
    /*! function to write from the file */
    base_status_t (*file_write) (base_file_handle_t *, void *data, base_size_t *len);
    /*! function to seek to a certian position in the file */
    base_status_t (*file_read_video) (base_file_handle_t *, base_frame_t *frame, base_video_read_flag_t flags);
    /*! function to write from the file */
    base_status_t (*file_write_video) (base_file_handle_t *, base_frame_t *frame);
    /*! function to seek to a certian position in the file */
    base_status_t (*file_seek) (base_file_handle_t *, unsigned int *cur_pos, int64_t samples, int whence);
    /*! function to set meta data */
    base_status_t (*file_set_string) (base_file_handle_t *fh, base_audio_col_t col, const char *string);
    /*! function to get meta data */
    base_status_t (*file_get_string) (base_file_handle_t *fh, base_audio_col_t col, const char **string);
    /*! function to pre close the file to read params */
    base_status_t (*file_pre_close) (base_file_handle_t *fh);
    /*! function to control the underlying tech of the file  */
    base_status_t (*file_command) (base_file_handle_t *fh, base_file_command_t command);
    /*! list of supported file extensions */
    char **extens;
    base_thread_rwlock_t *rwlock;
    int refs;
    base_mutex_t *reflock;
    base_loadable_module_interface_t *parent;
    struct base_file_interface *next;
};

/*! \brief Abstract interface to an asr module */
struct base_asr_interface {
    /*! the name of the interface */
    const char *interface_name;
    /*! function to open the asr interface */
    base_status_t (*asr_open) (base_asr_handle_t *ah, const char *codec, int rate, const char *dest, base_asr_flag_t *flags);
    /*! function to load a grammar to the asr interface */
    base_status_t (*asr_load_grammar) (base_asr_handle_t *ah, const char *grammar, const char *name);
    /*! function to unload a grammar to the asr interface */
    base_status_t (*asr_unload_grammar) (base_asr_handle_t *ah, const char *name);
    /*! function to close the asr interface */
    base_status_t (*asr_close) (base_asr_handle_t *ah, base_asr_flag_t *flags);
    /*! function to feed audio to the ASR */
    base_status_t (*asr_feed) (base_asr_handle_t *ah, void *data, unsigned int len, base_asr_flag_t *flags);
    /*! function to resume the ASR */
    base_status_t (*asr_resume) (base_asr_handle_t *ah);
    /*! function to pause the ASR */
    base_status_t (*asr_pause) (base_asr_handle_t *ah);
    /*! function to read results from the ASR */
    base_status_t (*asr_check_results) (base_asr_handle_t *ah, base_asr_flag_t *flags);
    /*! function to read results from the ASR */
    base_status_t (*asr_get_results) (base_asr_handle_t *ah, char **xmlstr, base_asr_flag_t *flags);
    /*! function to read result headers from the ASR */
    base_status_t (*asr_get_result_headers) (base_asr_handle_t *ah, base_event_t **headers, base_asr_flag_t *flags);
    /*! function to start ASR input timers */
    base_status_t (*asr_start_input_timers) (base_asr_handle_t *ah);
    void (*asr_text_param) (base_asr_handle_t *ah, char *param, const char *val);
    void (*asr_numeric_param) (base_asr_handle_t *ah, char *param, int val);
    void (*asr_float_param) (base_asr_handle_t *ah, char *param, double val);
    base_thread_rwlock_t *rwlock;
    int refs;
    base_mutex_t *reflock;
    base_loadable_module_interface_t *parent;
    struct base_asr_interface *next;
    /*! function to enable a grammar to the asr interface */
    base_status_t (*asr_enable_grammar) (base_asr_handle_t *ah, const char *name);
    /*! function to disable a grammar to the asr interface */
    base_status_t (*asr_disable_grammar) (base_asr_handle_t *ah, const char *name);
    /*! function to disable all grammars to the asr interface */
    base_status_t (*asr_disable_all_grammars) (base_asr_handle_t *ah);
    /*! function to feed DTMF to the ASR */
    base_status_t (*asr_feed_dtmf) (base_asr_handle_t *ah, const base_dtmf_t *dtmf, base_asr_flag_t *flags);
};

/*! \brief Abstract interface to a speech module */
struct base_speech_interface {
    /*! the name of the interface */
    const char *interface_name;
    /*! function to open the speech interface */
    base_status_t (*speech_open) (base_speech_handle_t *sh, const char *voice_name, int rate, int channels, base_speech_flag_t *flags);
    /*! function to close the speech interface */
    base_status_t (*speech_close) (base_speech_handle_t *, base_speech_flag_t *flags);
    /*! function to feed audio to the ASR */
    base_status_t (*speech_feed_tts) (base_speech_handle_t *sh, char *text, base_speech_flag_t *flags);
    /*! function to read audio from the TTS */
    base_status_t (*speech_read_tts) (base_speech_handle_t *sh, void *data, base_size_t *datalen, base_speech_flag_t *flags);
    void (*speech_flush_tts) (base_speech_handle_t *sh);
    void (*speech_text_param_tts) (base_speech_handle_t *sh, char *param, const char *val);
    void (*speech_numeric_param_tts) (base_speech_handle_t *sh, char *param, int val);
    void (*speech_float_param_tts) (base_speech_handle_t *sh, char *param, double val);
    base_thread_rwlock_t *rwlock;
    int refs;
    base_mutex_t *reflock;
    base_loadable_module_interface_t *parent;
    struct base_speech_interface *next;
};

/*! \brief Abstract interface to a say module */
struct base_say_interface {
    /*! the name of the interface */
    const char *interface_name;
    /*! function to pass down to the module */
    base_say_callback_t say_function;
    base_say_string_callback_t say_string_function;
    base_thread_rwlock_t *rwlock;
    int refs;
    base_mutex_t *reflock;
    base_loadable_module_interface_t *parent;
    struct base_say_interface *next;
};

/*! \brief Abstract interface to a chat module */
struct base_chat_interface {
    /*! the name of the interface */
    const char *interface_name;
    /*! function to open the directory interface */
    base_status_t (*chat_send) (base_event_t *message_event);

    base_thread_rwlock_t *rwlock;
    int refs;
    base_mutex_t *reflock;
    base_loadable_module_interface_t *parent;
    struct base_chat_interface *next;
};

/*! \brief Abstract interface to a management module */
struct base_management_interface {
    /*! the name of the interface */
    const char *relative_oid;
    /*! function to open the directory interface */
    base_status_t (*management_function) (char *relative_oid, base_management_action_t action, char *data, base_size_t datalen);
    base_thread_rwlock_t *rwlock;
    int refs;
    base_mutex_t *reflock;
    base_loadable_module_interface_t *parent;
    struct base_management_interface *next;
};

/*! \brief Abstract interface to a limit module */
struct base_limit_interface {
    /*! name of the interface */
    const char *interface_name;
    /*! increment */
    base_status_t (*incr) (base_core_session_t *session, const char *realm, const char *resource, const int max, const int interval);
    /*! release */
    base_status_t (*release) (base_core_session_t *session, const char *realm, const char *resource);
    /*! usage for resource */
    int (*usage) (const char *realm, const char *resource, uint32_t *rcount);
    /*! reset counters */
    base_status_t (*reset) (void);
    /*! freform status */
    char * (*status) (void);
    /*! reset interval counter */
    base_status_t (*interval_reset) (const char *realm, const char *resource);
    /* internal */
    base_thread_rwlock_t *rwlock;
    int refs;
    base_mutex_t *reflock;
    base_loadable_module_interface_t *parent;
    struct base_limit_interface *next;
};

/*! \brief Abstract interface to a directory module */
struct base_directory_interface {
    /*! the name of the interface */
    const char *interface_name;
    /*! function to open the directory interface */
    base_status_t (*directory_open) (base_directory_handle_t *dh, char *source, char *dsn, char *passwd);
    /*! function to close the directory interface */
    base_status_t (*directory_close) (base_directory_handle_t *dh);
    /*! function to query the directory interface */
    base_status_t (*directory_query) (base_directory_handle_t *dh, char *base, char *query);
    /*! function to advance to the next record */
    base_status_t (*directory_next) (base_directory_handle_t *dh);
    /*! function to advance to the next name/value pair in the current record */
    base_status_t (*directory_next_pair) (base_directory_handle_t *dh, char **var, char **val);
    base_thread_rwlock_t *rwlock;
    int refs;
    base_mutex_t *reflock;
    base_loadable_module_interface_t *parent;
    struct base_directory_interface *next;
};

/*! an abstract representation of a directory interface. */
struct base_directory_handle {
    /*! the interface of the module that implemented the current directory interface */
    base_directory_interface_t *directory_interface;
    /*! flags to control behaviour */
    uint32_t flags;

    /*! the handle's memory pool */
    base_memory_pool_t *memory_pool;
    /*! private data for the format module to store handle specific info */
    void *private_info;
};

struct base_audio_codec_settings {
    int unused;
};

struct base_video_codec_settings {
    uint32_t bandwidth;
    int32_t width;
    int32_t height;
    uint8_t try_hardware_encoder;
    uint8_t fps;
    char config_profile_name[64];
};

union base_codec_settings {
    struct base_audio_codec_settings audio;
    struct base_video_codec_settings video;
};

/*! an abstract handle of a fmtp parsed by codec */
struct base_codec_fmtp {
    /*! actual samples transferred per second for those who are not moron g722 RFC writers */
    uint32_t actual_samples_per_second;
    /*! bits transferred per second */
    int bits_per_second;
    /*! number of microseconds of media in one packet (ptime * 1000) */
    int microseconds_per_packet;
    /*! maximum ptime in ms */
    int max_ptime;
    /*! minimum ptime in ms */
    int min_ptime;
    /*! stereo, typically bidirectional */
    int stereo; 
    /* sender properties (stereo) */
    int sprop_stereo;
    /*! private data for the codec module to store handle specific info */
    void *private_info;

};

struct base_picture {
    uint32_t width;      /* the picture width */
    uint32_t height;     /* the picture height */
    uint8_t *planes[4];  /* pointer to the top left pixel for each plane */
    uint32_t stride[4];  /* stride between rows for each plane */
};

/*! an abstract handle to a codec module */
struct base_codec {
    /*! the codec interface table this handle uses */
    base_codec_interface_t *codec_interface;
    /*! the specific implementation of the above codec */
    const base_codec_implementation_t *implementation;
    /*! fmtp line from remote sdp */
    char *fmtp_in;
    /*! fmtp line for local sdp */
    char *fmtp_out;
    /*! flags to modify behaviour */
    uint32_t flags;
    /*! the handle's memory pool */
    base_memory_pool_t *memory_pool;
    /*! private data for the codec module to store handle specific info */
    void *private_info;
    base_payload_t agreed_pt;
    base_mutex_t *mutex;
    struct base_codec *next;
    base_core_session_t *session;
    base_frame_t *cur_frame;
};

/*! \brief A table of settings and callbacks that define a paticular implementation of a codec */
struct base_codec_implementation {
    /*! enumeration defining the type of the codec */
    base_codec_type_t codec_type;
    /*! the IANA code number */
    base_payload_t ianacode;
    /*! the IANA code name */
    char *iananame;
    /*! default fmtp to send (can be overridden by the init function) */
    char *fmtp;
    /*! samples transferred per second */
    uint32_t samples_per_second;
    /*! actual samples transferred per second for those who are not moron g722 RFC writers */
    uint32_t actual_samples_per_second;
    /*! bits transferred per second */
    int bits_per_second;
    /*! number of microseconds of media in one packet (ptime * 1000) */
    int microseconds_per_packet;
    /*! number of samples in one packet */
    uint32_t samples_per_packet;
    /*! number of bytes one packet will decompress to */
    uint32_t decoded_bytes_per_packet;
    /*! number of encoded bytes in the RTP payload */
    uint32_t encoded_bytes_per_packet;
    /*! number of channels represented */
    uint8_t number_of_channels;
    /*! number of codec frames packetized into one packet */
    int codec_frames_per_packet;
    /*! function to initialize a codec handle using this implementation */
    base_core_codec_init_func_t init;
    /*! function to encode raw data into encoded data */
    base_core_codec_encode_func_t encode;
    /*! function to decode encoded data into raw data */
    base_core_codec_decode_func_t decode;
    /*! function to encode video raw data into encoded data */
    base_core_codec_video_encode_func_t encode_video;
    /*! function to decode video encoded data into raw data */
    base_core_codec_video_decode_func_t decode_video;
    /*! function to send control messages to the codec */
    base_core_codec_control_func_t codec_control;
    /*! deinitalize a codec handle using this implementation */
    base_core_codec_destroy_func_t destroy;
    uint32_t codec_id;
    uint32_t impl_id;
    char *modname;
    struct base_codec_implementation *next;
};

/*! \brief Top level module interface to implement a series of codec implementations */
struct base_codec_interface {
    /*! the name of the interface */
    const char *interface_name;
    /*! a list of codec implementations related to the codec */
    base_codec_implementation_t *implementations;
    /*! function to decode a codec fmtp parameters */
    base_core_codec_fmtp_parse_func_t parse_fmtp;
    uint32_t codec_id;
    base_thread_rwlock_t *rwlock;
    int refs;
    base_mutex_t *reflock;
    char *modname;
    base_loadable_module_interface_t *parent;
    struct base_codec_interface *next;
};

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

/*! \brief A module interface to implement a chat application */
struct base_chat_application_interface {
    /*! the name of the interface */
    const char *interface_name;
    /*! function the application implements */
    base_chat_application_function_t chat_application_function;
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
    struct base_chat_application_interface *next;
};

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

#define PROTECT_INTERFACE(_it) if (_it) {base_thread_rwlock_rdlock(_it->parent->rwlock); base_thread_rwlock_rdlock(_it->rwlock); base_mutex_lock(_it->reflock); _it->refs++; _it->parent->refs++; base_mutex_unlock(_it->reflock);}	//if (!strcmp(_it->interface_name, "user")) base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_CRIT, "+++++++++++LOCK %s %d/%d\n", _it->interface_name, _it->refs, _it->parent->refs);
#define UNPROTECT_INTERFACE(_it) if (_it) {base_mutex_lock(_it->reflock); _it->refs--; _it->parent->refs--; base_mutex_unlock(_it->reflock); base_thread_rwlock_unlock(_it->rwlock); base_thread_rwlock_unlock(_it->parent->rwlock);}	//if (!strcmp(_it->interface_name, "user")) base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_CRIT, "---------UNLOCK %s %d/%d\n", _it->interface_name, _it->refs, _it->parent->refs);

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
