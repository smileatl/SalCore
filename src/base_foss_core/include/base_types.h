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
* Bret McDanel <trixter AT 0xdecafbad dot com>
* Joseph Sullivan <jossulli@amazon.com>
* Raymond Chandler <intralanman@freebase.org>
* Emmanuel Schmidbauer <e.schmidbauer@gmail.com>
* Andrey Volk <andywolk@gmail.com>
*
* base_types.h -- Data Types
*
*/
/*! \file base_types.h
\brief Data Types
*/
#ifndef BASE_TYPES_H
#define BASE_TYPES_H

#include <base.h>

BASE_BEGIN_EXTERN_C
#define BASE_ENT_ORIGINATE_DELIM ":_:"
#define BASE_BLANK_STRING ""
#define BASE_TON_UNDEF 255
#define BASE_NUMPLAN_UNDEF 255
#ifdef WIN32
#define BASE_SEQ_FWHITE FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY
#define BASE_SEQ_FRED FOREGROUND_RED | FOREGROUND_INTENSITY
#define BASE_SEQ_FMAGEN FOREGROUND_BLUE | FOREGROUND_RED
#define BASE_SEQ_FCYAN FOREGROUND_GREEN | FOREGROUND_BLUE
#define BASE_SEQ_FGREEN FOREGROUND_GREEN
#define BASE_SEQ_FYELLOW FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY
#define BASE_SEQ_DEFAULT_COLOR BASE_SEQ_FWHITE
#else
#define BASE_SEQ_ESC "\033["
/* Ansi Control character suffixes */
#define BASE_SEQ_HOME_CHAR 'H'
#define BASE_SEQ_HOME_CHAR_STR "H"
#define BASE_SEQ_CLEARLINE_CHAR '1'
#define BASE_SEQ_CLEARLINE_CHAR_STR "1"
#define BASE_SEQ_CLEARLINEEND_CHAR "K"
#define BASE_SEQ_CLEARSCR_CHAR0 '2'
#define BASE_SEQ_CLEARSCR_CHAR1 'J'
#define BASE_SEQ_CLEARSCR_CHAR "2J"
#define BASE_SEQ_DEFAULT_COLOR BASE_SEQ_ESC BASE_SEQ_END_COLOR	/* Reset to Default fg/bg color */
#define BASE_SEQ_AND_COLOR ";"	/* To add multiple color definitions */
#define BASE_SEQ_END_COLOR "m"	/* To end color definitions */
/* Foreground colors values */
#define BASE_SEQ_F_BLACK "30"
#define BASE_SEQ_F_RED "31"
#define BASE_SEQ_F_GREEN "32"
#define BASE_SEQ_F_YELLOW "33"
#define BASE_SEQ_F_BLUE "34"
#define BASE_SEQ_F_MAGEN "35"
#define BASE_SEQ_F_CYAN "36"
#define BASE_SEQ_F_WHITE "37"
/* Background colors values */
#define BASE_SEQ_B_BLACK "40"
#define BASE_SEQ_B_RED "41"
#define BASE_SEQ_B_GREEN "42"
#define BASE_SEQ_B_YELLOW "43"
#define BASE_SEQ_B_BLUE "44"
#define BASE_SEQ_B_MAGEN "45"
#define BASE_SEQ_B_CYAN "46"
#define BASE_SEQ_B_WHITE "47"
/* Preset escape sequences - Change foreground colors only */
#define BASE_SEQ_FBLACK BASE_SEQ_ESC BASE_SEQ_F_BLACK BASE_SEQ_END_COLOR
#define BASE_SEQ_FRED BASE_SEQ_ESC BASE_SEQ_F_RED BASE_SEQ_END_COLOR
#define BASE_SEQ_FGREEN BASE_SEQ_ESC BASE_SEQ_F_GREEN BASE_SEQ_END_COLOR
#define BASE_SEQ_FYELLOW BASE_SEQ_ESC BASE_SEQ_F_YELLOW BASE_SEQ_END_COLOR
#define BASE_SEQ_FBLUE BASE_SEQ_ESC BASE_SEQ_F_BLUE BASE_SEQ_END_COLOR
#define BASE_SEQ_FMAGEN BASE_SEQ_ESC BASE_SEQ_F_MAGEN BASE_SEQ_END_COLOR
#define BASE_SEQ_FCYAN BASE_SEQ_ESC BASE_SEQ_F_CYAN BASE_SEQ_END_COLOR
#define BASE_SEQ_FWHITE BASE_SEQ_ESC BASE_SEQ_F_WHITE BASE_SEQ_END_COLOR
#define BASE_SEQ_BBLACK BASE_SEQ_ESC BASE_SEQ_B_BLACK BASE_SEQ_END_COLOR
#define BASE_SEQ_BRED BASE_SEQ_ESC BASE_SEQ_B_RED BASE_SEQ_END_COLOR
#define BASE_SEQ_BGREEN BASE_SEQ_ESC BASE_SEQ_B_GREEN BASE_SEQ_END_COLOR
#define BASE_SEQ_BYELLOW BASE_SEQ_ESC BASE_SEQ_B_YELLOW BASE_SEQ_END_COLOR
#define BASE_SEQ_BBLUE BASE_SEQ_ESC BASE_SEQ_B_BLUE BASE_SEQ_END_COLOR
#define BASE_SEQ_BMAGEN BASE_SEQ_ESC BASE_SEQ_B_MAGEN BASE_SEQ_END_COLOR
#define BASE_SEQ_BCYAN BASE_SEQ_ESC BASE_SEQ_B_CYAN BASE_SEQ_END_COLOR
#define BASE_SEQ_BWHITE BASE_SEQ_ESC BASE_SEQ_B_WHITE BASE_SEQ_END_COLOR
/* Preset escape sequences */
#define BASE_SEQ_HOME BASE_SEQ_ESC BASE_SEQ_HOME_CHAR_STR
#define BASE_SEQ_CLEARLINE BASE_SEQ_ESC BASE_SEQ_CLEARLINE_CHAR_STR
#define BASE_SEQ_CLEARLINEEND BASE_SEQ_ESC BASE_SEQ_CLEARLINEEND_CHAR
#define BASE_SEQ_CLEARSCR BASE_SEQ_ESC BASE_SEQ_CLEARSCR_CHAR BASE_SEQ_HOME
#endif
#define BASE_DEFAULT_CLID_NAME ""
#define BASE_DEFAULT_CLID_NUMBER "0000000000"
#define BASE_DEFAULT_DTMF_DURATION 2000
#define BASE_DEFAULT_TIMEOUT 60
#define BASE_MIN_DTMF_DURATION 400
#define BASE_MAX_DTMF_DURATION 192000
#define BASE_DEFAULT_DIR_PERMS BASE_FPROT_UREAD | BASE_FPROT_UWRITE | BASE_FPROT_UEXECUTE | BASE_FPROT_GREAD | BASE_FPROT_GEXECUTE
#ifdef WIN32
#define BASE_PATH_SEPARATOR "/"
#else
#define BASE_PATH_SEPARATOR "/"
#endif
#define BASE_URL_SEPARATOR "://"
#define BASE_IGNORE_DISPLAY_UPDATES_VARIABLE "ignore_display_updates"
#define BASE_AUDIO_SPOOL_PATH_VARIABLE "audio_spool_path"
#define BASE_BRIDGE_HANGUP_CAUSE_VARIABLE "bridge_hangup_cause"
#define BASE_READ_TERMINATOR_USED_VARIABLE "read_terminator_used"
#define BASE_SEND_SILENCE_WHEN_IDLE_VARIABLE "send_silence_when_idle"
#define BASE_CURRENT_APPLICATION_VARIABLE "current_application"
#define BASE_CURRENT_APPLICATION_DATA_VARIABLE "current_application_data"
#define BASE_CURRENT_APPLICATION_RESPONSE_VARIABLE "current_application_response"
#define BASE_PASSTHRU_PTIME_MISMATCH_VARIABLE "passthru_ptime_mismatch"
#define BASE_ENABLE_HEARTBEAT_EVENTS_VARIABLE "enable_heartbeat_events"
#define BASE_BYPASS_MEDIA_AFTER_BRIDGE_VARIABLE "bypass_media_after_bridge"
#define BASE_READ_RESULT_VARIABLE "read_result"
#define BASE_ATT_XFER_RESULT_VARIABLE "att_xfer_result"
#define BASE_COPY_XML_CDR_VARIABLE "copy_xml_cdr"
#define BASE_COPY_JSON_CDR_VARIABLE "copy_json_cdr"
#define BASE_CURRENT_APPLICATION_VARIABLE "current_application"
#define BASE_PROTO_SPECIFIC_HANGUP_CAUSE_VARIABLE "proto_specific_hangup_cause"
#define BASE_TRANSFER_HISTORY_VARIABLE "transfer_history"
#define BASE_TRANSFER_SOURCE_VARIABLE "transfer_source"
#define BASE_SENSITIVE_DTMF_VARIABLE "sensitive_dtmf"
#define BASE_RECORD_POST_PROCESS_EXEC_APP_VARIABLE "record_post_process_exec_app"
#define BASE_RECORD_POST_PROCESS_EXEC_API_VARIABLE "record_post_process_exec_api"

#define BASE_CHANNEL_EXECUTE_ON_ANSWER_VARIABLE "execute_on_answer"
#define BASE_CHANNEL_EXECUTE_ON_PRE_ANSWER_VARIABLE "execute_on_pre_answer"
#define BASE_CHANNEL_EXECUTE_ON_MEDIA_VARIABLE "execute_on_media"
#define BASE_CHANNEL_EXECUTE_ON_RING_VARIABLE "execute_on_ring"
#define BASE_CHANNEL_EXECUTE_ON_TONE_DETECT_VARIABLE "execute_on_tone_detect"
#define BASE_CHANNEL_EXECUTE_ON_ORIGINATE_VARIABLE "execute_on_originate"
#define BASE_CHANNEL_EXECUTE_ON_POST_ORIGINATE_VARIABLE "execute_on_post_originate"
#define BASE_CHANNEL_EXECUTE_ON_PRE_ORIGINATE_VARIABLE "execute_on_pre_originate"

#define BASE_CHANNEL_EXECUTE_ON_PRE_BRIDGE_VARIABLE "execute_on_pre_bridge"
#define BASE_CHANNEL_EXECUTE_ON_POST_BRIDGE_VARIABLE "execute_on_post_bridge"

#define BASE_CHANNEL_API_ON_ANSWER_VARIABLE "api_on_answer"
#define BASE_CHANNEL_API_ON_PRE_ANSWER_VARIABLE "api_on_pre_answer"
#define BASE_CHANNEL_API_ON_MEDIA_VARIABLE "api_on_media"
#define BASE_CHANNEL_API_ON_RING_VARIABLE "api_on_ring"
#define BASE_CHANNEL_API_ON_TONE_DETECT_VARIABLE "api_on_tone_detect"
#define BASE_CHANNEL_API_ON_ORIGINATE_VARIABLE "api_on_originate"
#define BASE_CHANNEL_API_ON_POST_ORIGINATE_VARIABLE "api_on_post_originate"
#define BASE_CHANNEL_API_ON_PRE_ORIGINATE_VARIABLE "api_on_pre_originate"

#define BASE_CALL_TIMEOUT_VARIABLE "call_timeout"
#define BASE_HOLDING_UUID_VARIABLE "holding_uuid"
#define BASE_SOFT_HOLDING_UUID_VARIABLE "soft_holding_uuid"
#define BASE_API_BRIDGE_END_VARIABLE "api_after_bridge"
#define BASE_API_BRIDGE_START_VARIABLE "api_before_bridge"
#define BASE_API_HANGUP_HOOK_VARIABLE "api_hangup_hook"
#define BASE_API_REPORTING_HOOK_VARIABLE "api_reporting_hook"
#define BASE_SESSION_IN_HANGUP_HOOK_VARIABLE "session_in_hangup_hook"
#define BASE_PROCESS_CDR_VARIABLE "process_cdr"
#define BASE_SKIP_CDR_CAUSES_VARIABLE "skip_cdr_causes"
#define BASE_FORCE_PROCESS_CDR_VARIABLE "force_process_cdr"
#define BASE_BRIDGE_CHANNEL_VARIABLE "bridge_channel"
#define BASE_CHANNEL_NAME_VARIABLE "channel_name"
#define BASE_BRIDGE_UUID_VARIABLE "bridge_uuid"
#define BASE_CONTINUE_ON_FAILURE_VARIABLE "continue_on_fail"
#define BASE_PLAYBACK_TERMINATORS_VARIABLE "playback_terminators"
#define BASE_PLAYBACK_TERMINATOR_USED "playback_terminator_used"
#define BASE_CACHE_SPEECH_HANDLES_VARIABLE "cache_speech_handles"
#define BASE_CACHE_SPEECH_HANDLES_OBJ_NAME "__cache_speech_handles_obj__"
#define BASE_BYPASS_MEDIA_VARIABLE "bypass_media"
#define BASE_PROXY_MEDIA_VARIABLE "proxy_media"
#define BASE_ENDPOINT_DISPOSITION_VARIABLE "endpoint_disposition"
#define BASE_HOLD_MUSIC_VARIABLE "hold_music"
#define BASE_TEMP_HOLD_MUSIC_VARIABLE "temp_hold_music"
#define BASE_EXPORT_VARS_VARIABLE "export_vars"
#define BASE_BRIDGE_EXPORT_VARS_VARIABLE "bridge_export_vars"
#define BASE_R_SDP_VARIABLE "base_r_sdp"
#define BASE_L_SDP_VARIABLE "base_l_sdp"
#define BASE_B_SDP_VARIABLE "base_m_sdp"
#define BASE_BRIDGE_VARIABLE "bridge_to"
#define BASE_LAST_BRIDGE_VARIABLE "last_bridge_to"
#define BASE_SIGNAL_BRIDGE_VARIABLE "signal_bridge_to"
#define BASE_SIGNAL_BOND_VARIABLE "signal_bond"
#define BASE_ORIGINATE_SIGNAL_BOND_VARIABLE "originate_signal_bond"
#define BASE_ORIGINATOR_VARIABLE "originator"
#define BASE_ORIGINATOR_CODEC_VARIABLE "originator_codec"
#define BASE_ORIGINATOR_VIDEO_CODEC_VARIABLE "originator_video_codec"
#define BASE_LOCAL_MEDIA_IP_VARIABLE "local_media_ip"
#define BASE_LOCAL_MEDIA_PORT_VARIABLE "local_media_port"
#define BASE_ADVERTISED_MEDIA_IP_VARIABLE "advertised_media_ip"
#define BASE_REMOTE_MEDIA_IP_VARIABLE "remote_media_ip"
#define BASE_REMOTE_MEDIA_PORT_VARIABLE "remote_media_port"
#define BASE_REMOTE_VIDEO_IP_VARIABLE "remote_video_ip"
#define BASE_REMOTE_VIDEO_PORT_VARIABLE "remote_video_port"
#define BASE_LOCAL_VIDEO_IP_VARIABLE "local_video_ip"
#define BASE_LOCAL_VIDEO_PORT_VARIABLE "local_video_port"
#define BASE_LOCAL_TEXT_IP_VARIABLE "local_text_ip"
#define BASE_LOCAL_TEXT_PORT_VARIABLE "local_text_port"
#define BASE_HANGUP_AFTER_BRIDGE_VARIABLE "hangup_after_bridge"
#define BASE_PARK_AFTER_BRIDGE_VARIABLE "park_after_bridge"
#define BASE_PARK_AFTER_EARLY_BRIDGE_VARIABLE "park_after_early_bridge"
#define BASE_TRANSFER_AFTER_BRIDGE_VARIABLE "transfer_after_bridge"
#define BASE_TRANSFER_AFTER_EARLY_BRIDGE_VARIABLE "transfer_after_early_bridge"
#define BASE_EXEC_AFTER_BRIDGE_APP_VARIABLE "exec_after_bridge_app"
#define BASE_EXEC_AFTER_BRIDGE_ARG_VARIABLE "exec_after_bridge_arg"
#define BASE_MAX_FORWARDS_VARIABLE "max_forwards"
#define BASE_RFC7989_SESSION_ID_VARIABLE "session_uuid"
#define BASE_RFC7989_REMOTE_SESSION_ID_VARIABLE "remote_session_uuid"
#define BASE_RFC7989_APP_SESSION_ID_VARIABLE "app_session_uuid"
#define BASE_RFC7989_GENERIC_PARAM_VARIABLE "generic_param_session_uuid"
#define BASE_MAX_SESSION_TRANSFERS_VARIABLE "max_session_transfers"
#define BASE_DISABLE_APP_LOG_VARIABLE "disable_app_log"
#define BASE_SPEECH_KEY "speech"
#define BASE_UUID_BRIDGE "uuid_bridge"
#define BASE_BITS_PER_BYTE 8
#define BASE_DEFAULT_FILE_BUFFER_LEN 65536
#define BASE_DTMF_LOG_LEN 1000
#define BASE_MAX_TRANS 2000
#define BASE_CORE_SESSION_MAX_PRIVATES 2
#define BASE_DEFAULT_VIDEO_SIZE 1200
#define BASE_RTCP_AUDIO_INTERVAL_MSEC "1000"
#define BASE_RTCP_VIDEO_INTERVAL_MSEC "1000"

#define TEXT_UNICODE_LINEFEED {0xe2, 0x80, 0xa8}
#define MAX_FMTP_LEN 256

/* Jitter */
#define JITTER_VARIANCE_THRESHOLD 400.0
/* IPDV */
#define IPDV_THRESHOLD 1.0
/* Burst and Lost Rate */
#define LOST_BURST_ANALYZE 500
/* Burst */
#define LOST_BURST_CAPTURE 1024

typedef uint8_t base_byte_t;

typedef struct {
    unsigned int value : 31;
} base_uint31_t;

typedef enum {
    BASE_PVT_PRIMARY = 0,
    BASE_PVT_SECONDARY
} base_pvt_class_t;


/*!
\enum base_dtmf_source_t
\brief DTMF sources
<pre>
BASE_DTMF_UNKNOWN             - Unknown source
BASE_DTMF_INBAND_AUDIO        - From audio
BASE_DTMF_RTP                 - From RTP as a telephone event
BASE_DTMF_ENDPOINT            - From endpoint signaling
BASE_DTMF_APP                 - From application
</pre>
*/
typedef enum {
    BASE_DTMF_UNKNOWN,
    BASE_DTMF_INBAND_AUDIO,
    BASE_DTMF_RTP,
    BASE_DTMF_ENDPOINT,
    BASE_DTMF_APP
} base_dtmf_source_t;

typedef enum {
    DIGIT_TARGET_SELF,
    DIGIT_TARGET_PEER,
    DIGIT_TARGET_BOTH
} base_digit_action_target_t;



typedef enum {
    DTMF_FLAG_SKIP_PROCESS = (1 << 0),
    DTMF_FLAG_SENSITIVE = (1 << 1)
} dtmf_flag_t;

typedef struct {
    char digit;
    uint32_t duration;
    int32_t flags;
    base_dtmf_source_t source;
} base_dtmf_t;

typedef enum {
    BASE_CALL_DIRECTION_INBOUND,
    BASE_CALL_DIRECTION_OUTBOUND
} base_call_direction_t;

typedef enum {
    SBF_DIAL_ALEG = (1 << 0),
    SBF_EXEC_ALEG = (1 << 1),
    SBF_DIAL_BLEG = (1 << 2),
    SBF_EXEC_BLEG = (1 << 3),
    SBF_EXEC_OPPOSITE = (1 << 4),
    SBF_EXEC_SAME = (1 << 5),
    SBF_ONCE = (1 << 6),
    SBF_EXEC_INLINE = (1 << 7)
} base_bind_flag_enum_t;
typedef uint32_t base_bind_flag_t;

typedef enum {
    BASE_DTMF_RECV = 0,
    BASE_DTMF_SEND = 1
} base_dtmf_direction_t;

typedef enum {
    SOF_NONE = 0,
    SOF_NOBLOCK = (1 << 0),
    SOF_FORKED_DIAL = (1 << 1),
    SOF_NO_EFFECTIVE_ANI = (1 << 2),
    SOF_NO_EFFECTIVE_ANIII = (1 << 3),
    SOF_NO_EFFECTIVE_CID_NUM = (1 << 4),
    SOF_NO_EFFECTIVE_CID_NAME = (1 << 5),
    SOF_NO_LIMITS = (1 << 6)
} base_originate_flag_enum_t;
typedef uint32_t base_originate_flag_t;

typedef enum {
    SPF_NONE = 0,
    SPF_ODD = (1 << 0),
    SPF_EVEN = (1 << 1),
    SPF_ROBUST_TCP = (1 << 2),
    SPF_ROBUST_UDP = (1 << 3)
} base_port_flag_enum_t;
typedef uint32_t base_port_flag_t;

typedef enum {
    ED_NONE = 0,
    ED_MUX_READ = (1 << 0),
    ED_MUX_WRITE = (1 << 1),
    ED_DTMF = (1 << 2),
    ED_COPY_DISPLAY = (1 << 3),
    ED_BRIDGE_READ = (1 << 4),
    ED_BRIDGE_WRITE = (1 << 5),
    ED_TAP_READ = (1 << 6),
    ED_TAP_WRITE = (1 << 7),
    ED_STEREO = (1 << 8)
} base_eavesdrop_flag_enum_t;
typedef uint32_t base_eavesdrop_flag_t;

typedef enum {
    SCF_NONE = 0,
    SCF_USE_SQL = (1 << 0),
    SCF_NO_NEW_OUTBOUND_SESSIONS = (1 << 1),
    SCF_NO_NEW_INBOUND_SESSIONS = (1 << 2),
    SCF_NO_NEW_SESSIONS = (SCF_NO_NEW_OUTBOUND_SESSIONS | SCF_NO_NEW_INBOUND_SESSIONS),
    SCF_SHUTTING_DOWN = (1 << 3),
    SCF_VG = (1 << 4),
    SCF_RESTART = (1 << 5),
    SCF_SHUTDOWN_REQUESTED = (1 << 6),
    SCF_USE_AUTO_NAT = (1 << 7),
    SCF_EARLY_HANGUP = (1 << 8),
    SCF_CALIBRATE_CLOCK = (1 << 9),
    SCF_USE_HEAVY_TIMING = (1 << 10),
    SCF_USE_CLOCK_RT = (1 << 11),
    SCF_VERBOSE_EVENTS = (1 << 12),
    SCF_USE_WIN32_MONOTONIC = (1 << 13),
    SCF_AUTO_SCHEMAS = (1 << 14),
    SCF_MINIMAL = (1 << 15),
    SCF_USE_NAT_MAPPING = (1 << 16),
    SCF_CLEAR_SQL = (1 << 17),
    SCF_THREADED_SYSTEM_EXEC = (1 << 18),
    SCF_SYNC_CLOCK_REQUESTED = (1 << 19),
    SCF_CORE_NON_SQLITE_DB_REQ = (1 << 20),
    SCF_DEBUG_SQL = (1 << 21),
    SCF_API_EXPANSION = (1 << 22),
    SCF_SESSION_THREAD_POOL = (1 << 23),
    SCF_DIALPLAN_TIMESTAMPS = (1 << 24),
    SCF_CPF_SOFT_PREFIX = (1 << 25),
    SCF_CPF_SOFT_LOOKUP = (1 << 26),
    SCF_EVENT_CHANNEL_ENABLE_HIERARCHY_DELIVERY = (1 << 27),
    SCF_EVENT_CHANNEL_HIERARCHY_DELIVERY_ONCE = (1 << 28),
    SCF_EVENT_CHANNEL_LOG_UNDELIVERABLE_JSON = (1 << 29),
    SCF_LOG_DISABLE = (1 << 30)
} base_core_flag_enum_t;
typedef uint32_t base_core_flag_t;

typedef enum {
    BASE_ENDPOINT_INTERFACE,
    BASE_TIMER_INTERFACE,
    BASE_DIALPLAN_INTERFACE,
    BASE_CODEC_INTERFACE,
    BASE_APPLICATION_INTERFACE,
    BASE_API_INTERFACE,
    BASE_FILE_INTERFACE,
    BASE_SPEECH_INTERFACE,
    BASE_DIRECTORY_INTERFACE,
    BASE_CHAT_INTERFACE,
    BASE_SAY_INTERFACE,
    BASE_ASR_INTERFACE,
    BASE_MANAGEMENT_INTERFACE,
    BASE_LIMIT_INTERFACE,
    BASE_CHAT_APPLICATION_INTERFACE,
    BASE_JSON_API_INTERFACE,
    BASE_DATABASE_INTERFACE,
} base_module_interface_name_t;

/*!
\enum base_database_flag_t
\brief Database flags
<pre>
BASE_DATABASE_FLAG_ROW_SIZE_LIMIT = (1 <<  0) - Indicates that database has got row-size limit for the combined sizes of all columns.
</pre>
*/
typedef enum {
    BASE_DATABASE_FLAG_ROW_SIZE_LIMIT = (1 << 0)
} base_database_flag_enum_t;
typedef uint32_t base_database_flag_t;

typedef enum {
    SUF_NONE = 0,
    SUF_THREAD_RUNNING = (1 << 0),
    SUF_READY = (1 << 1),
    SUF_NATIVE = (1 << 2)
} base_unicast_flag_enum_t;
typedef uint32_t base_unicast_flag_t;

typedef enum {
    BASE_FALSE = 0,
    BASE_TRUE = 1
} base_bool_t;

/* WARNING, Do not forget to update *SAY_METHOD_NAMES[] in src/base_ivr_play_say.c */
typedef enum {
    SSM_NA,
    SSM_PRONOUNCED,
    SSM_ITERATED,
    SSM_COUNTED,
    SSM_PRONOUNCED_YEAR
} base_say_method_t;

/* WARNING, Do not forget to update *SAY_TYPE_NAMES[] in src/base_ivr_say.c */
typedef enum {
    SST_NUMBER,
    SST_ITEMS,
    SST_PERSONS,
    SST_MESSAGES,
    SST_CURRENCY,
    SST_TIME_MEASUREMENT,
    SST_CURRENT_DATE,
    SST_CURRENT_TIME,
    SST_CURRENT_DATE_TIME,
    SST_TELEPHONE_NUMBER,
    SST_TELEPHONE_EXTENSION,
    SST_URL,
    SST_IP_ADDRESS,
    SST_EMAIL_ADDRESS,
    SST_POSTAL_ADDRESS,
    SST_ACCOUNT_NUMBER,
    SST_NAME_SPELLED,
    SST_NAME_PHONETIC,
    SST_SHORT_DATE_TIME
} base_say_type_t;

typedef enum {
    SSG_MASCULINE,
    SSG_FEMININE,
    SSG_NEUTER,
    SSG_UTRUM
} base_say_gender_t;

typedef enum {
    SMA_NONE,
    SMA_GET,
    SMA_SET
} base_management_action_t;

typedef enum {
    SSHF_NONE = 0,
    SSHF_OWN_THREAD = (1 << 0),
    SSHF_FREE_ARG = (1 << 1),
    SSHF_NO_DEL = (1 << 2)
} base_scheduler_flag_enum_t;
typedef uint32_t base_scheduler_flag_t;

typedef enum {
    SMF_NONE = 0,
    SMF_REBRIDGE = (1 << 0),
    SMF_ECHO_ALEG = (1 << 1),
    SMF_ECHO_BLEG = (1 << 2),
    SMF_FORCE = (1 << 3),
    SMF_LOOP = (1 << 4),
    SMF_HOLD_BLEG = (1 << 5),
    SMF_IMMEDIATE = (1 << 6),
    SMF_EXEC_INLINE = (1 << 7),
    SMF_PRIORITY = (1 << 8),
    SMF_REPLYONLY_A = (1 << 9),
    SMF_REPLYONLY_B = (1 << 10)
} base_media_flag_enum_t;
typedef uint32_t base_media_flag_t;

typedef enum {
    BASE_BITPACK_MODE_RFC3551,
    BASE_BITPACK_MODE_AAL2
} base_bitpack_mode_t;

typedef enum {
    BASE_ABC_TYPE_INIT,
    BASE_ABC_TYPE_READ,
    BASE_ABC_TYPE_WRITE,
    BASE_ABC_TYPE_WRITE_REPLACE,
    BASE_ABC_TYPE_READ_REPLACE,
    BASE_ABC_TYPE_READ_PING,
    BASE_ABC_TYPE_TAP_NATIVE_READ,
    BASE_ABC_TYPE_TAP_NATIVE_WRITE,
    BASE_ABC_TYPE_CLOSE,
    BASE_ABC_TYPE_READ_VIDEO_PING,
    BASE_ABC_TYPE_WRITE_VIDEO_PING,
    BASE_ABC_TYPE_STREAM_VIDEO_PING,
    BASE_ABC_TYPE_VIDEO_PATCH,
    BASE_ABC_TYPE_READ_TEXT
} base_abc_type_t;

typedef struct {
    base_byte_t *buf;
    uint32_t buflen;
    base_byte_t *cur;
    uint32_t bytes;
    uint32_t bits_tot;
    base_byte_t bits_cur;
    base_byte_t bits_rem;
    base_byte_t frame_bits;
    base_byte_t shiftby;
    base_byte_t this_byte;
    base_byte_t under;
    base_byte_t over;
    base_bitpack_mode_t mode;
} base_bitpack_t;


struct base_directories {
    char *base_dir;
    char *mod_dir;
    char *conf_dir;
    char *log_dir;
    char *run_dir;
    char *db_dir;
    char *script_dir;
    char *temp_dir;
    char *htdocs_dir;
    char *grammar_dir;
    char *storage_dir;
    char *cache_dir;
    char *recordings_dir;
    char *sounds_dir;
    char *lib_dir;
    char *certs_dir;
    char *fonts_dir;
    char *images_dir;
    char *data_dir;
    char *localstate_dir;
};

typedef struct base_directories base_directories;
BASE_DECLARE_DATA extern base_directories BASE_GLOBAL_dirs;

struct base_filenames {
    char *conf_name;
};

typedef struct base_filenames base_filenames;
BASE_DECLARE_DATA extern base_filenames BASE_GLOBAL_filenames;

#define BASE_MAX_STACKS 32
#define BASE_THREAD_STACKSIZE 240 * 1024
#define BASE_SYSTEM_THREAD_STACKSIZE 8192 * 1024
#define BASE_MAX_INTERVAL 120	/* we only do up to 120ms */
#define BASE_INTERVAL_PAD 10	/* A little extra buffer space to be safe */
#define BASE_MAX_SAMPLE_LEN 48
#define BASE_BYTES_PER_SAMPLE 2	/* slin is 2 bytes per sample */
#define BASE_RECOMMENDED_BUFFER_SIZE 8192
#define BASE_MAX_CODECS 50
#define BASE_MAX_STATE_HANDLERS 30
#define BASE_CORE_QUEUE_LEN 100000
#define BASE_MAX_MANAGEMENT_BUFFER_LEN 1024 * 8

#define BASE_ACCEPTABLE_INTERVAL(_i) (_i && _i <= BASE_MAX_INTERVAL && (_i % 10) == 0)

/* Check if RAND_MAX is a power of 2 minus 1 or in other words all bits set */
#if ((RAND_MAX) & ((RAND_MAX) + 1)) == 0 && (RAND_MAX) != 0
#define BASE_RAND_MAX RAND_MAX
#else
#define BASE_RAND_MAX 0x7fff
#endif

typedef enum {
    BASE_RW_READ,
    BASE_RW_WRITE
} base_rw_t;

typedef enum {
    BASE_CPF_NONE = 0,
    BASE_CPF_SCREEN = (1 << 0),
    BASE_CPF_HIDE_NAME = (1 << 1),
    BASE_CPF_HIDE_NUMBER = (1 << 2),
    BASE_CPF_SOFT_PREFIX = (1 << 3),
    BASE_CPF_SOFT_LOOKUP = (1 << 4)
} base_caller_profile_flag_enum_t;
typedef uint32_t base_caller_profile_flag_t;

typedef enum {
    // flags matching libsndfile
    BASE_AUDIO_COL_STR_TITLE = 0x01,
    BASE_AUDIO_COL_STR_COPYRIGHT = 0x02,
    BASE_AUDIO_COL_STR_SOFTWARE = 0x03,
    BASE_AUDIO_COL_STR_ARTIST = 0x04,
    BASE_AUDIO_COL_STR_COMMENT = 0x05,
    BASE_AUDIO_COL_STR_DATE = 0x06,
    BASE_AUDIO_COL_STR_ALBUM = 0x07,
    BASE_AUDIO_COL_STR_LICENSE = 0x08,
    BASE_AUDIO_COL_STR_TRACKNUMBER = 0x09,
    BASE_AUDIO_COL_STR_GENRE = 0x10,

    // custom flags
    BASE_AUDIO_COL_STR_FILE_SIZE = 0xF0,
    BASE_AUDIO_COL_STR_FILE_TRIMMED = 0xF1,
    BASE_AUDIO_COL_STR_FILE_TRIMMED_MS = 0xF2
} base_audio_col_t;

typedef enum {
    BASE_XML_SECTION_RESULT = 0,
    BASE_XML_SECTION_CONFIG = (1 << 0),
    BASE_XML_SECTION_DIRECTORY = (1 << 1),
    BASE_XML_SECTION_DIALPLAN = (1 << 2),
    BASE_XML_SECTION_LANGUAGES = (1 << 3),
    BASE_XML_SECTION_CHATPLAN = (1 << 4),
    BASE_XML_SECTION_CHANNELS = (1 << 5),
    /* Nothing after this line */
    BASE_XML_SECTION_MAX = (1 << 5)
} base_xml_section_enum_t;
typedef uint32_t base_xml_section_t;

/*!
\enum base_vad_flag_t
\brief RTP Related Flags
<pre>
BASE_VAD_FLAG_TALKING         - Currently Talking
BASE_VAD_FLAG_EVENTS_TALK     - Fire events when talking is detected
BASE_VAD_FLAG_EVENTS_NOTALK   - Fire events when not talking is detected
BASE_VAD_FLAG_CNG				- Send CNG
</pre>
*/
typedef enum {
    BASE_VAD_FLAG_TALKING = (1 << 0),
    BASE_VAD_FLAG_EVENTS_TALK = (1 << 1),
    BASE_VAD_FLAG_EVENTS_NOTALK = (1 << 2),
    BASE_VAD_FLAG_CNG = (1 << 3)
} base_vad_flag_enum_t;
typedef uint32_t base_vad_flag_t;

typedef enum {
    BASE_VAD_STATE_NONE,
    BASE_VAD_STATE_START_TALKING,
    BASE_VAD_STATE_TALKING,
    BASE_VAD_STATE_STOP_TALKING,
    BASE_VAD_STATE_ERROR
} base_vad_state_t;
typedef struct base_vad_s base_vad_t;

typedef struct error_period {
    int64_t start;
    int64_t stop;
    uint32_t flaws;
    uint32_t consecutive_flaws;
    struct error_period *next;
} base_error_period_t;


typedef struct {
    base_size_t raw_bytes;
    base_size_t media_bytes;
    base_size_t packet_count;
    base_size_t period_packet_count;
    base_size_t media_packet_count;
    base_size_t skip_packet_count;
    base_size_t jb_packet_count;
    base_size_t dtmf_packet_count;
    base_size_t cng_packet_count;
    base_size_t flush_packet_count;
    base_size_t largest_jb_size;
    /* Jitter */
    int64_t last_proc_time;
    int64_t jitter_n;
    int64_t jitter_add;
    int64_t jitter_addsq;

    double variance;
    double min_variance;
    double max_variance;
    double std_deviation;

    /* Burst and Packet Loss */
    double lossrate;
    double burstrate;
    double mean_interval;
    int loss[LOST_BURST_CAPTURE];
    int last_loss;
    int recved;
    int last_processed_seq;
    base_size_t flaws;
    base_size_t last_flaw;
    double R;
    double mos;
    struct error_period *error_log;
} base_rtp_numbers_t;

typedef struct {
    uint32_t packet_count;        /* sent packet count */
    uint32_t octet_count;
    uint32_t peer_ssrc;
    uint32_t last_rpt_ts;         /* RTP timestamp at which the last report was generated and sent */
    uint32_t ssrc;                /* identifier of the source */
    uint32_t csrc;                /* contributing source 0-15 32bit each */
    uint32_t last_pkt_tsdiff;     /* Jitter calculation, timestamp difference between the two last received packet */
    double   inter_jitter;        /* Jitter calculation, Interarrival jitter */
    uint32_t last_rpt_ext_seq;    /* Packet loss calculation, extended sequence number at the begining of this RTCP report interval */
    uint16_t last_rpt_cycle;      /* Packet loss calculation, sequence number cycle at the begining of the current RTCP report interval */
    uint16_t period_pkt_count;    /* Packet loss calculation, packet count received during this RTCP report interval */
    uint16_t pkt_count;           /* Packet loss calculation, packet count received during this session */
    uint16_t sent_pkt_count;
    uint32_t rtcp_rtp_count;      /* RTCP report generated count */
    uint32_t high_ext_seq_recv;   /* Packet loss calculation, highest extended sequence number received and processed for stats */
    uint16_t cycle;               /* Packet loss calculation, sequence number cycle of the current RTCP report interval */
    uint32_t bad_seq;             /* Bad SEQ found, used to detect reset on the other side */
    uint16_t base_seq;            /* Packet loss calculation, first sequence number received */
    uint32_t cum_lost;            /* Packet loss calculation, cumulative number of packet lost */
    uint32_t last_recv_lsr_local; /* RTT calculation, When receiving an SR we save our local timestamp in fraction of 65536 seconds */
    uint32_t last_recv_lsr_peer;  /* RTT calculation, When receiving an SR we extract the middle 32bits of the remote NTP timestamp to include it in the next SR LSR */
    uint32_t init;
} base_rtcp_numbers_t;

typedef struct {
    uint16_t nack_count; 
    uint16_t fir_count;
    uint16_t pli_count;
    uint16_t sr_count;
    uint16_t rr_count;
} base_rtcp_video_counters_t;

typedef struct {
    /* counters and stats for the incoming video stream and outgoing RTCP*/
    base_rtcp_video_counters_t video_in;
    /* counters and stats for the outgoing video stream and incoming RTCP*/
    base_rtcp_video_counters_t video_out;
} base_rtcp_video_stats_t;

typedef struct {
    base_rtp_numbers_t inbound;
    base_rtp_numbers_t outbound;
    base_rtcp_numbers_t rtcp;
    uint32_t read_count;
} base_rtp_stats_t;

typedef enum {
    BASE_RTP_FLUSH_ONCE,
    BASE_RTP_FLUSH_STICK,
    BASE_RTP_FLUSH_UNSTICK
} base_rtp_flush_t;

#define BASE_RTP_CNG_PAYLOAD 13

/*!
\enum base_rtp_flag_t
\brief RTP Related Flags
<pre>
BASE_RTP_FLAG_NOBLOCK       - Do not block
BASE_RTP_FLAG_IO            - IO is ready
BASE_RTP_FLAG_USE_TIMER     - Timeout Reads and replace with a CNG Frame
BASE_RTP_FLAG_SECURE        - Secure RTP
BASE_RTP_FLAG_AUTOADJ       - Auto-Adjust the dest based on the source
BASE_RTP_FLAG_RAW_WRITE     - Try to forward packets unscathed
BASE_RTP_FLAG_GOOGLEHACK    - Convert payload from 102 to 97
BASE_RTP_FLAG_VAD           - Enable VAD
BASE_RTP_FLAG_BREAK		  - Stop what you are doing and return BASE_STATUS_BREAK
BASE_RTP_FLAG_DATAWAIT	  - Do not return from reads unless there is data even when non blocking
BASE_RTP_FLAG_BUGGY_2833    - Emulate the bug in cisco equipment to allow interop
BASE_RTP_FLAG_PASS_RFC2833  - Pass 2833 (ignore it)
BASE_RTP_FLAG_AUTO_CNG      - Generate outbound CNG frames when idle
</pre>
*/
typedef enum {
    BASE_RTP_FLAG_NOBLOCK = 0,
    BASE_RTP_FLAG_DTMF_ON,
    BASE_RTP_FLAG_IO,
    BASE_RTP_FLAG_USE_TIMER,
    BASE_RTP_FLAG_RTCP_PASSTHRU,
    BASE_RTP_FLAG_SECURE_SEND,
    BASE_RTP_FLAG_SECURE_RECV,
    BASE_RTP_FLAG_AUTOADJ,
    BASE_RTP_FLAG_RTCP_AUTOADJ,
    BASE_RTP_FLAG_RAW_WRITE,
    BASE_RTP_FLAG_GOOGLEHACK,
    BASE_RTP_FLAG_VAD,
    BASE_RTP_FLAG_BREAK,
    BASE_RTP_FLAG_UDPTL,
    BASE_RTP_FLAG_DATAWAIT,
    BASE_RTP_FLAG_BYTESWAP,
    BASE_RTP_FLAG_PASS_RFC2833,
    BASE_RTP_FLAG_AUTO_CNG,
    BASE_RTP_FLAG_SECURE_SEND_RESET,
    BASE_RTP_FLAG_SECURE_RECV_RESET,
    BASE_RTP_FLAG_PROXY_MEDIA,
    BASE_RTP_FLAG_SHUTDOWN,
    BASE_RTP_FLAG_FLUSH,
    BASE_RTP_FLAG_AUTOFLUSH,
    BASE_RTP_FLAG_STICKY_FLUSH,
    BASE_RTP_FLAG_DEBUG_RTP_READ,
    BASE_RTP_FLAG_DEBUG_RTP_WRITE,
    BASE_RTP_FLAG_ESTIMATORS,
    BASE_RTP_FLAG_ADJ_BITRATE_CAP,
    BASE_RTP_FLAG_VIDEO,
    BASE_RTP_FLAG_ENABLE_RTCP,
    BASE_RTP_FLAG_RTCP_MUX,
    BASE_RTP_FLAG_KILL_JB,
    BASE_RTP_FLAG_VIDEO_BREAK,
    BASE_RTP_FLAG_PAUSE,
    BASE_RTP_FLAG_FIR,
    BASE_RTP_FLAG_PLI,
    BASE_RTP_FLAG_RESET,
    BASE_RTP_FLAG_MUTE,
    BASE_RTP_FLAG_NACK,
    BASE_RTP_FLAG_TMMBR,
    BASE_RTP_FLAG_DETECT_SSRC,
    BASE_RTP_FLAG_TEXT,
    BASE_RTP_FLAG_OLD_FIR,
    BASE_RTP_FLAG_PASSTHRU,
    BASE_RTP_FLAG_SECURE_SEND_MKI,
    BASE_RTP_FLAG_SECURE_RECV_MKI,
    BASE_RTP_FLAG_SRTP_HANGUP_ON_ERROR,
    BASE_RTP_FLAG_AUDIO_FIRE_SEND_RTCP_EVENT,
    BASE_RTP_FLAG_VIDEO_FIRE_SEND_RTCP_EVENT,
    BASE_RTP_FLAG_INVALID
} base_rtp_flag_t;


typedef enum {
    RTP_BUG_NONE = 0,			/* won't be using this one much ;) */

    RTP_BUG_CISCO_SKIP_MARK_BIT_2833 = (1 << 0),
    /* Some Cisco devices get mad when you send the mark bit on new 2833 because it makes
    them flush their jitterbuffer and the dtmf along with it.

    This flag will disable the sending of the mark bit on the first DTMF packet.
    */


    RTP_BUG_SONUS_SEND_INVALID_TIMESTAMP_2833 = (1 << 1),
    /*
    Sonus wrongly expects that, when sending a multi-packet 2833 DTMF event, The sender
    should increment the RTP timestamp in each packet when, in reality, the sender should
    send the same exact timestamp and increment the duration field in the 2833 payload.
    This allows a reconstruction of the duration if any of the packets are lost.

    final_duration - initial_timestamp = total_samples

    However, if the duration value exceeds the space allocated (16 bits), The sender should increment
    the timestamp one unit and reset the duration to 0.

    Always sending a duration of 0 with a new timestamp should be tolerated but is rarely intentional
    and is mistakenly done by many devices.
    The issue is that the Sonus expects everyone to do it this way instead of tolerating either way.
    Sonus will actually ignore every packet with the same timestamp before concluding if it's DTMF.

    This flag will cause each packet to have a new timestamp.
    */


    RTP_BUG_IGNORE_MARK_BIT = (1 << 2),

    /*
    A Huawei SBC has been discovered that sends the mark bit on every single RTP packet.
    Since this causes the RTP stack to flush it's buffers, it horribly messes up the timing on the channel.

    This flag will do nothing when an inbound packet contains the mark bit.

    */


    RTP_BUG_SEND_LINEAR_TIMESTAMPS = (1 << 3),

    /*
    Our friends at Sonus get real mad when the timestamps are not in perfect sequence even during periods of silence.
    With this flag, we will only increment the timestamp when write packets even if they are eons apart.

    */

    RTP_BUG_START_SEQ_AT_ZERO = (1 << 4),

    /*
    Our friends at Sonus also get real mad if the sequence number does not start at 0.
    Typically, we set this to a random starting value for your saftey.
    This is a security risk you take upon yourself when you enable this flag.
    */


    RTP_BUG_NEVER_SEND_MARKER = (1 << 5),

    /*
    Our friends at Sonus are on a roll, They also get easily dumbfounded by marker bits.
    This flag will never send any. Sheesh....
    */

    RTP_BUG_IGNORE_DTMF_DURATION = (1 << 6),

    /*
    Guess Who? ... Yep, Sonus (and who know's who else) likes to interweave DTMF with the audio stream making it take
    2X as long as it should and sending an incorrect duration making the DTMF very delayed.
    This flag will treat every dtmf as if it were 50ms and queue it on recipt of the leading packet rather than at the end.
    */


    RTP_BUG_ACCEPT_ANY_PACKETS = (1 << 7),

    /*
    Oracle's Contact Center Anywhere (CCA) likes to use a single RTP socket to send all its outbound audio.
    This messes up our ability to auto adjust to NATTED RTP and causes us to ignore its audio packets.
    This flag will allow compatibility with this dying product.
    */


    RTP_BUG_GEN_ONE_GEN_ALL = (1 << 8),

    /*
    Some RTP endpoints (and by some we mean *cough* _SONUS_!) do not like it when the timestamps jump forward or backwards in time.
    So say you are generating a file that says "please wait for me to complete your call, or generating ringback"
    Now you place and outbound call and you are bridging.  Well, while you were playing the file, you were generating your own RTP timestamps.
    But, now that you have a remote RTP stream, you'd rather send those timestamps as-is in case they will be fed to a remote jitter buffer......
    Ok, so this causes the audio to completely fade out despite the fact that we send the mark bit which should give them heads up its happening.

    Sigh, This flag will tell FreeBASE that if it ever generates even one RTP packet itself, to continue to generate all of them and ignore the
    actual timestamps in the frames.

    */

    RTP_BUG_CHANGE_SSRC_ON_MARKER = (1 << 9),

    /*
    By default FS will change the SSRC when the marker is set and it detects a timestamp reset.
    If this setting is enabled it will NOT do this (old behaviour).
    */

    RTP_BUG_FLUSH_JB_ON_DTMF = (1 << 10),

    /* FLUSH JITTERBUFFER When getting RFC2833 to reduce bleed through */

    RTP_BUG_ACCEPT_ANY_PAYLOAD = (1 << 11),

    /*
    Make FS accept any payload type instead of dropping and returning CNG frame. Workaround while FS only supports a single payload per rtp session.
    This can be used by endpoint modules to detect payload changes and act appropriately (ex: sofia could send a reINVITE with single codec).
    This should probably be a flag, but flag enum is already full!
    */


    RTP_BUG_ALWAYS_AUTO_ADJUST = (1 << 12)

    /*
    Leave the auto-adjust behavior enableed permenantly rather than only at appropriate times.  (IMPLICITLY sets RTP_BUG_ACCEPT_ANY_PACKETS)

    */

} base_rtp_bug_flag_t;

#ifdef _MSC_VER
#pragma pack(push, r1, 1)
#endif

#if BASE_BYTE_ORDER == __BIG_ENDIAN
typedef struct {
    unsigned version:2;			/* protocol version       */
    unsigned p:1;				/* padding flag           */
    unsigned x:1;				/* header extension flag  */
    unsigned cc:4;				/* CSRC count             */
    unsigned m:1;				/* marker bit             */
    unsigned pt:7;				/* payload type           */
    unsigned seq:16;			/* sequence number        */
    unsigned ts:32;				/* timestamp              */
    unsigned ssrc:32;			/* synchronization source */
} base_rtp_hdr_t;

typedef struct {
    unsigned length:16;			/* length                 */
    unsigned profile:16;		/* defined by profile     */
} base_rtp_hdr_ext_t;

#else /*  BIG_ENDIAN */

typedef struct {
    unsigned cc:4;				/* CSRC count             */
    unsigned x:1;				/* header extension flag  */
    unsigned p:1;				/* padding flag           */
    unsigned version:2;			/* protocol version       */
    unsigned pt:7;				/* payload type           */
    unsigned m:1;				/* marker bit             */
    unsigned seq:16;			/* sequence number        */
    unsigned ts:32;				/* timestamp              */
    unsigned ssrc:32;			/* synchronization source */
} base_rtp_hdr_t;

typedef struct {
    unsigned profile:16;		/* defined by profile     */
    unsigned length:16;			/* length                 */
} base_rtp_hdr_ext_t;

#endif

#ifdef _MSC_VER
#pragma pack(pop, r1)
#endif

#ifdef _MSC_VER
#pragma pack(push, r1, 1)
#endif

#if BASE_BYTE_ORDER == __BIG_ENDIAN
typedef struct base_rtcp_hdr_s {
    unsigned version:2;			/* protocol version                  */
    unsigned p:1;				/* padding flag                      */
    unsigned count:5;			/* number of reception report blocks */
    unsigned type:8;			/* packet type                       */
    unsigned length:16;			/* length in 32-bit words - 1        */
} base_rtcp_hdr_t;

#else /*  BIG_ENDIAN */

typedef struct base_rtcp_hdr_s {
    unsigned count:5;			/* number of reception report blocks */
    unsigned p:1;				/* padding flag                      */
    unsigned version:2;			/* protocol version                  */
    unsigned type:8;			/* packet type                       */
    unsigned length:16;			/* length in 32-bit words - 1        */
} base_rtcp_hdr_t;

#endif

#ifdef _MSC_VER
#pragma pack(pop, r1)
#endif

typedef struct audio_buffer_header_s {
    uint32_t ts;
    uint32_t len;
} audio_buffer_header_t;


/*!
\enum base_priority_t
\brief Priority Indication
<pre>
BASE_PRIORITY_NORMAL  - Normal Priority
BASE_PRIORITY_LOW     - Low Priority
BASE_PRIORITY_HIGH    - High Priority
</pre>
*/
typedef enum {
    BASE_PRIORITY_NORMAL,
    BASE_PRIORITY_LOW,
    BASE_PRIORITY_HIGH
} base_priority_t;

/*!
\enum base_ivr_option_t
\brief Possible options related to ivr functions
<pre>
BASE_IVR_OPTION_NONE  - nothing whatsoever
BASE_IVR_OPTION_ASYNC - Asynchronous (do things in the background when applicable)
BASE_IVR_OPTION_FILE  - string argument implies a filename
</pre>
*/
typedef enum {
    BASE_IVR_OPTION_NONE = 0,
    BASE_IVR_OPTION_ASYNC = (1 << 0),
    BASE_IVR_OPTION_FILE = (1 << 1)
} base_ivr_option_enum_t;
typedef uint32_t base_ivr_option_t;

/*!
\enum base_core_session_message_types_t
\brief Possible types of messages for inter-session communication
<pre>
BASE_MESSAGE_REDIRECT_AUDIO     - Indication to redirect audio to another location if possible
BASE_MESSAGE_TRANSMIT_TEXT      - A text message
BASE_MESSAGE_INDICATE_ANSWER    - indicate answer
BASE_MESSAGE_INDICATE_PROGRESS  - indicate progress
BASE_MESSAGE_INDICATE_BRIDGE    - indicate a bridge starting
BASE_MESSAGE_INDICATE_UNBRIDGE  - indicate a bridge ending
BASE_MESSAGE_INDICATE_TRANSFER  - indicate a transfer is taking place
BASE_MESSAGE_INDICATE_MEDIA	  - indicate media is required
BASE_MESSAGE_INDICATE_NOMEDIA	  - indicate no-media is required
BASE_MESSAGE_INDICATE_HOLD      - indicate hold
BASE_MESSAGE_INDICATE_UNHOLD    - indicate unhold
BASE_MESSAGE_INDICATE_REDIRECT  - indicate redirect
BASE_MESSAGE_INDICATE_RESPOND    - indicate reject
BASE_MESSAGE_INDICATE_BROADCAST - indicate media broadcast
BASE_MESSAGE_INDICATE_MEDIA_REDIRECT - indicate media broadcast
BASE_MESSAGE_INDICATE_DEFLECT - indicate deflect
BASE_MESSAGE_INDICATE_VIDEO_REFRESH_REQ - indicate video refresh request
</pre>
*/
typedef enum {
    BASE_MESSAGE_REDIRECT_AUDIO,
    BASE_MESSAGE_TRANSMIT_TEXT,
    BASE_MESSAGE_INDICATE_ANSWER,
    BASE_MESSAGE_INDICATE_ACKNOWLEDGE_CALL,
    BASE_MESSAGE_INDICATE_PROGRESS,
    BASE_MESSAGE_INDICATE_BRIDGE,
    BASE_MESSAGE_INDICATE_UNBRIDGE,
    BASE_MESSAGE_INDICATE_TRANSFER,
    BASE_MESSAGE_INDICATE_RINGING,
    BASE_MESSAGE_INDICATE_ALERTING,
    BASE_MESSAGE_INDICATE_MEDIA,
    BASE_MESSAGE_INDICATE_3P_MEDIA,
    BASE_MESSAGE_INDICATE_NOMEDIA,
    BASE_MESSAGE_INDICATE_3P_NOMEDIA,
    BASE_MESSAGE_INDICATE_HOLD,
    BASE_MESSAGE_INDICATE_UNHOLD,
    BASE_MESSAGE_INDICATE_REDIRECT,
    BASE_MESSAGE_INDICATE_RESPOND,
    BASE_MESSAGE_INDICATE_BROADCAST,
    BASE_MESSAGE_INDICATE_MEDIA_REDIRECT,
    BASE_MESSAGE_INDICATE_DEFLECT,
    BASE_MESSAGE_INDICATE_VIDEO_REFRESH_REQ,
    BASE_MESSAGE_INDICATE_DISPLAY,
    BASE_MESSAGE_INDICATE_MEDIA_PARAMS,
    BASE_MESSAGE_INDICATE_TRANSCODING_NECESSARY,
    BASE_MESSAGE_INDICATE_AUDIO_SYNC,
    BASE_MESSAGE_INDICATE_VIDEO_SYNC,
    BASE_MESSAGE_INDICATE_REQUEST_IMAGE_MEDIA,
    BASE_MESSAGE_INDICATE_UUID_CHANGE,
    BASE_MESSAGE_INDICATE_SIMPLIFY,
    BASE_MESSAGE_INDICATE_DEBUG_MEDIA,
    BASE_MESSAGE_INDICATE_PROXY_MEDIA,
    BASE_MESSAGE_INDICATE_APPLICATION_EXEC,
    BASE_MESSAGE_INDICATE_APPLICATION_EXEC_COMPLETE,
    BASE_MESSAGE_INDICATE_PHONE_EVENT,
    BASE_MESSAGE_INDICATE_T38_DESCRIPTION,
    BASE_MESSAGE_INDICATE_UDPTL_MODE,
    BASE_MESSAGE_INDICATE_CLEAR_PROGRESS,
    BASE_MESSAGE_INDICATE_JITTER_BUFFER,
    BASE_MESSAGE_INDICATE_RECOVERY_REFRESH,
    BASE_MESSAGE_INDICATE_SIGNAL_DATA,
    BASE_MESSAGE_INDICATE_MESSAGE,
    BASE_MESSAGE_INDICATE_INFO,
    BASE_MESSAGE_INDICATE_AUDIO_DATA,
    BASE_MESSAGE_INDICATE_BLIND_TRANSFER_RESPONSE,
    BASE_MESSAGE_INDICATE_STUN_ERROR,
    BASE_MESSAGE_INDICATE_MEDIA_RENEG,
    BASE_MESSAGE_INDICATE_KEEPALIVE,
    BASE_MESSAGE_INDICATE_HARD_MUTE,
    BASE_MESSAGE_INDICATE_BITRATE_REQ,
    BASE_MESSAGE_INDICATE_BITRATE_ACK,
    BASE_MESSAGE_INDICATE_CODEC_DEBUG_REQ,
    BASE_MESSAGE_INDICATE_CODEC_SPECIFIC_REQ,
    BASE_MESSAGE_REFER_EVENT,
    BASE_MESSAGE_ANSWER_EVENT,
    BASE_MESSAGE_PROGRESS_EVENT,
    BASE_MESSAGE_RING_EVENT,
    BASE_MESSAGE_RESAMPLE_EVENT,
    BASE_MESSAGE_HEARTBEAT_EVENT,
    BASE_MESSAGE_INDICATE_SESSION_ID,
    BASE_MESSAGE_INDICATE_PROMPT,
    BASE_MESSAGE_INVALID
} base_core_session_message_types_t;

typedef struct {
    uint16_t T38FaxVersion;
    uint32_t T38MaxBitRate;
    base_bool_t T38FaxFillBitRemoval;
    base_bool_t T38FaxTranscodingMMR;
    base_bool_t T38FaxTranscodingJBIG;
    const char *T38FaxRateManagement;
    uint32_t T38FaxMaxBuffer;
    uint32_t T38FaxMaxDatagram;
    const char *T38FaxUdpEC;
    const char *T38VendorInfo;
    const char *remote_ip;
    uint16_t remote_port;
    const char *local_ip;
    uint16_t local_port;
    const char *sdp_o_line;
} base_t38_options_t;

/*!
\enum base_stack_t
\brief Expression of how to stack a list
<pre>
BASE_STACK_BOTTOM - Stack on the bottom
BASE_STACK_TOP	- Stack on the top
</pre>
*/
typedef enum {
    BASE_STACK_BOTTOM = (1 << 0),
    BASE_STACK_TOP = (1 << 1),
    BASE_STACK_UNSHIFT = (1 << 2),
    BASE_STACK_PUSH = (1 << 3)
} base_stack_t;

/*!
\enum base_status_t
\brief Common return values
<pre>
BASE_STATUS_SUCCESS	- General Success (common return value for most functions)
BASE_STATUS_FALSE		- General Falsehood
BASE_STATUS_TIMEOUT	- A Timeout has occured
BASE_STATUS_RESTART	- An indication to restart the previous operation
BASE_STATUS_TERM		- An indication to terminate
BASE_STATUS_NOTIMPL	- An indication that requested resource is not impelemented
BASE_STATUS_MEMERR	- General memory error
BASE_STATUS_NOOP		- NOTHING
BASE_STATUS_RESAMPLE	- An indication that a resample has occured
BASE_STATUS_GENERR	- A general Error
BASE_STATUS_INUSE		- An indication that requested resource is in use
BASE_STATUS_BREAK     - A non-fatal break of an operation
BASE_STATUS_SOCKERR   - A socket error
BASE_STATUS_MORE_DATA - Need More Data
BASE_STATUS_NOTFOUND  - Not Found
BASE_STATUS_UNLOAD    - Unload
BASE_STATUS_NOUNLOAD  - Never Unload
</pre>
*/
typedef enum {
    BASE_STATUS_SUCCESS,
    BASE_STATUS_FALSE,
    BASE_STATUS_TIMEOUT,
    BASE_STATUS_RESTART,
    BASE_STATUS_INTR,
    BASE_STATUS_NOTIMPL,
    BASE_STATUS_MEMERR,
    BASE_STATUS_NOOP,
    BASE_STATUS_RESAMPLE,
    BASE_STATUS_GENERR,
    BASE_STATUS_INUSE,
    BASE_STATUS_BREAK,
    BASE_STATUS_SOCKERR,
    BASE_STATUS_MORE_DATA,
    BASE_STATUS_NOTFOUND,
    BASE_STATUS_UNLOAD,
    BASE_STATUS_NOUNLOAD,
    BASE_STATUS_IGNORE,
    BASE_STATUS_TOO_SMALL,
    BASE_STATUS_FOUND,
    BASE_STATUS_CONTINUE,
    BASE_STATUS_TERM,
    BASE_STATUS_NOT_INITALIZED,
    BASE_STATUS_TOO_LATE,
    BASE_STATUS_XBREAK = 35,
    BASE_STATUS_WINBREAK = 730035
} base_status_t;



/*!
\enum base_log_level_t
\brief Log Level Enumeration
<pre>
BASE_LOG_DEBUG            - Debug
BASE_LOG_INFO             - Info
BASE_LOG_NOTICE           - Notice
BASE_LOG_WARNING          - Warning
BASE_LOG_ERROR            - Error
BASE_LOG_CRIT             - Critical
BASE_LOG_ALERT            - Alert
BASE_LOG_CONSOLE          - Console
</pre>
*/
typedef enum {
    BASE_LOG_DEBUG10 = 110,
    BASE_LOG_DEBUG9 = 109,
    BASE_LOG_DEBUG8 = 108,
    BASE_LOG_DEBUG7 = 107,
    BASE_LOG_DEBUG6 = 106,
    BASE_LOG_DEBUG5 = 105,
    BASE_LOG_DEBUG4 = 104,
    BASE_LOG_DEBUG3 = 103,
    BASE_LOG_DEBUG2 = 102,
    BASE_LOG_DEBUG1 = 101,
    BASE_LOG_DEBUG = 7,
    BASE_LOG_INFO = 6,
    BASE_LOG_NOTICE = 5,
    BASE_LOG_WARNING = 4,
    BASE_LOG_ERROR = 3,
    BASE_LOG_CRIT = 2,
    BASE_LOG_ALERT = 1,
    BASE_LOG_CONSOLE = 0,
    BASE_LOG_DISABLE = -1,
    BASE_LOG_INVALID = 64,
    BASE_LOG_UNINIT = 1000
} base_log_level_t;


/*!
\enum base_text_channel_t
\brief A target to write log/debug info to
<pre>
BASE_CHANNEL_ID_LOG			- Write to the currently defined log
BASE_CHANNEL_ID_LOG_CLEAN		- Write to the currently defined log with no extra file/line/date information
BASE_CHANNEL_ID_EVENT			- Write to the event engine as a LOG event
</pre>
*/
typedef enum {
    BASE_CHANNEL_ID_LOG,
    BASE_CHANNEL_ID_LOG_CLEAN,
    BASE_CHANNEL_ID_EVENT,
    BASE_CHANNEL_ID_SESSION
} base_text_channel_t;

typedef enum {
    SCSMF_DYNAMIC = (1 << 0),
    SCSMF_FREE_STRING_REPLY = (1 << 1),
    SCSMF_FREE_POINTER_REPLY = (1 << 2)
} base_core_session_message_flag_enum_t;
typedef uint32_t base_core_session_message_flag_t;

typedef struct base_core_session base_core_session_t;
static inline base_core_session_t *base_core_session_type_check(base_core_session_t *session) { return session; }
static inline const char *base_const_char_type_check(const char *str) { return str; }

#define BASE_CHANNEL_LOG BASE_CHANNEL_ID_LOG, __FILE__, __BASE_FUNC__, __LINE__, NULL
#define BASE_CHANNEL_LOG_CLEAN BASE_CHANNEL_ID_LOG_CLEAN, __FILE__, __BASE_FUNC__, __LINE__, NULL
#define BASE_CHANNEL_SESSION_LOG_CLEAN(x) BASE_CHANNEL_ID_LOG_CLEAN, __FILE__, __BASE_FUNC__, __LINE__, base_core_session_get_uuid((x))
#define BASE_CHANNEL_EVENT BASE_CHANNEL_ID_EVENT, __FILE__, __BASE_FUNC__, __LINE__, NULL
#define BASE_CHANNEL_SESSION_LOG(x) BASE_CHANNEL_ID_SESSION, __FILE__, __BASE_FUNC__, __LINE__, (const char*)base_core_session_type_check(x)
#define BASE_CHANNEL_CHANNEL_LOG(x) BASE_CHANNEL_ID_SESSION, __FILE__, __BASE_FUNC__, __LINE__, (const char*)base_channel_get_session(x)
#define BASE_CHANNEL_UUID_LOG(x) BASE_CHANNEL_ID_LOG, __FILE__, __BASE_FUNC__, __LINE__, base_const_char_type_check(x)

typedef enum {
    CCS_DOWN,
    CCS_DIALING,
    CCS_RINGING,
    CCS_EARLY,
    CCS_ACTIVE,
    CCS_HELD,
    CCS_RING_WAIT,
    CCS_HANGUP,
    CCS_UNHELD
} base_channel_callstate_t;

typedef enum {
    SDS_DOWN,
    SDS_RINGING,
    SDS_ACTIVE,
    SDS_ACTIVE_MULTI,
    SDS_HELD,
    SDS_UNHELD,
    SDS_HANGUP
} base_device_state_t;


/*!
\enum base_channel_state_t
\brief Channel States (these are the defaults, CS_SOFT_EXECUTE, CS_EXCHANGE_MEDIA, and CS_CONSUME_MEDIA are often overridden by specific apps)
<pre>
CS_NEW       - Channel is newly created.
CS_INIT      - Channel has been initialized.
CS_ROUTING   - Channel is looking for an extension to execute.
CS_SOFT_EXECUTE  - Channel is ready to execute from 3rd party control.
CS_EXECUTE   - Channel is executing it's dialplan.
CS_EXCHANGE_MEDIA  - Channel is exchanging media with another channel.
CS_PARK      - Channel is accepting media awaiting commands.
CS_CONSUME_MEDIA		 - Channel is consuming all media and dropping it.
CS_HIBERNATE - Channel is in a sleep state.
CS_RESET 	 - Channel is in a reset state.
CS_HANGUP    - Channel is flagged for hangup and ready to end.
CS_REPORTING - Channel is ready to collect call detail.
CS_DESTROY      - Channel is ready to be destroyed and out of the state machine.
</pre>
*/
typedef enum {
    CS_NEW,
    CS_INIT,
    CS_ROUTING,
    CS_SOFT_EXECUTE,
    CS_EXECUTE,
    CS_EXCHANGE_MEDIA,
    CS_PARK,
    CS_CONSUME_MEDIA,
    CS_HIBERNATE,
    CS_RESET,
    CS_HANGUP,
    CS_REPORTING,
    CS_DESTROY,
    CS_NONE
} base_channel_state_t;

typedef enum {
    BASE_RING_READY_NONE,
    BASE_RING_READY_RINGING,
    BASE_RING_READY_QUEUED
}  base_ring_ready_t;


/*!
\enum base_channel_flag_t
\brief Channel Flags

<pre>
CF_ANSWERED			- Channel is answered
CF_OUTBOUND			- Channel is an outbound channel
CF_EARLY_MEDIA		- Channel is ready for audio before answer
CF_ORIGINATOR		- Channel is an originator
CF_TRANSFER			- Channel is being transfered
CF_ACCEPT_CNG		- Channel will accept CNG frames
CF_REDIRECT 		- Channel is being redirected
CF_BRIDGED			- Channel in a bridge
CF_HOLD				- Channel is on hold
CF_HOLD_BLEG        - B leg is on hold
CF_SERVICE			- Channel has a service thread
CF_TAGGED			- Channel is tagged
CF_WINNER			- Channel is the winner
CF_REUSE_CALLER_PROFILE - Channel reuse caller profile 
CF_CONTROLLED		- Channel is under control
CF_PROXY_MODE		- Channel has no media
CF_SUSPEND			- Suspend i/o
CF_EVENT_PARSE		- Suspend control events
CF_GEN_RINGBACK		- Channel is generating it's own ringback
CF_RING_READY		- Channel is ready to send ringback
CF_BREAK			- Channel should stop what it's doing
CF_BROADCAST		- Channel is broadcasting
CF_UNICAST			- Channel has a unicast connection
CF_VIDEO			- Channel has video
CF_EVENT_LOCK		- Don't parse events
CF_RESET			- Tell extension parser to reset
CF_ORIGINATING		- Channel is originating
CF_STOP_BROADCAST	- Signal to stop broadcast

CF_AUDIO_PAUSE      - Audio is not ready to read/write
CF_VIDEO_PAUSE      - Video is not ready to read/write

CF_MEDIA_SET        - Session has read codec assigned

</pre>
*/

typedef enum {
    CC_MEDIA_ACK = 1,
    CC_BYPASS_MEDIA,
    CC_PROXY_MEDIA,
    CC_JITTERBUFFER,
    CC_FS_RTP,
    CC_QUEUEABLE_DTMF_DELAY,
    CC_IO_OVERRIDE,
    CC_RTP_RTT,
    CC_MSRP,
    CC_MUTE_VIA_MEDIA_STREAM,
    /* WARNING: DO NOT ADD ANY FLAGS BELOW THIS LINE */
    CC_FLAG_MAX
} base_channel_cap_t;

typedef enum {
    CF_ANSWERED = 1,
    CF_OUTBOUND,
    CF_EARLY_MEDIA,
    CF_BRIDGE_ORIGINATOR,
    CF_UUID_BRIDGE_ORIGINATOR,
    CF_TRANSFER,
    CF_ACCEPT_CNG,
    CF_REDIRECT,
    CF_BRIDGED,
    CF_HOLD,
    CF_HOLD_BLEG,
    CF_SERVICE,
    CF_TAGGED,
    CF_WINNER,
    CF_REUSE_CALLER_PROFILE,
    CF_CONTROLLED,
    CF_PROXY_MODE,
    CF_PROXY_OFF,
    CF_SUSPEND,
    CF_EVENT_PARSE,
    CF_GEN_RINGBACK,
    CF_RING_READY,
    CF_BREAK,
    CF_BROADCAST,
    CF_UNICAST,
    CF_VIDEO,
    CF_EVENT_LOCK,
    CF_EVENT_LOCK_PRI,
    CF_RESET,
    CF_ORIGINATING,
    CF_STOP_BROADCAST,
    CF_PROXY_MEDIA,
    CF_INNER_BRIDGE,
    CF_REQ_MEDIA,
    CF_VERBOSE_EVENTS,
    CF_PAUSE_BUGS,
    CF_DIVERT_EVENTS,
    CF_BLOCK_STATE,
    CF_FS_RTP,
    CF_REPORTING,
    CF_PARK,
    CF_TIMESTAMP_SET,
    CF_ORIGINATOR,
    CF_XFER_ZOMBIE,
    CF_MEDIA_ACK,
    CF_THREAD_SLEEPING,
    CF_DISABLE_RINGBACK,
    CF_NOT_READY,
    CF_SIGNAL_BRIDGE_TTL,
    CF_MEDIA_BRIDGE_TTL,
    CF_BYPASS_MEDIA_AFTER_BRIDGE,
    CF_LEG_HOLDING,
    CF_BROADCAST_DROP_MEDIA,
    CF_EARLY_HANGUP,
    CF_MEDIA_SET,
    CF_CONSUME_ON_ORIGINATE,
    CF_PASSTHRU_PTIME_MISMATCH,
    CF_BRIDGE_NOWRITE,
    CF_RECOVERED,
    CF_JITTERBUFFER,
    CF_JITTERBUFFER_PLC,
    CF_DIALPLAN,
    CF_BLEG,
    CF_BLOCK_BROADCAST_UNTIL_MEDIA,
    CF_CNG_PLC,
    CF_ATTENDED_TRANSFER,
    CF_LAZY_ATTENDED_TRANSFER,
    CF_SIGNAL_DATA,
    CF_SIMPLIFY,
    CF_ZOMBIE_EXEC,
    CF_INTERCEPT,
    CF_INTERCEPTED,
    CF_VIDEO_REFRESH_REQ,
    CF_MANUAL_VID_REFRESH,
    CF_MANUAL_MEDIA_PARAMS,
    CF_SERVICE_AUDIO,
    CF_SERVICE_VIDEO,
    CF_CHANNEL_SWAP,
    CF_DEVICE_LEG,
    CF_FINAL_DEVICE_LEG,
    CF_PICKUP,
    CF_CONFIRM_BLIND_TRANSFER,
    CF_NO_PRESENCE,
    CF_CONFERENCE,
    CF_CONFERENCE_ADV,
    CF_RECOVERING,
    CF_RECOVERING_BRIDGE,
    CF_TRACKED,
    CF_TRACKABLE,
    CF_NO_CDR,
    CF_EARLY_OK,
    CF_MEDIA_TRANS,
    CF_HOLD_ON_BRIDGE,
    CF_SECURE,
    CF_LIBERAL_DTMF,
    CF_SLA_BARGE,
    CF_SLA_BARGING,
    CF_PROTO_HOLD, //TFLAG_SIP_HOLD
    CF_HOLD_LOCK,
    CF_VIDEO_POSSIBLE,//TFLAG_VIDEO
    CF_NOTIMER_DURING_BRIDGE,
    CF_PASS_RFC2833,
    CF_T38_PASSTHRU,
    CF_DROP_DTMF,
    CF_REINVITE,
    CF_NOSDP_REINVITE,
    CF_AUTOFLUSH_DURING_BRIDGE,
    CF_RTP_NOTIMER_DURING_BRIDGE,
    CF_AVPF,
    CF_AVPF_MOZ,
    CF_ICE,
    CF_DTLS,
    CF_VERBOSE_SDP,
    CF_DTLS_OK,
    CF_3PCC,
    CF_VIDEO_PASSIVE,
    CF_NOVIDEO,
    CF_VIDEO_BITRATE_UNMANAGABLE,
    CF_VIDEO_ECHO,
    CF_VIDEO_BLANK,
    CF_VIDEO_WRITING,
    CF_SLA_INTERCEPT,
    CF_VIDEO_BREAK,
    CF_AUDIO_PAUSE_READ,
    CF_AUDIO_PAUSE_WRITE,
    CF_VIDEO_PAUSE_READ,
    CF_VIDEO_PAUSE_WRITE,
    CF_BYPASS_MEDIA_AFTER_HOLD,
    CF_HANGUP_HELD,
    CF_CONFERENCE_RESET_MEDIA,
    CF_VIDEO_DECODED_READ,
    CF_VIDEO_DEBUG_READ,
    CF_VIDEO_DEBUG_WRITE,
    CF_NO_RECOVER,
    CF_VIDEO_READY,
    CF_VIDEO_MIRROR_INPUT,
    CF_VIDEO_READ_FILE_ATTACHED,
    CF_VIDEO_WRITE_FILE_ATTACHED,
    CF_3P_MEDIA_REQUESTED,
    CF_3P_NOMEDIA_REQUESTED,
    CF_3P_NOMEDIA_REQUESTED_BLEG,
    CF_IMAGE_SDP,
    CF_VIDEO_SDP_RECVD,
    CF_TEXT_SDP_RECVD,
    CF_HAS_TEXT,
    CF_TEXT_POSSIBLE,
    CF_TEXT_PASSIVE,
    CF_TEXT_ECHO,
    CF_TEXT_ACTIVE,
    CF_TEXT_IDLE,
    CF_TEXT_LINE_BASED,
    CF_QUEUE_TEXT_EVENTS,
    CF_FIRE_TEXT_EVENTS,
    CF_MSRP,
    CF_MSRPS,
    CF_WANT_MSRP,
    CF_WANT_MSRPS,
    CF_RTT,
    CF_WANT_RTT,
    CF_AUDIO,
    CF_AWAITING_STREAM_CHANGE,
    CF_PROCESSING_STREAM_CHANGE,
    CF_STREAM_CHANGED,
    CF_ARRANGED_BRIDGE,
    CF_STATE_REPEAT,
    CF_WANT_DTLSv1_2,
    CF_RFC7329_COMPAT,
    CF_REATTACHED,
    CF_VIDEO_READ_TAPPED,
    CF_VIDEO_WRITE_TAPPED,
    CF_DEVICES_CHANGED,
    /* WARNING: DO NOT ADD ANY FLAGS BELOW THIS LINE */
    /* IF YOU ADD NEW ONES CHECK IF THEY SHOULD PERSIST OR ZERO THEM IN base_core_session.c base_core_session_request_xml() */
    CF_FLAG_MAX
} base_channel_flag_t;

typedef struct base_vid_params_s {
    uint32_t width;
    uint32_t height;
    uint32_t fps;
    uint32_t d_width;
    uint32_t d_height;
} base_vid_params_t;

typedef struct base_fps_s {
    float fps;
    int ms;
    int samples;
} base_fps_t;


typedef enum {
    CF_APP_TAGGED = (1 << 0),
    CF_APP_T38 = (1 << 1),
    CF_APP_T38_REQ = (1 << 2),
    CF_APP_T38_FAIL = (1 << 3),
    CF_APP_T38_NEGOTIATED = (1 << 4),
    CF_APP_T38_POSSIBLE = (1 << 5)
} base_channel_app_flag_t;


/*!
\enum base_frame_flag_t
\brief Frame Flags

<pre>
SFF_CNG        = (1 <<  0) - Frame represents comfort noise
SFF_RAW_RTP    = (1 <<  1) - Frame has raw rtp accessible
SFF_RTP_HEADER = (1 <<  2) - Get the rtp header from the frame header
SFF_PLC        = (1 <<  3) - Frame has generated PLC data
SFF_RFC2833    = (1 <<  4) - Frame has rfc2833 dtmf data
SFF_DYNAMIC    = (1 <<  5) - Frame is dynamic and should be freed
SFF_MARKER     = (1 << 11) - Frame flag has Marker set, only set by encoder
SFF_WAIT_KEY_FRAME = (1 << 12) - Need a key from before could decode, or force generate a key frame on encode
</pre>
*/
typedef enum {
    SFF_NONE = 0,
    SFF_CNG = (1 << 0),
    SFF_RAW_RTP = (1 << 1),
    SFF_RTP_HEADER = (1 << 2),
    SFF_PLC = (1 << 3),
    SFF_RFC2833 = (1 << 4),
    SFF_PROXY_PACKET = (1 << 5),
    SFF_DYNAMIC = (1 << 6),
    SFF_UDPTL_PACKET = (1 << 7),
    SFF_NOT_AUDIO = (1 << 8),
    SFF_RTCP = (1 << 9),
    SFF_MARKER = (1 << 10),
    SFF_WAIT_KEY_FRAME = (1 << 11),
    SFF_RAW_RTP_PARSE_FRAME = (1 << 12),
    SFF_PICTURE_RESET = (1 << 13),
    SFF_SAME_IMAGE = (1 << 14),
    SFF_USE_VIDEO_TIMESTAMP = (1 << 15),
    SFF_ENCODED = (1 << 16),
    SFF_TEXT_LINE_BREAK = (1 << 17),
    SFF_IS_KEYFRAME = (1 << 18),
    SFF_EXTERNAL = (1 << 19)
} base_frame_flag_enum_t;
typedef uint32_t base_frame_flag_t;


typedef enum {
    SAF_NONE = 0,
    SAF_SUPPORT_NOMEDIA = (1 << 0),
    SAF_ROUTING_EXEC = (1 << 1),
    SAF_MEDIA_TAP = (1 << 2),
    SAF_ZOMBIE_EXEC = (1 << 3),
    SAF_NO_LOOPBACK = (1 << 4),
    SAF_SUPPORT_TEXT_ONLY = (1 << 5)
} base_application_flag_enum_t;
typedef uint32_t base_application_flag_t;

typedef enum {
    SCAF_NONE = 0
} base_chat_application_flag_enum_t;
typedef uint32_t base_chat_application_flag_t;


/*!
\enum base_signal_t
\brief Signals to send to channels
<pre>
BASE_SIG_KILL - Kill the channel
BASE_SIG_XFER - Stop the current io but leave it viable
</pre>
*/

typedef enum {
    BASE_SIG_NONE,
    BASE_SIG_KILL,
    BASE_SIG_XFER,
    BASE_SIG_BREAK
} base_signal_t;

/*!
\enum base_codec_flag_t
\brief Codec related flags
<pre>
BASE_CODEC_FLAG_ENCODE =			(1 <<  0) - Codec can encode
BASE_CODEC_FLAG_DECODE =			(1 <<  1) - Codec can decode
BASE_CODEC_FLAG_SILENCE_START =	(1 <<  2) - Start period of silence
BASE_CODEC_FLAG_SILENCE_STOP =	(1 <<  3) - End period of silence
BASE_CODEC_FLAG_SILENCE =			(1 <<  4) - Silence
BASE_CODEC_FLAG_FREE_POOL =		(1 <<  5) - Free codec's pool on destruction
BASE_CODEC_FLAG_AAL2 =			(1 <<  6) - USE AAL2 Bitpacking
BASE_CODEC_FLAG_PASSTHROUGH =		(1 <<  7) - Passthrough only
</pre>
*/
typedef enum {
    BASE_CODEC_FLAG_ENCODE = (1 << 0),
    BASE_CODEC_FLAG_DECODE = (1 << 1),
    BASE_CODEC_FLAG_SILENCE_START = (1 << 2),
    BASE_CODEC_FLAG_SILENCE_STOP = (1 << 3),
    BASE_CODEC_FLAG_SILENCE = (1 << 4),
    BASE_CODEC_FLAG_FREE_POOL = (1 << 5),
    BASE_CODEC_FLAG_AAL2 = (1 << 6),
    BASE_CODEC_FLAG_PASSTHROUGH = (1 << 7),
    BASE_CODEC_FLAG_READY = (1 << 8),
    BASE_CODEC_FLAG_HAS_ADJ_BITRATE = (1 << 14),
    BASE_CODEC_FLAG_HAS_PLC = (1 << 15),
    BASE_CODEC_FLAG_VIDEO_PATCHING = (1 << 16)
} base_codec_flag_enum_t;
typedef uint32_t base_codec_flag_t;


/*!
\enum base_speech_flag_t
\brief Speech related flags
<pre>
BASE_SPEECH_FLAG_HASTEXT =		(1 <<  0) - Interface is has text to read.
BASE_SPEECH_FLAG_PEEK =			(1 <<  1) - Read data but do not erase it.
BASE_SPEECH_FLAG_FREE_POOL =		(1 <<  2) - Free interface's pool on destruction.
BASE_SPEECH_FLAG_BLOCKING =       (1 <<  3) - Indicate that a blocking call is desired
BASE_SPEECH_FLAG_PAUSE = 			(1 <<  4) - Pause toggle for playback
</pre>
*/
typedef enum {
    BASE_SPEECH_FLAG_NONE = 0,
    BASE_SPEECH_FLAG_HASTEXT = (1 << 0),
    BASE_SPEECH_FLAG_PEEK = (1 << 1),
    BASE_SPEECH_FLAG_FREE_POOL = (1 << 2),
    BASE_SPEECH_FLAG_BLOCKING = (1 << 3),
    BASE_SPEECH_FLAG_PAUSE = (1 << 4),
    BASE_SPEECH_FLAG_OPEN = (1 << 5),
    BASE_SPEECH_FLAG_DONE = (1 << 6)
} base_speech_flag_enum_t;
typedef uint32_t base_speech_flag_t;

/*!
\enum base_asr_flag_t
\brief Asr related flags
<pre>
BASE_ASR_FLAG_DATA =			(1 <<  0) - Interface has data
BASE_ASR_FLAG_FREE_POOL =		(1 <<  1) - Pool needs to be freed
BASE_ASR_FLAG_CLOSED = 		(1 <<  2) - Interface has been closed
BASE_ASR_FLAG_FIRE_EVENTS =	(1 <<  3) - Fire all speech events
BASE_ASR_FLAG_AUTO_RESUME =   (1 <<  4) - Auto Resume
</pre>
*/
typedef enum {
    BASE_ASR_FLAG_NONE = 0,
    BASE_ASR_FLAG_DATA = (1 << 0),
    BASE_ASR_FLAG_FREE_POOL = (1 << 1),
    BASE_ASR_FLAG_CLOSED = (1 << 2),
    BASE_ASR_FLAG_FIRE_EVENTS = (1 << 3),
    BASE_ASR_FLAG_AUTO_RESUME = (1 << 4)

} base_asr_flag_enum_t;
typedef uint32_t base_asr_flag_t;

/*!
\enum base_directory_flag_t
\brief Directory Handle related flags
<pre>
BASE_DIRECTORY_FLAG_FREE_POOL =		(1 <<  0) - Free interface's pool on destruction.
</pre>
*/
typedef enum {
    BASE_DIRECTORY_FLAG_FREE_POOL = (1 << 0)

} base_directory_flag_enum_t;
typedef uint32_t base_directory_flag_t;

/*!
\enum base_codec_type_t
\brief Codec types
<pre>
BASE_CODEC_TYPE_AUDIO - Audio Codec
BASE_CODEC_TYPE_VIDEO - Video Codec
BASE_CODEC_TYPE_T38   - T38 Codec
BASE_CODEC_TYPE_APP   - Application Codec
</pre>
*/
typedef enum {
    BASE_CODEC_TYPE_AUDIO,
    BASE_CODEC_TYPE_VIDEO,
    BASE_CODEC_TYPE_T38,
    BASE_CODEC_TYPE_APP
} base_codec_type_t;

typedef enum {
    BASE_MEDIA_TYPE_AUDIO,
    BASE_MEDIA_TYPE_VIDEO,
    BASE_MEDIA_TYPE_TEXT
} base_media_type_t;
#define BASE_MEDIA_TYPE_TOTAL 3


/*!
\enum base_timer_flag_t
\brief Timer related flags
<pre>
BASE_TIMER_FLAG_FREE_POOL =		(1 <<  0) - Free timer's pool on destruction
</pre>
*/
typedef enum {
    BASE_TIMER_FLAG_FREE_POOL = (1 << 0)
} base_timer_flag_enum_t;
typedef uint32_t base_timer_flag_t;


/*!
\enum base_timer_flag_t
\brief Timer related flags
<pre>
SMBF_READ_STREAM - Include the Read Stream
SMBF_WRITE_STREAM - Include the Write Stream
SMBF_WRITE_REPLACE - Replace the Write Stream
SMBF_READ_REPLACE - Replace the Read Stream
SMBF_STEREO - Record in stereo
SMBF_ANSWER_REQ - Don't record until the channel is answered
SMBF_BRIDGE_REQ - Don't record until the channel is bridged
SMBF_THREAD_LOCK - Only let the same thread who created the bug remove it.
SMBF_PRUNE -
SMBF_NO_PAUSE -
SMBF_STEREO_SWAP - Record in stereo: Write Stream - left channel, Read Stream - right channel
</pre>
*/
typedef enum {
    SMBF_BOTH = 0,
    SMBF_READ_STREAM = (1 << 0),
    SMBF_WRITE_STREAM = (1 << 1),
    SMBF_WRITE_REPLACE = (1 << 2),
    SMBF_READ_REPLACE = (1 << 3),
    SMBF_READ_PING = (1 << 4),
    SMBF_STEREO = (1 << 5),
    SMBF_ANSWER_REQ = (1 << 6),
    SMBF_BRIDGE_REQ = (1 << 7),
    SMBF_THREAD_LOCK = (1 << 8),
    SMBF_PRUNE = (1 << 9),
    SMBF_NO_PAUSE = (1 << 10),
    SMBF_STEREO_SWAP = (1 << 11),
    SMBF_LOCK = (1 << 12),
    SMBF_TAP_NATIVE_READ = (1 << 13),
    SMBF_TAP_NATIVE_WRITE = (1 << 14),
    SMBF_ONE_ONLY = (1 << 15),
    SMBF_MASK = (1 << 16),
    SMBF_READ_VIDEO_PING = (1 << 17),
    SMBF_WRITE_VIDEO_PING = (1 << 18),
    SMBF_READ_VIDEO_STREAM = (1 << 19),
    SMBF_WRITE_VIDEO_STREAM = (1 << 20),
    SMBF_VIDEO_PATCH = (1 << 21),
    SMBF_SPY_VIDEO_STREAM = (1 << 22),
    SMBF_SPY_VIDEO_STREAM_BLEG = (1 << 23),
    SMBF_READ_VIDEO_PATCH = (1 << 24),
    SMBF_READ_TEXT_STREAM = (1 << 25),
    SMBF_FIRST = (1 << 26),
    SMBF_PAUSE = (1 << 27)
} base_media_bug_flag_enum_t;
typedef uint32_t base_media_bug_flag_t;

/*!
\enum base_file_flag_t
\brief File flags
<pre>
BASE_FILE_FLAG_READ =         (1 <<  0) - Open for read
BASE_FILE_FLAG_WRITE =        (1 <<  1) - Open for write
BASE_FILE_FLAG_FREE_POOL =    (1 <<  2) - Free file handle's pool on destruction
BASE_FILE_DATA_SHORT =        (1 <<  3) - Read data in shorts
BASE_FILE_DATA_INT =          (1 <<  4) - Read data in ints
BASE_FILE_DATA_FLOAT =        (1 <<  5) - Read data in floats
BASE_FILE_DATA_DOUBLE =       (1 <<  6) - Read data in doubles
BASE_FILE_DATA_RAW =          (1 <<  7) - Read data as is
BASE_FILE_PAUSE =             (1 <<  8) - Pause
BASE_FILE_NATIVE =            (1 <<  9) - File is in native format (no transcoding)
BASE_FILE_SEEK = 				(1 << 10) - File has done a seek
BASE_FILE_OPEN =              (1 << 11) - File is open
</pre>
*/
typedef enum {
    BASE_FILE_FLAG_READ = (1 << 0),
    BASE_FILE_FLAG_WRITE = (1 << 1),
    BASE_FILE_FLAG_FREE_POOL = (1 << 2),
    BASE_FILE_DATA_SHORT = (1 << 3),
    BASE_FILE_DATA_INT = (1 << 4),
    BASE_FILE_DATA_FLOAT = (1 << 5),
    BASE_FILE_DATA_DOUBLE = (1 << 6),
    BASE_FILE_DATA_RAW = (1 << 7),
    BASE_FILE_PAUSE = (1 << 8),
    BASE_FILE_NATIVE = (1 << 9),
    BASE_FILE_SEEK = (1 << 10),
    BASE_FILE_OPEN = (1 << 11),
    BASE_FILE_CALLBACK = (1 << 12),
    BASE_FILE_DONE = (1 << 13),
    BASE_FILE_BUFFER_DONE = (1 << 14),
    BASE_FILE_WRITE_APPEND = (1 << 15),
    BASE_FILE_WRITE_OVER = (1 << 16),
    BASE_FILE_NOMUX = (1 << 17),
    BASE_FILE_BREAK_ON_CHANGE = (1 << 18),
    BASE_FILE_FLAG_VIDEO = (1 << 19),
    BASE_FILE_FLAG_VIDEO_EOF = (1 << 20),
    BASE_FILE_PRE_CLOSED = (1 << 21)
} base_file_flag_enum_t;
typedef uint32_t base_file_flag_t;

typedef enum {
    BASE_IO_FLAG_NONE = 0,
    BASE_IO_FLAG_NOBLOCK = (1 << 0),
    BASE_IO_FLAG_SINGLE_READ = (1 << 1),
    BASE_IO_FLAG_FORCE = (1 << 2),
    BASE_IO_FLAG_QUEUED = (1 << 3)
} base_io_flag_enum_t;
typedef uint32_t base_io_flag_t;

/* make sure this is synced with the EVENT_NAMES array in base_event.c
also never put any new ones before EVENT_ALL
*/
/*!
\enum base_event_types_t
\brief Built-in Events

<pre>
BASE_EVENT_CUSTOM				- A custom event
BASE_EVENT_CLONE				- A cloned event
BASE_EVENT_CHANNEL_CREATE		- A channel has been created
BASE_EVENT_CHANNEL_DESTROY	- A channel has been destroyed
BASE_EVENT_CHANNEL_STATE		- A channel has changed state
BASE_EVENT_CHANNEL_CALLSTATE	- A channel has changed call state
BASE_EVENT_CHANNEL_ANSWER		- A channel has been answered
BASE_EVENT_CHANNEL_HANGUP		- A channel has been hungup
BASE_EVENT_CHANNEL_HANGUP_COMPLETE	- A channel has completed the hangup
BASE_EVENT_CHANNEL_EXECUTE	- A channel has executed a module's application
BASE_EVENT_CHANNEL_EXECUTE_COMPLETE	- A channel has finshed executing a module's application
BASE_EVENT_CHANNEL_HOLD		- A channel has been put on hold
BASE_EVENT_CHANNEL_UNHOLD		- A channel has been unheld
BASE_EVENT_CHANNEL_BRIDGE     - A channel has bridged to another channel
BASE_EVENT_CHANNEL_UNBRIDGE   - A channel has unbridged from another channel
BASE_EVENT_CHANNEL_PROGRESS	- A channel has started ringing
BASE_EVENT_CHANNEL_PROGRESS_MEDIA	- A channel has started early media
BASE_EVENT_CHANNEL_OUTGOING	- A channel has been unparked
BASE_EVENT_CHANNEL_PARK 		- A channel has been parked
BASE_EVENT_CHANNEL_UNPARK 	- A channel has been unparked
BASE_EVENT_CHANNEL_APPLICATION- A channel has called and event from an application
BASE_EVENT_CHANNEL_ORIGINATE  - A channel has been originated
BASE_EVENT_CHANNEL_UUID       - A channel has changed uuid
BASE_EVENT_API				- An API call has been executed
BASE_EVENT_LOG				- A LOG event has been triggered
BASE_EVENT_INBOUND_CHAN		- A new inbound channel has been created
BASE_EVENT_OUTBOUND_CHAN		- A new outbound channel has been created
BASE_EVENT_STARTUP			- The system has been started
BASE_EVENT_SHUTDOWN			- The system has been shutdown
BASE_EVENT_PUBLISH			- Publish
BASE_EVENT_UNPUBLISH			- UnPublish
BASE_EVENT_TALK				- Talking Detected
BASE_EVENT_NOTALK				- Not Talking Detected
BASE_EVENT_SESSION_CRASH		- Session Crashed
BASE_EVENT_MODULE_LOAD		- Module was loaded
BASE_EVENT_MODULE_UNLOAD		- Module was unloaded
BASE_EVENT_DTMF				- DTMF was sent
BASE_EVENT_MESSAGE			- A Basic Message
BASE_EVENT_PRESENCE_IN		- Presence in
BASE_EVENT_NOTIFY_IN			- Received incoming NOTIFY from gateway subscription
BASE_EVENT_PRESENCE_OUT		- Presence out
BASE_EVENT_PRESENCE_PROBE		- Presence probe
BASE_EVENT_MESSAGE_WAITING	- A message is waiting
BASE_EVENT_MESSAGE_QUERY		- A query for MESSAGE_WAITING events
BASE_EVENT_ROSTER				- ?
BASE_EVENT_CODEC				- Codec Change
BASE_EVENT_BACKGROUND_JOB		- Background Job
BASE_EVENT_DETECTED_SPEECH	- Detected Speech
BASE_EVENT_DETECTED_TONE      - Detected Tone
BASE_EVENT_PRIVATE_COMMAND	- A private command event
BASE_EVENT_HEARTBEAT			- Machine is alive
BASE_EVENT_TRAP				- Error Trap
BASE_EVENT_ADD_SCHEDULE		- Something has been scheduled
BASE_EVENT_DEL_SCHEDULE		- Something has been unscheduled
BASE_EVENT_EXE_SCHEDULE		- Something scheduled has been executed
BASE_EVENT_RE_SCHEDULE		- Something scheduled has been rescheduled
BASE_EVENT_RELOADXML			- XML registry has been reloaded
BASE_EVENT_NOTIFY				- Notification
BASE_EVENT_PHONE_FEATURE		- Notification (DND/CFWD/etc)
BASE_EVENT_PHONE_FEATURE_SUBSCRIBE - Phone feature subscription
BASE_EVENT_SEND_MESSAGE		- Message
BASE_EVENT_RECV_MESSAGE		- Message
BASE_EVENT_REQUEST_PARAMS
BASE_EVENT_CHANNEL_DATA
BASE_EVENT_GENERAL
BASE_EVENT_COMMAND
BASE_EVENT_SESSION_HEARTBEAT
BASE_EVENT_CLIENT_DISCONNECTED
BASE_EVENT_SERVER_DISCONNECTED
BASE_EVENT_SEND_INFO
BASE_EVENT_RECV_INFO
BASE_EVENT_RECV_RTCP_MESSAGE
BASE_EVENT_SEND_RTCP_MESSAGE
BASE_EVENT_CALL_SECURE
BASE_EVENT_NAT            	- NAT Management (new/del/status)
BASE_EVENT_RECORD_START
BASE_EVENT_RECORD_STOP
BASE_EVENT_PLAYBACK_START
BASE_EVENT_PLAYBACK_STOP
BASE_EVENT_CALL_UPDATE
BASE_EVENT_FAILURE            - A failure occurred which might impact the normal functioning of the base
BASE_EVENT_SOCKET_DATA
BASE_EVENT_MEDIA_BUG_START
BASE_EVENT_MEDIA_BUG_STOP
BASE_EVENT_CONFERENCE_DATA_QUERY
BASE_EVENT_CONFERENCE_DATA
BASE_EVENT_CALL_SETUP_REQ
BASE_EVENT_CALL_SETUP_RESULT
BASE_EVENT_CALL_DETAIL
BASE_EVENT_DEVICE_STATE
BASE_EVENT_SHUTDOWN_REQUESTED		- Shutdown of the system has been requested
BASE_EVENT_ALL				- All events at once
</pre>

*/
typedef enum {
    BASE_EVENT_CUSTOM,
    BASE_EVENT_CLONE,
    BASE_EVENT_CHANNEL_CREATE,
    BASE_EVENT_CHANNEL_DESTROY,
    BASE_EVENT_CHANNEL_STATE,
    BASE_EVENT_CHANNEL_CALLSTATE,
    BASE_EVENT_CHANNEL_ANSWER,
    BASE_EVENT_CHANNEL_HANGUP,
    BASE_EVENT_CHANNEL_HANGUP_COMPLETE,
    BASE_EVENT_CHANNEL_EXECUTE,
    BASE_EVENT_CHANNEL_EXECUTE_COMPLETE,
    BASE_EVENT_CHANNEL_HOLD,
    BASE_EVENT_CHANNEL_UNHOLD,
    BASE_EVENT_CHANNEL_BRIDGE,
    BASE_EVENT_CHANNEL_UNBRIDGE,
    BASE_EVENT_CHANNEL_PROGRESS,
    BASE_EVENT_CHANNEL_PROGRESS_MEDIA,
    BASE_EVENT_CHANNEL_OUTGOING,
    BASE_EVENT_CHANNEL_PARK,
    BASE_EVENT_CHANNEL_UNPARK,
    BASE_EVENT_CHANNEL_APPLICATION,
    BASE_EVENT_CHANNEL_ORIGINATE,
    BASE_EVENT_CHANNEL_UUID,
    BASE_EVENT_API,
    BASE_EVENT_LOG,
    BASE_EVENT_INBOUND_CHAN,
    BASE_EVENT_OUTBOUND_CHAN,
    BASE_EVENT_STARTUP,
    BASE_EVENT_SHUTDOWN,
    BASE_EVENT_PUBLISH,
    BASE_EVENT_UNPUBLISH,
    BASE_EVENT_TALK,
    BASE_EVENT_NOTALK,
    BASE_EVENT_SESSION_CRASH,
    BASE_EVENT_MODULE_LOAD,
    BASE_EVENT_MODULE_UNLOAD,
    BASE_EVENT_DTMF,
    BASE_EVENT_MESSAGE,
    BASE_EVENT_PRESENCE_IN,
    BASE_EVENT_NOTIFY_IN,
    BASE_EVENT_PRESENCE_OUT,
    BASE_EVENT_PRESENCE_PROBE,
    BASE_EVENT_MESSAGE_WAITING,
    BASE_EVENT_MESSAGE_QUERY,
    BASE_EVENT_ROSTER,
    BASE_EVENT_CODEC,
    BASE_EVENT_BACKGROUND_JOB,
    BASE_EVENT_DETECTED_SPEECH,
    BASE_EVENT_DETECTED_TONE,
    BASE_EVENT_PRIVATE_COMMAND,
    BASE_EVENT_HEARTBEAT,
    BASE_EVENT_TRAP,
    BASE_EVENT_ADD_SCHEDULE,
    BASE_EVENT_DEL_SCHEDULE,
    BASE_EVENT_EXE_SCHEDULE,
    BASE_EVENT_RE_SCHEDULE,
    BASE_EVENT_RELOADXML,
    BASE_EVENT_NOTIFY,
    BASE_EVENT_PHONE_FEATURE,
    BASE_EVENT_PHONE_FEATURE_SUBSCRIBE,
    BASE_EVENT_SEND_MESSAGE,
    BASE_EVENT_RECV_MESSAGE,
    BASE_EVENT_REQUEST_PARAMS,
    BASE_EVENT_CHANNEL_DATA,
    BASE_EVENT_GENERAL,
    BASE_EVENT_COMMAND,
    BASE_EVENT_SESSION_HEARTBEAT,
    BASE_EVENT_CLIENT_DISCONNECTED,
    BASE_EVENT_SERVER_DISCONNECTED,
    BASE_EVENT_SEND_INFO,
    BASE_EVENT_RECV_INFO,
    BASE_EVENT_RECV_RTCP_MESSAGE,
    BASE_EVENT_SEND_RTCP_MESSAGE,
    BASE_EVENT_CALL_SECURE,
    BASE_EVENT_NAT,
    BASE_EVENT_RECORD_START,
    BASE_EVENT_RECORD_STOP,
    BASE_EVENT_PLAYBACK_START,
    BASE_EVENT_PLAYBACK_STOP,
    BASE_EVENT_CALL_UPDATE,
    BASE_EVENT_FAILURE,
    BASE_EVENT_SOCKET_DATA,
    BASE_EVENT_MEDIA_BUG_START,
    BASE_EVENT_MEDIA_BUG_STOP,
    BASE_EVENT_CONFERENCE_DATA_QUERY,
    BASE_EVENT_CONFERENCE_DATA,
    BASE_EVENT_CALL_SETUP_REQ,
    BASE_EVENT_CALL_SETUP_RESULT,
    BASE_EVENT_CALL_DETAIL,
    BASE_EVENT_DEVICE_STATE,
    BASE_EVENT_TEXT,
    BASE_EVENT_SHUTDOWN_REQUESTED,
    BASE_EVENT_ALL
} base_event_types_t;

typedef enum {
    BASE_INPUT_TYPE_DTMF,
    BASE_INPUT_TYPE_EVENT
} base_input_type_t;

typedef enum {
    BASE_CAUSE_NONE = 0,
    BASE_CAUSE_UNALLOCATED_NUMBER = 1,
    BASE_CAUSE_NO_ROUTE_TRANSIT_NET = 2,
    BASE_CAUSE_NO_ROUTE_DESTINATION = 3,
    BASE_CAUSE_CHANNEL_UNACCEPTABLE = 6,
    BASE_CAUSE_CALL_AWARDED_DELIVERED = 7,
    BASE_CAUSE_NORMAL_CLEARING = 16,
    BASE_CAUSE_USER_BUSY = 17,
    BASE_CAUSE_NO_USER_RESPONSE = 18,
    BASE_CAUSE_NO_ANSWER = 19,
    BASE_CAUSE_SUBSCRIBER_ABSENT = 20,
    BASE_CAUSE_CALL_REJECTED = 21,
    BASE_CAUSE_NUMBER_CHANGED = 22,
    BASE_CAUSE_REDIRECTION_TO_NEW_DESTINATION = 23,
    BASE_CAUSE_EXCHANGE_ROUTING_ERROR = 25,
    BASE_CAUSE_DESTINATION_OUT_OF_ORDER = 27,
    BASE_CAUSE_INVALID_NUMBER_FORMAT = 28,
    BASE_CAUSE_FACILITY_REJECTED = 29,
    BASE_CAUSE_RESPONSE_TO_STATUS_ENQUIRY = 30,
    BASE_CAUSE_NORMAL_UNSPECIFIED = 31,
    BASE_CAUSE_NORMAL_CIRCUIT_CONGESTION = 34,
    BASE_CAUSE_NETWORK_OUT_OF_ORDER = 38,
    BASE_CAUSE_NORMAL_TEMPORARY_FAILURE = 41,
    BASE_CAUSE_BASE_CONGESTION = 42,
    BASE_CAUSE_ACCESS_INFO_DISCARDED = 43,
    BASE_CAUSE_REQUESTED_CHAN_UNAVAIL = 44,
    BASE_CAUSE_PRE_EMPTED = 45,
    BASE_CAUSE_FACILITY_NOT_SUBSCRIBED = 50,
    BASE_CAUSE_OUTGOING_CALL_BARRED = 52,
    BASE_CAUSE_INCOMING_CALL_BARRED = 54,
    BASE_CAUSE_BEARERCAPABILITY_NOTAUTH = 57,
    BASE_CAUSE_BEARERCAPABILITY_NOTAVAIL = 58,
    BASE_CAUSE_SERVICE_UNAVAILABLE = 63,
    BASE_CAUSE_BEARERCAPABILITY_NOTIMPL = 65,
    BASE_CAUSE_CHAN_NOT_IMPLEMENTED = 66,
    BASE_CAUSE_FACILITY_NOT_IMPLEMENTED = 69,
    BASE_CAUSE_SERVICE_NOT_IMPLEMENTED = 79,
    BASE_CAUSE_INVALID_CALL_REFERENCE = 81,
    BASE_CAUSE_INCOMPATIBLE_DESTINATION = 88,
    BASE_CAUSE_INVALID_MSG_UNSPECIFIED = 95,
    BASE_CAUSE_MANDATORY_IE_MISSING = 96,
    BASE_CAUSE_MESSAGE_TYPE_NONEXIST = 97,
    BASE_CAUSE_WRONG_MESSAGE = 98,
    BASE_CAUSE_IE_NONEXIST = 99,
    BASE_CAUSE_INVALID_IE_CONTENTS = 100,
    BASE_CAUSE_WRONG_CALL_STATE = 101,
    BASE_CAUSE_RECOVERY_ON_TIMER_EXPIRE = 102,
    BASE_CAUSE_MANDATORY_IE_LENGTH_ERROR = 103,
    BASE_CAUSE_PROTOCOL_ERROR = 111,
    BASE_CAUSE_INTERWORKING = 127,
    BASE_CAUSE_SUCCESS = 142,
    BASE_CAUSE_ORIGINATOR_CANCEL = 487,
    BASE_CAUSE_CRASH = 700,
    BASE_CAUSE_SYSTEM_SHUTDOWN = 701,
    BASE_CAUSE_LOSE_RACE = 702,
    BASE_CAUSE_MANAGER_REQUEST = 703,
    BASE_CAUSE_BLIND_TRANSFER = 800,
    BASE_CAUSE_ATTENDED_TRANSFER = 801,
    BASE_CAUSE_ALLOTTED_TIMEOUT = 802,
    BASE_CAUSE_USER_CHALLENGE = 803,
    BASE_CAUSE_MEDIA_TIMEOUT = 804,
    BASE_CAUSE_PICKED_OFF = 805,
    BASE_CAUSE_USER_NOT_REGISTERED = 806,
    BASE_CAUSE_PROGRESS_TIMEOUT = 807,
    BASE_CAUSE_INVALID_GATEWAY = 808,
    BASE_CAUSE_GATEWAY_DOWN = 809,
    BASE_CAUSE_INVALID_URL = 810,
    BASE_CAUSE_INVALID_PROFILE = 811,
    BASE_CAUSE_NO_PICKUP = 812,
    BASE_CAUSE_SRTP_READ_ERROR = 813,
    BASE_CAUSE_BOWOUT = 814,
    BASE_CAUSE_BUSY_EVERYWHERE = 815,
    BASE_CAUSE_DECLINE = 816,
    BASE_CAUSE_DOES_NOT_EXIST_ANYWHERE = 817,
    BASE_CAUSE_NOT_ACCEPTABLE = 818,
    BASE_CAUSE_UNWANTED = 819,
    BASE_CAUSE_NO_IDENTITY = 820,
    BASE_CAUSE_BAD_IDENTITY_INFO = 821,
    BASE_CAUSE_UNSUPPORTED_CERTIFICATE = 822,
    BASE_CAUSE_INVALID_IDENTITY = 823,
    BASE_CAUSE_STALE_DATE = 824,
    BASE_CAUSE_REJECT_ALL = 825
} base_call_cause_t;

typedef enum {
    SCSC_PAUSE_INBOUND,
    SCSC_PAUSE_OUTBOUND,
    SCSC_PAUSE_ALL,
    SCSC_HUPALL,
    SCSC_SHUTDOWN,
    SCSC_CHECK_RUNNING,
    SCSC_LOGLEVEL,
    SCSC_SPS,
    SCSC_LAST_SPS,
    SCSC_RECLAIM,
    SCSC_MAX_SESSIONS,
    SCSC_SYNC_CLOCK,
    SCSC_MAX_DTMF_DURATION,
    SCSC_MIN_DTMF_DURATION,
    SCSC_DEFAULT_DTMF_DURATION,
    SCSC_SHUTDOWN_ELEGANT,
    SCSC_SHUTDOWN_ASAP,
    SCSC_CANCEL_SHUTDOWN,
    SCSC_SEND_SIGHUP,
    SCSC_DEBUG_LEVEL,
    SCSC_FLUSH_DB_HANDLES,
    SCSC_SHUTDOWN_NOW,
    SCSC_REINCARNATE_NOW,
    SCSC_CALIBRATE_CLOCK,
    SCSC_SAVE_HISTORY,
    SCSC_CRASH,
    SCSC_MIN_IDLE_CPU,
    SCSC_VERBOSE_EVENTS,
    SCSC_SHUTDOWN_CHECK,
    SCSC_PAUSE_INBOUND_CHECK,
    SCSC_PAUSE_OUTBOUND_CHECK,
    SCSC_PAUSE_CHECK,
    SCSC_READY_CHECK,
    SCSC_THREADED_SYSTEM_EXEC,
    SCSC_SYNC_CLOCK_WHEN_IDLE,
    SCSC_DEBUG_SQL,
    SCSC_SQL,
    SCSC_API_EXPANSION,
    SCSC_RECOVER,
    SCSC_SPS_PEAK,
    SCSC_SPS_PEAK_FIVEMIN,
    SCSC_SESSIONS_PEAK,
    SCSC_SESSIONS_PEAK_FIVEMIN,
    SCSC_MDNS_RESOLVE,
    SCSC_SHUTDOWN_CAUSE
} base_session_ctl_t;

typedef enum {
    SSH_FLAG_STICKY = (1 << 0),
    SSH_FLAG_PRE_EXEC = (1 << 1)
} base_state_handler_flag_t;

#ifdef WIN32
typedef SOCKET base_os_socket_t;
#define BASE_SOCK_INVALID INVALID_SOCKET
#else
typedef int base_os_socket_t;
#define BASE_SOCK_INVALID -1
#endif

typedef struct fspr_pool_t base_memory_pool_t;
typedef void* base_plc_state_t;
typedef uint16_t base_port_t;
typedef uint8_t base_payload_t;
typedef struct base_app_log base_app_log_t;
typedef struct base_rtp base_rtp_t;
typedef struct base_rtcp base_rtcp_t;
typedef struct base_core_session_message base_core_session_message_t;
typedef struct base_event_header base_event_header_t;
typedef struct base_event base_event_t;
typedef struct base_event_subclass base_event_subclass_t;
typedef struct base_event_node base_event_node_t;
typedef struct base_loadable_module base_loadable_module_t;
typedef struct base_frame base_frame_t;
typedef struct base_rtcp_frame base_rtcp_frame_t;
typedef struct base_channel base_channel_t;
typedef struct base_sql_queue_manager base_sql_queue_manager_t;
typedef struct base_file_handle base_file_handle_t;
typedef struct base_caller_profile base_caller_profile_t;
typedef struct base_caller_extension base_caller_extension_t;
typedef struct base_caller_application base_caller_application_t;
typedef struct base_state_handler_table base_state_handler_table_t;
typedef struct base_timer base_timer_t;
typedef struct base_codec base_codec_t;
typedef struct base_core_thread_session base_core_thread_session_t;
typedef struct base_codec_implementation base_codec_implementation_t;
typedef struct base_buffer base_buffer_t;
typedef union  base_codec_settings base_codec_settings_t;
typedef struct base_codec_fmtp base_codec_fmtp_t;
typedef struct base_coredb_handle base_coredb_handle_t;
typedef struct base_odbc_handle base_odbc_handle_t;
typedef struct base_database_interface_handle base_database_interface_handle_t;

typedef struct base_io_routines base_io_routines_t;
typedef struct base_speech_handle base_speech_handle_t;
typedef struct base_asr_handle base_asr_handle_t;
typedef struct base_directory_handle base_directory_handle_t;
typedef struct base_loadable_module_interface base_loadable_module_interface_t;
typedef struct base_endpoint_interface base_endpoint_interface_t;
typedef struct base_timer_interface base_timer_interface_t;
typedef struct base_dialplan_interface base_dialplan_interface_t;
typedef struct base_codec_interface base_codec_interface_t;
typedef struct base_application_interface base_application_interface_t;
typedef struct base_chat_application_interface base_chat_application_interface_t;
typedef struct base_api_interface base_api_interface_t;
typedef struct base_json_api_interface base_json_api_interface_t;
typedef struct base_file_interface base_file_interface_t;
typedef struct base_speech_interface base_speech_interface_t;
typedef struct base_asr_interface base_asr_interface_t;
typedef struct base_directory_interface base_directory_interface_t;
typedef struct base_chat_interface base_chat_interface_t;
typedef struct base_management_interface base_management_interface_t;
typedef struct base_core_port_allocator base_core_port_allocator_t;
typedef struct base_media_bug base_media_bug_t;
typedef struct base_limit_interface base_limit_interface_t;
typedef struct base_database_interface base_database_interface_t;

typedef void (*hashtable_destructor_t)(void *ptr);

struct base_console_callback_match_node {
    char *val;
    struct base_console_callback_match_node *next;
};
typedef struct base_console_callback_match_node base_console_callback_match_node_t;

struct base_console_callback_match {
    struct base_console_callback_match_node *head;
    struct base_console_callback_match_node *end;
    int count;
    int dynamic;
};
typedef struct base_console_callback_match base_console_callback_match_t;

typedef void (*base_media_bug_exec_cb_t)(base_media_bug_t *bug, void *user_data);

typedef base_status_t (*base_core_video_thread_callback_func_t) (base_core_session_t *session, base_frame_t *frame, void *user_data);
typedef base_status_t (*base_core_text_thread_callback_func_t) (base_core_session_t *session, base_frame_t *frame, void *user_data);
typedef void (*base_cap_callback_t) (const char *var, const char *val, void *user_data);
typedef base_status_t (*base_console_complete_callback_t) (const char *, const char *, base_console_callback_match_t **matches);
typedef base_bool_t (*base_media_bug_callback_t) (base_media_bug_t *, void *, base_abc_type_t);
typedef base_bool_t (*base_tone_detect_callback_t) (base_core_session_t *, const char *, const char *);
typedef struct base_xml_binding base_xml_binding_t;

typedef void (*base_engine_function_t) (base_core_session_t *session, void *user_data);


typedef base_status_t (*base_core_codec_encode_func_t) (base_codec_t *codec,
    base_codec_t *other_codec,
    void *decoded_data,
    uint32_t decoded_data_len,
    uint32_t decoded_rate,
    void *encoded_data, uint32_t *encoded_data_len, uint32_t *encoded_rate, unsigned int *flag);


typedef base_status_t (*base_core_codec_decode_func_t) (base_codec_t *codec,
    base_codec_t *other_codec,
    void *encoded_data,
    uint32_t encoded_data_len,
    uint32_t encoded_rate,
    void *decoded_data, uint32_t *decoded_data_len, uint32_t *decoded_rate, unsigned int *flag);

typedef base_status_t (*base_core_codec_video_encode_func_t) (base_codec_t *codec, base_frame_t *frame);

typedef base_status_t (*base_core_codec_video_decode_func_t) (base_codec_t *codec, base_frame_t *frame);

typedef enum {
    SCC_VIDEO_GEN_KEYFRAME = 0,
    SCC_VIDEO_BANDWIDTH,
    SCC_VIDEO_RESET,
    SCC_AUDIO_PACKET_LOSS,
    SCC_AUDIO_ADJUST_BITRATE,
    SCC_AUDIO_VAD,
    SCC_DEBUG,
    SCC_CODEC_SPECIFIC
} base_codec_control_command_t;

typedef enum {
    SCCT_NONE = 0,
    SCCT_STRING,
    SCCT_INT,
} base_codec_control_type_t;

typedef enum {
    BASE_IO_READ,
    BASE_IO_WRITE
} base_io_type_t;

typedef base_status_t (*base_core_codec_control_func_t) (base_codec_t *codec,
    base_codec_control_command_t cmd,
    base_codec_control_type_t ctype,
    void *cmd_data,
    base_codec_control_type_t atype,
    void *cmd_arg,
    base_codec_control_type_t *rtype,
    void **ret_data);


typedef base_status_t (*base_core_codec_init_func_t) (base_codec_t *, base_codec_flag_t, const base_codec_settings_t *codec_settings);
typedef base_status_t (*base_core_codec_fmtp_parse_func_t) (const char *fmtp, base_codec_fmtp_t *codec_fmtp);
typedef base_status_t (*base_core_codec_destroy_func_t) (base_codec_t *);


typedef base_status_t (*base_chat_application_function_t) (base_event_t *, const char *);
#define BASE_STANDARD_CHAT_APP(name) static base_status_t name (base_event_t *message, const char *data)

typedef void (*base_application_function_t) (base_core_session_t *, const char *);
#define BASE_STANDARD_APP(name) static void name (base_core_session_t *session, const char *data)

typedef int (*base_core_recover_callback_t)(base_core_session_t *session);
typedef void (*base_event_callback_t) (base_event_t *);
typedef base_caller_extension_t *(*base_dialplan_hunt_function_t) (base_core_session_t *, void *, base_caller_profile_t *);
#define BASE_STANDARD_DIALPLAN(name) static base_caller_extension_t *name (base_core_session_t *session, void *arg, base_caller_profile_t *caller_profile)

typedef base_bool_t (*base_hash_delete_callback_t) (_In_ const void *key, _In_ const void *val, _In_opt_ void *pData);
#define BASE_HASH_DELETE_FUNC(name) static base_bool_t name (const void *key, const void *val, void *pData)

typedef struct base_scheduler_task base_scheduler_task_t;

typedef void (*base_scheduler_func_t) (base_scheduler_task_t *task);

#define BASE_STANDARD_SCHED_FUNC(name) static void name (base_scheduler_task_t *task)

typedef base_status_t (*base_state_handler_t) (base_core_session_t *);
typedef struct base_stream_handle base_stream_handle_t;
typedef uint8_t * (*base_stream_handle_read_function_t) (base_stream_handle_t *handle, int *len);
typedef base_status_t (*base_stream_handle_write_function_t) (base_stream_handle_t *handle, const char *fmt, ...);
typedef base_status_t (*base_stream_handle_raw_write_function_t) (base_stream_handle_t *handle, uint8_t *data, base_size_t datalen);

typedef base_status_t (*base_api_function_t) (_In_opt_z_ const char *cmd, _In_opt_ base_core_session_t *session,
    _In_ base_stream_handle_t *stream);


#define BASE_STANDARD_API(name) static base_status_t name (_In_opt_z_ const char *cmd, _In_opt_ base_core_session_t *session, _In_ base_stream_handle_t *stream)


#define BASE_STANDARD_JSON_API(name) static base_status_t name (const cJSON *json, _In_opt_ base_core_session_t *session, cJSON **json_reply)

typedef base_status_t (*base_input_callback_function_t) (base_core_session_t *session, void *input,
    base_input_type_t input_type, void *buf, unsigned int buflen);
typedef base_status_t (*base_read_frame_callback_function_t) (base_core_session_t *session, base_frame_t *frame, void *user_data);
typedef struct base_say_interface base_say_interface_t;

#define DMACHINE_MAX_DIGIT_LEN 512

typedef enum {
    DM_MATCH_POSITIVE,
    DM_MATCH_NEGATIVE
} dm_match_type_t;

struct base_ivr_dmachine;
typedef struct base_ivr_dmachine base_ivr_dmachine_t;

struct base_ivr_dmachine_match {
    base_ivr_dmachine_t *dmachine;
    const char *match_digits;
    int32_t match_key;
    dm_match_type_t type;
    void *user_data;
};

typedef struct base_ivr_dmachine_match base_ivr_dmachine_match_t;
typedef base_status_t (*base_ivr_dmachine_callback_t) (base_ivr_dmachine_match_t *match);

#define MAX_ARG_RECURSION 25

#define arg_recursion_check_start(_args) if (_args) {					\
		if (_args->loops >= MAX_ARG_RECURSION) {						\
			base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_ERROR,		\
							  "RECURSION ERROR!  It's not the best idea to call things that collect input recursively from an input callback.\n"); \
			return BASE_STATUS_GENERR;								\
		} else {_args->loops++;}										\
	}


#define arg_recursion_check_stop(_args) if (_args) _args->loops--

typedef struct {
    base_input_callback_function_t input_callback;
    void *buf;
    uint32_t buflen;
    base_read_frame_callback_function_t read_frame_callback;
    void *user_data;
    base_ivr_dmachine_t *dmachine;
    int loops;
} base_input_args_t;


typedef struct {
    base_say_type_t type;
    base_say_method_t method;
    base_say_gender_t gender;
    const char *ext;
} base_say_args_t;


typedef base_status_t (*base_say_callback_t) (base_core_session_t *session,
    char *tosay,
    base_say_args_t *say_args,
    base_input_args_t *args);

typedef base_status_t (*base_say_string_callback_t) (base_core_session_t *session,
    char *tosay,
    base_say_args_t *say_args, char **rstr);

struct base_say_file_handle;
typedef struct base_say_file_handle base_say_file_handle_t;

typedef base_status_t (*base_new_say_callback_t) (base_say_file_handle_t *sh,
    char *tosay,
    base_say_args_t *say_args);


typedef struct base_xml *base_xml_t;
typedef struct base_core_time_duration base_core_time_duration_t;
typedef base_xml_t(*base_xml_open_root_function_t) (uint8_t reload, const char **err, void *user_data);
typedef base_xml_t(*base_xml_search_function_t) (const char *section,
    const char *tag_name, const char *key_name, const char *key_value, base_event_t *params,
    void *user_data);

struct base_hashtable;
struct base_hashtable_iterator;
typedef struct base_hashtable base_hash_t;
typedef struct base_hashtable base_inthash_t;
typedef struct base_hashtable_iterator base_hash_index_t;

struct base_network_list;
typedef struct base_network_list base_network_list_t;


#define BASE_API_VERSION 5
#define BASE_MODULE_LOAD_ARGS (base_loadable_module_interface_t **module_interface, base_memory_pool_t *pool)
#define BASE_MODULE_RUNTIME_ARGS (void)
#define BASE_MODULE_SHUTDOWN_ARGS (void)
typedef base_status_t (*base_module_load_t) BASE_MODULE_LOAD_ARGS;
typedef base_status_t (*base_module_runtime_t) BASE_MODULE_RUNTIME_ARGS;
typedef base_status_t (*base_module_shutdown_t) BASE_MODULE_SHUTDOWN_ARGS;
#define BASE_MODULE_LOAD_FUNCTION(name) base_status_t name BASE_MODULE_LOAD_ARGS
#define BASE_MODULE_RUNTIME_FUNCTION(name) base_status_t name BASE_MODULE_RUNTIME_ARGS
#define BASE_MODULE_SHUTDOWN_FUNCTION(name) base_status_t name BASE_MODULE_SHUTDOWN_ARGS

typedef enum {
    BASE_PRI_LOW = 1,
    BASE_PRI_NORMAL = 10,
    BASE_PRI_IMPORTANT = 50,
    BASE_PRI_REALTIME = 99,
} base_thread_priority_t;

typedef enum {
    SMODF_NONE = 0,
    SMODF_GLOBAL_SYMBOLS = (1 << 0)
} base_module_flag_enum_t;
typedef uint32_t base_module_flag_t;

typedef struct base_loadable_module_function_table {
    int base_api_version;
    base_module_load_t load;
    base_module_shutdown_t shutdown;
    base_module_runtime_t runtime;
    base_module_flag_t flags;
} base_loadable_module_function_table_t;

typedef int (*base_modulename_callback_func_t) (void *user_data, const char *module_name);

typedef struct base_slin_data base_slin_data_t;

#define BASE_MODULE_DEFINITION_EX(name, load, shutdown, runtime, flags)					\
static const char modname[] =  #name ;														\
BASE_MOD_DECLARE_DATA base_loadable_module_function_table_t name##_module_interface = {	\
	BASE_API_VERSION,																		\
	load,																					\
	shutdown,																				\
	runtime,																				\
	flags																					\
}

#define BASE_MODULE_DEFINITION(name, load, shutdown, runtime)								\
		BASE_MODULE_DEFINITION_EX(name, load, shutdown, runtime, SMODF_NONE)

/* things we don't deserve to know about */
/*! \brief A channel */
struct base_channel;
/*! \brief A core session representing a call and all of it's resources */
struct base_core_session;
/*! \brief An audio bug */
struct base_media_bug;
/*! \brief A digit stream parser object */
struct base_ivr_digit_stream_parser;
struct sql_queue_manager;

struct base_media_handle_s;
typedef struct base_media_handle_s base_media_handle_t;

typedef uint32_t base_event_channel_id_t;

struct base_live_array_s;
typedef struct base_live_array_s base_live_array_t;

typedef enum {
    SDP_OFFER,
    SDP_ANSWER
} base_sdp_type_t;

#define SDP_TYPE_REQUEST SDP_OFFER
#define SDP_TYPE_RESPONSE SDP_ANSWER

typedef enum {
    AEAD_AES_256_GCM_8,
    AEAD_AES_256_GCM,
    AEAD_AES_128_GCM_8,
    AEAD_AES_128_GCM,
    AES_CM_256_HMAC_SHA1_80,
    AES_CM_192_HMAC_SHA1_80,
    AES_CM_128_HMAC_SHA1_80,
    AES_CM_256_HMAC_SHA1_32,
    AES_CM_192_HMAC_SHA1_32,
    AES_CM_128_HMAC_SHA1_32,
    AES_CM_128_NULL_AUTH,
    CRYPTO_INVALID
} base_rtp_crypto_key_type_t;

/* Keep in sync with CRYPTO_KEY_PARAM_METHOD table. */
typedef enum {
    CRYPTO_KEY_PARAM_METHOD_INLINE,											/* identified by "inline" chars in SRTP key parameter */
    CRYPTO_KEY_PARAM_METHOD_INVALID
} base_rtp_crypto_key_param_method_type_t;

typedef struct payload_map_s {
    base_media_type_t type;
    base_sdp_type_t sdp_type;
    uint32_t ptime;
    uint32_t rate;
    uint8_t allocated;
    uint8_t negotiated;
    uint8_t current;
    unsigned long hash;

    char *rm_encoding;
    char *iananame;
    char *modname;
    base_payload_t pt;
    unsigned long rm_rate;
    unsigned long adv_rm_rate;
    uint32_t codec_ms;
    uint32_t bitrate;

    char *rm_fmtp;

    base_payload_t recv_pt;

    char *fmtp_out;

    char *remote_sdp_ip;
    base_port_t remote_sdp_port;

    int channels;
    int adv_channels;

    struct payload_map_s *next;

} payload_map_t;

typedef enum {
    BASE_MEDIA_FLOW_SENDRECV = 0,
    BASE_MEDIA_FLOW_SENDONLY,
    BASE_MEDIA_FLOW_RECVONLY,
    BASE_MEDIA_FLOW_INACTIVE,
    BASE_MEDIA_FLOW_DISABLED
} base_media_flow_t;

typedef enum {
    ICE_GOOGLE_JINGLE = (1 << 0),
    ICE_VANILLA = (1 << 1),
    ICE_CONTROLLED = (1 << 2),
    ICE_LITE = (1 << 3)
} base_core_media_ice_type_t;

typedef enum {
    BASE_POLL_READ = (1 << 0),
    BASE_POLL_WRITE = (1 << 1),
    BASE_POLL_ERROR = (1 << 2),
    BASE_POLL_HUP = (1 << 3),
    BASE_POLL_RDNORM = (1 << 4),
    BASE_POLL_RDBAND = (1 << 5),
    BASE_POLL_PRI = (1 << 6),
    BASE_POLL_INVALID = (1 << 7)
} base_poll_t;

typedef struct base_waitlist_s {
    base_os_socket_t sock;
    uint32_t events;
    uint32_t revents;
} base_waitlist_t;

struct base_jb_s;
typedef struct base_jb_s base_jb_t;

//struct kalman_estimator_s;
//typedef struct kalman_estimator_s kalman_estimator_t;

//struct cusum_kalman_detector_s;
//typedef struct cusum_kalman_detector_s cusum_kalman_detector_t;

struct base_img_txt_handle_s;
typedef struct base_img_txt_handle_s base_img_txt_handle_t;

struct base_frame_buffer_s;
typedef struct base_frame_buffer_s base_frame_buffer_t;

typedef enum {
    SVR_BLOCK = (1 << 0),
    SVR_FLUSH = (1 << 1),
    SVR_CHECK = (1 << 2)
} base_video_read_flag_t;

typedef enum {
    SPY_LOWER_RIGHT_SMALL,
    SPY_LOWER_RIGHT_LARGE,
    SPY_DUAL_CROP
} base_vid_spy_fmt_t;

typedef enum {
    SCFC_FLUSH_AUDIO,
    SCFC_PAUSE_READ,
    SCFC_PAUSE_WRITE,
    SCFC_RESUME_WRITE
} base_file_command_t;


struct base_rtp_text_factory_s;
typedef struct base_rtp_text_factory_s  base_rtp_text_factory_t;
typedef struct base_agc_s base_agc_t;

struct base_chromakey_s;
typedef struct base_chromakey_s base_chromakey_t;

typedef enum {
    BASE_VIDEO_ENCODE_SPEED_DEFAULT = 0,
    BASE_VIDEO_ENCODE_SPEED_FAST = 0,
    BASE_VIDEO_ENCODE_SPEED_MEDIUM,
    BASE_VIDEO_ENCODE_SPEED_SLOW
} base_video_encode_speed_t;

typedef enum {
    BASE_VIDEO_PROFILE_BASELINE,
    BASE_VIDEO_PROFILE_MAIN,
    BASE_VIDEO_PROFILE_HIGH
} base_video_profile_t;

#define BASE_RTP_MAX_CRYPTO_LEN 64

/* If MKI is used, then one or more key-materials are present in the <key-params> section of the crypto attribute.
* This struct describes the single MKI entry (key-material) within <key-params> section of crypto attribute.
* Key-material follows the format:
*		"inline:" <key||salt> ["|" lifetime] ["|" MKI ":" length]
* which translates to
*		"inline: KEYSALT|MKI_ID:MKI_SZ" or "inline: KEYSALT|LIFETIME|MKI_ID:MKI_SZ" */
typedef struct base_crypto_key_material_s {
    base_rtp_crypto_key_param_method_type_t	method;
    unsigned char								raw_key[BASE_RTP_MAX_CRYPTO_LEN];	/* Key-salt. Master key appended with salt. Sizes determined by crypto suite. */
    char										*crypto_key;						/* Complete key material string ("method:keysalt[|lifetime]|mki"). */
    uint64_t									lifetime;	/* OPTIONAL. The lifetime value MAY be written as a non-zero, positive decimal integer or as a power of 2. Must be less than max lifetime of RTP and RTCP packets in given crypto suite. */
    unsigned int								mki_id;		/* OPTIONAL. */
    unsigned int								mki_size;	/* OPTIONAL. Byte length of the master key field in the RTP packet. */
    struct base_crypto_key_material_s			*next;		/* NULL if this is the last master key in crypto attribute set. */
} base_crypto_key_material_t;

typedef struct secure_settings_s {
    int crypto_tag;
    unsigned char local_raw_key[BASE_RTP_MAX_CRYPTO_LEN];
    unsigned char remote_raw_key[BASE_RTP_MAX_CRYPTO_LEN];
    base_rtp_crypto_key_type_t crypto_type;
    char *local_crypto_key;
    char *remote_crypto_key;

    /* 
    * MKI support (as per rfc4568).
    * Normally single crypto attribute contains one key-material in a <key-params> section, e.g. "inline: KEYSALT" or "inline: KEYSALT|2^LIFETIME_BITS".
    * But if MKI is used, then one or more key-materials are present in the <key-params> section of the crypto attribute. Each key-material follows the format:
    *
    * "inline:" <key||salt> ["|" lifetime] ["|" MKI ":" length]
    *
    *		"inline: KEYSALT|MKI_ID:MKI_SZ"
    * or
    *		"inline: KEYSALT|2^LIFETIME_BITS|MKI_ID:MKI_SZ"
    *
    * This points to singly linked list of key-material descriptions if there is more than one key-material present in this crypto attribute (this key is inserted as the head of the list in that case), or to NULL otherwise.
    */
    struct base_crypto_key_material_s	*local_key_material_next;		/* NULL if MKI not used for crypto set on outbound SRTP. */
    unsigned long						local_key_material_n;			/* number of key_materials in the linked list for outbound SRTP */
    struct base_crypto_key_material_s	*remote_key_material_next;		/* NULL if MKI not used for crypto set on inbound SRTP. */
    unsigned long						remote_key_material_n;			/* number of key_materials in the linked list for inbound SRTP */
} base_secure_settings_t;

/* Default MKI index used for packets send from FS. We always use first key if multiple master keys are present in the crypto attribute. */ 
#define BASE_CRYPTO_MKI_INDEX 0

/* max number of MKI in a single crypto line supported */
#define BASE_CRYPTO_MKI_MAX	20

struct base_dial_handle_s;
typedef struct base_dial_handle_s base_dial_handle_t;

struct base_dial_leg_s;
typedef struct base_dial_leg_s base_dial_leg_t;

struct base_dial_leg_list_s;
typedef struct base_dial_leg_list_s base_dial_leg_list_t;

struct base_dial_handle_list_s;
typedef struct base_dial_handle_list_s base_dial_handle_list_t;

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
