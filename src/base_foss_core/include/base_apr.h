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
* Eliot Gable <egable@gmail.com>
*
* base_apr.h -- APR includes header
*
*/
/*! \file base_apr.h
\brief APR includes header

The things powered by APR are renamed into the base_ namespace to provide a cleaner
look to things and helps me to document what parts of APR I am using I'd like to take this
opportunity to thank APR for all the awesome stuff it does and for making my life much easier.

*/
#ifndef BASE_APR_H
#define BASE_APR_H

BASE_BEGIN_EXTERN_C

BASE_DECLARE(int) base_status_is_timeup(int status);

#ifdef WIN32
typedef DWORD base_thread_id_t;
#else
#include <pthread.h>
typedef pthread_t base_thread_id_t;
#endif

BASE_DECLARE(base_thread_id_t) base_thread_self(void);

/*! \brief Compare two thread ids
*  \param tid1 1st Thread ID to compare
*  \param tid2 2nd Thread ID to compare
*/
BASE_DECLARE(int) base_thread_equal(base_thread_id_t tid1, base_thread_id_t tid2);


/*
The pieces of apr we allow ppl to pass around between modules we typedef into our namespace and wrap all the functions
any other apr code should be as hidden as possible.
*/
/**
* @defgroup base_apr Brought To You By APR
* @ingroup FREEBASE
* @{
*/
/**
* @defgroup base_memory_pool Memory Pool Functions
* @ingroup base_apr
* @{
*/
/** The fundamental pool type */
/* see base types.h 	typedef struct fspr_pool_t base_memory_pool_t;*/
/**
* Clear all memory in the pool and run all the cleanups. This also destroys all
* subpools.
* @param pool The pool to clear
* @remark This does not actually free the memory, it just allows the pool
*         to re-use this memory for the next allocation.
* @see fspr_pool_destroy()
*/
BASE_DECLARE(void) base_pool_clear(base_memory_pool_t *pool);

/** @} */

/**
* @defgroup base_string String Handling funcions
* @ingroup base_apr
* @{
*/

BASE_DECLARE(int) base_snprintf(_Out_z_cap_(len)
    char *buf, _In_ base_size_t len, _In_z_ _Printf_format_string_ const char *format, ...);

BASE_DECLARE(int) base_vasprintf(_Out_opt_ char **buf, _In_z_ _Printf_format_string_ const char *format, _In_ va_list ap);

BASE_DECLARE(int) base_vsnprintf(char *buf, base_size_t len, const char *format, va_list ap);

BASE_DECLARE(char *) base_copy_string(_Out_z_cap_(dst_size)
    char *dst, _In_z_ const char *src, _In_ base_size_t dst_size);

/** @} */

#if 0
/**
* @defgroup fspr_hash Hash Tables
* @ingroup base_apr
* @{
*/

/** Abstract type for hash tables. */
typedef struct fspr_hash_t base_hash_t;

/** Abstract type for scanning hash tables. */
typedef struct fspr_hash_index_t base_hash_index_t;

/**
* When passing a key to base_hashfunc_default, this value can be
* passed to indicate a string-valued key, and have the length compute automatically.
*
*/
#define BASE_HASH_KEY_STRING     (-1)

/**
* Start iterating over the entries in a hash table.
* @param p The pool to allocate the base_hash_index_t iterator. If this
*          pool is NULL, then an internal, non-thread-safe iterator is used.
* @param ht The hash table
* @remark  There is no restriction on adding or deleting hash entries during
* an iteration (although the results may be unpredictable unless all you do
* is delete the current entry) and multiple iterations can be in
* progress at the same time.

*/
BASE_DECLARE(base_hash_index_t *) base_core_hash_first(base_memory_pool_t *pool, base_hash_t *ht);

/**
* Continue iterating over the entries in a hash table.
* @param ht The iteration state
* @return a pointer to the updated iteration state.  NULL if there are no more
*         entries.
*/
BASE_DECLARE(base_hash_index_t *) base_core_hash_next(base_hash_index_t *ht);

/**
* Get the current entry's details from the iteration state.
* @param hi The iteration state
* @param key Return pointer for the pointer to the key.
* @param klen Return pointer for the key length.
* @param val Return pointer for the associated value.
* @remark The return pointers should point to a variable that will be set to the
*         corresponding data, or they may be NULL if the data isn't interesting.
*/
BASE_DECLARE(void) base_core_hash_this(base_hash_index_t *hi, const void **key, base_ssize_t *klen, void **val);



BASE_DECLARE(base_memory_pool_t *) base_hash_pool_get(base_hash_t *ht);

/** @} */


#endif

/**
* The default hash function.
* @param key pointer to the key.
* @param klen the key length.
*
*/
BASE_DECLARE(unsigned int) base_hashfunc_default(const char *key, base_ssize_t *klen);

BASE_DECLARE(unsigned int) base_ci_hashfunc_default(const char *char_key, base_ssize_t *klen);


/**
* @defgroup base_time Time Routines
* @ingroup base_apr
* @{
*/

/** number of microseconds since 00:00:00 january 1, 1970 UTC */
typedef int64_t base_time_t;

/** number of microseconds in the interval */
typedef int64_t base_interval_time_t;

/**
* a structure similar to ANSI struct tm with the following differences:
*  - tm_usec isn't an ANSI field
*  - tm_gmtoff isn't an ANSI field (it's a bsdism)
*/
typedef struct base_time_exp_t {
    /** microseconds past tm_sec */
    int32_t tm_usec;
    /** (0-61) seconds past tm_min */
    int32_t tm_sec;
    /** (0-59) minutes past tm_hour */
    int32_t tm_min;
    /** (0-23) hours past midnight */
    int32_t tm_hour;
    /** (1-31) day of the month */
    int32_t tm_mday;
    /** (0-11) month of the year */
    int32_t tm_mon;
    /** year since 1900 */
    int32_t tm_year;
    /** (0-6) days since sunday */
    int32_t tm_wday;
    /** (0-365) days since jan 1 */
    int32_t tm_yday;
    /** daylight saving time */
    int32_t tm_isdst;
    /** seconds east of UTC */
    int32_t tm_gmtoff;
} base_time_exp_t;

BASE_DECLARE(base_time_t) base_time_make(base_time_t sec, int32_t usec);

/**
* @return the current time
*/
BASE_DECLARE(base_time_t) base_time_now(void);

/**
* Convert time value from human readable format to a numeric fspr_time_t that
* always represents GMT
* @param result the resulting imploded time
* @param input the input exploded time
*/
BASE_DECLARE(base_status_t) base_time_exp_gmt_get(base_time_t *result, base_time_exp_t *input);

/**
* formats the exploded time according to the format specified
* @param s string to write to
* @param retsize The length of the returned string
* @param max The maximum length of the string
* @param format The format for the time string
* @param tm The time to convert
*/
BASE_DECLARE(base_status_t) base_strftime(char *s, base_size_t *retsize, base_size_t max, const char *format, base_time_exp_t *tm);

/**
* formats the exploded time according to the format specified (does not validate format string)
* @param s string to write to
* @param retsize The length of the returned string
* @param max The maximum length of the string
* @param format The format for the time string
* @param tm The time to convert
*/
BASE_DECLARE(base_status_t) base_strftime_nocheck(char *s, base_size_t *retsize, base_size_t max, const char *format, base_time_exp_t *tm);

/**
* base_rfc822_date formats dates in the RFC822
* format in an efficient manner.  It is a fixed length
* format which requires the indicated amount of storage,
* including the trailing NUL terminator.
* @param date_str String to write to.
* @param t the time to convert
*/
BASE_DECLARE(base_status_t) base_rfc822_date(char *date_str, base_time_t t);

/**
* convert a time to its human readable components in GMT timezone
* @param result the exploded time
* @param input the time to explode
*/
BASE_DECLARE(base_status_t) base_time_exp_gmt(base_time_exp_t *result, base_time_t input);

/**
* Convert time value from human readable format to a numeric fspr_time_t
* e.g. elapsed usec since epoch
* @param result the resulting imploded time
* @param input the input exploded time
*/
BASE_DECLARE(base_status_t) base_time_exp_get(base_time_t *result, base_time_exp_t *input);

/**
* convert a time to its human readable components in local timezone
* @param result the exploded time
* @param input the time to explode
*/
BASE_DECLARE(base_status_t) base_time_exp_lt(base_time_exp_t *result, base_time_t input);

/**
* convert a time to its human readable components in a specific timezone with offset
* @param result the exploded time
* @param input the time to explode
*/
BASE_DECLARE(base_status_t) base_time_exp_tz(base_time_exp_t *result, base_time_t input, base_int32_t offs);

/**
* Sleep for the specified number of micro-seconds.
* @param t desired amount of time to sleep.
* @warning May sleep for longer than the specified time.
*/
BASE_DECLARE(void) base_sleep(base_interval_time_t t);
BASE_DECLARE(void) base_micro_sleep(base_interval_time_t t);

/** @} */

/**
* @defgroup base_thread_mutex Thread Mutex Routines
* @ingroup base_apr
* @{
*/

/** Opaque thread-local mutex structure */
typedef struct fspr_thread_mutex_t base_mutex_t;

/** Lock Flags */
#define BASE_MUTEX_DEFAULT	0x0	/**< platform-optimal lock behavior */
#define BASE_MUTEX_NESTED		0x1	/**< enable nested (recursive) locks */
#define	BASE_MUTEX_UNNESTED	0x2	/**< disable nested locks */

/**
* Create and initialize a mutex that can be used to synchronize threads.
* @param lock the memory address where the newly created mutex will be
*        stored.
* @param flags Or'ed value of:
* <PRE>
*           BASE_THREAD_MUTEX_DEFAULT   platform-optimal lock behavior.
*           BASE_THREAD_MUTEX_NESTED    enable nested (recursive) locks.
*           BASE_THREAD_MUTEX_UNNESTED  disable nested locks (non-recursive).
* </PRE>
* @param pool the pool from which to allocate the mutex.
* @warning Be cautious in using BASE_THREAD_MUTEX_DEFAULT.  While this is the
* most optimial mutex based on a given platform's performance charateristics,
* it will behave as either a nested or an unnested lock.
*
*/
BASE_DECLARE(base_status_t) base_mutex_init(base_mutex_t ** lock, unsigned int flags, base_memory_pool_t *pool);


/**
* Destroy the mutex and free the memory associated with the lock.
* @param lock the mutex to destroy.
*/
BASE_DECLARE(base_status_t) base_mutex_destroy(base_mutex_t *lock);

/**
* Acquire the lock for the given mutex. If the mutex is already locked,
* the current thread will be put to sleep until the lock becomes available.
* @param lock the mutex on which to acquire the lock.
*/
BASE_DECLARE(base_status_t) base_mutex_lock(base_mutex_t *lock);

/**
* Release the lock for the given mutex.
* @param lock the mutex from which to release the lock.
*/
BASE_DECLARE(base_status_t) base_mutex_unlock(base_mutex_t *lock);

/**
* Attempt to acquire the lock for the given mutex. If the mutex has already
* been acquired, the call returns immediately with APR_EBUSY. Note: it
* is important that the APR_STATUS_IS_EBUSY(s) macro be used to determine
* if the return value was APR_EBUSY, for portability reasons.
* @param lock the mutex on which to attempt the lock acquiring.
*/
BASE_DECLARE(base_status_t) base_mutex_trylock(base_mutex_t *lock);

/** @} */

/**
* @defgroup base_atomic Multi-Threaded Adtomic Operations Routines
* @ingroup base_apr
* @{
*/

/** Opaque type used for the atomic operations */
#ifdef fspr_atomic_t
typedef fspr_atomic_t base_atomic_t;
#else
typedef uint32_t base_atomic_t;
#endif

/**
* Some architectures require atomic operations internal structures to be
* initialized before use.
* @param pool The memory pool to use when initializing the structures.
*/
BASE_DECLARE(base_status_t) base_atomic_init(base_memory_pool_t *pool);

/**
* Uses an atomic operation to read the uint32 value at the location specified
* by mem.
* @param mem The location of memory which stores the value to read.
*/
BASE_DECLARE(uint32_t) base_atomic_read(volatile base_atomic_t *mem);

/**
* Uses an atomic operation to set a uint32 value at a specified location of
* memory.
* @param mem The location of memory to set.
* @param val The uint32 value to set at the memory location.
*/
BASE_DECLARE(void) base_atomic_set(volatile base_atomic_t *mem, uint32_t val);

/**
* Uses an atomic operation to add the uint32 value to the value at the
* specified location of memory.
* @param mem The location of the value to add to.
* @param val The uint32 value to add to the value at the memory location.
*/
BASE_DECLARE(void) base_atomic_add(volatile base_atomic_t *mem, uint32_t val);

/**
* Uses an atomic operation to increment the value at the specified memroy
* location.
* @param mem The location of the value to increment.
*/
BASE_DECLARE(void) base_atomic_inc(volatile base_atomic_t *mem);

/**
* Uses an atomic operation to decrement the value at the specified memroy
* location.
* @param mem The location of the value to decrement.
*/
BASE_DECLARE(int)  base_atomic_dec(volatile base_atomic_t *mem);

/** @} */

/**
* @defgroup base_thread_rwlock Thread Read/Write lock Routines
* @ingroup base_apr
* @{
*/

/** Opaque structure used for the rwlock */
typedef struct fspr_thread_rwlock_t base_thread_rwlock_t;

BASE_DECLARE(base_status_t) base_thread_rwlock_create(base_thread_rwlock_t ** rwlock, base_memory_pool_t *pool);
BASE_DECLARE(base_status_t) base_thread_rwlock_destroy(base_thread_rwlock_t *rwlock);
BASE_DECLARE(base_memory_pool_t *) base_thread_rwlock_pool_get(base_thread_rwlock_t *rwlock);
BASE_DECLARE(base_status_t) base_thread_rwlock_rdlock(base_thread_rwlock_t *rwlock);
BASE_DECLARE(base_status_t) base_thread_rwlock_tryrdlock(base_thread_rwlock_t *rwlock);
BASE_DECLARE(base_status_t) base_thread_rwlock_wrlock(base_thread_rwlock_t *rwlock);
BASE_DECLARE(base_status_t) base_thread_rwlock_trywrlock(base_thread_rwlock_t *rwlock);
BASE_DECLARE(base_status_t) base_thread_rwlock_trywrlock_timeout(base_thread_rwlock_t *rwlock, int timeout);
BASE_DECLARE(base_status_t) base_thread_rwlock_unlock(base_thread_rwlock_t *rwlock);

/** @} */

/**
* @defgroup base_thread_cond Condition Variable Routines
* @ingroup base_apr
* @{
*/

/**
* Note: destroying a condition variable (or likewise, destroying or
* clearing the pool from which a condition variable was allocated) if
* any threads are blocked waiting on it gives undefined results.
*/

/** Opaque structure for thread condition variables */
typedef struct fspr_thread_cond_t base_thread_cond_t;

/**
* Create and initialize a condition variable that can be used to signal
* and schedule threads in a single process.
* @param cond the memory address where the newly created condition variable
*        will be stored.
* @param pool the pool from which to allocate the mutex.
*/
BASE_DECLARE(base_status_t) base_thread_cond_create(base_thread_cond_t ** cond, base_memory_pool_t *pool);

/**
* Put the active calling thread to sleep until signaled to wake up. Each
* condition variable must be associated with a mutex, and that mutex must
* be locked before  calling this function, or the behavior will be
* undefined. As the calling thread is put to sleep, the given mutex
* will be simultaneously released; and as this thread wakes up the lock
* is again simultaneously acquired.
* @param cond the condition variable on which to block.
* @param mutex the mutex that must be locked upon entering this function,
*        is released while the thread is asleep, and is again acquired before
*        returning from this function.
*/
BASE_DECLARE(base_status_t) base_thread_cond_wait(base_thread_cond_t *cond, base_mutex_t *mutex);

/**
* Put the active calling thread to sleep until signaled to wake up or
* the timeout is reached. Each condition variable must be associated
* with a mutex, and that mutex must be locked before calling this
* function, or the behavior will be undefined. As the calling thread
* is put to sleep, the given mutex will be simultaneously released;
* and as this thread wakes up the lock is again simultaneously acquired.
* @param cond the condition variable on which to block.
* @param mutex the mutex that must be locked upon entering this function,
*        is released while the thread is asleep, and is again acquired before
*        returning from this function.
* @param timeout The amount of time in microseconds to wait. This is
*        a maximum, not a minimum. If the condition is signaled, we
*        will wake up before this time, otherwise the error APR_TIMEUP
*        is returned.
*/
BASE_DECLARE(base_status_t) base_thread_cond_timedwait(base_thread_cond_t *cond, base_mutex_t *mutex, base_interval_time_t timeout);

/**
* Signals a single thread, if one exists, that is blocking on the given
* condition variable. That thread is then scheduled to wake up and acquire
* the associated mutex. Although it is not required, if predictable scheduling
* is desired, that mutex must be locked while calling this function.
* @param cond the condition variable on which to produce the signal.
*/
BASE_DECLARE(base_status_t) base_thread_cond_signal(base_thread_cond_t *cond);

/**
* Signals all threads blocking on the given condition variable.
* Each thread that was signaled is then scheduled to wake up and acquire
* the associated mutex. This will happen in a serialized manner.
* @param cond the condition variable on which to produce the broadcast.
*/
BASE_DECLARE(base_status_t) base_thread_cond_broadcast(base_thread_cond_t *cond);

/**
* Destroy the condition variable and free the associated memory.
* @param cond the condition variable to destroy.
*/
BASE_DECLARE(base_status_t) base_thread_cond_destroy(base_thread_cond_t *cond);

/** @} */

/**
* @defgroup base_UUID UUID Handling
* @ingroup base_apr
* @{
*/

/** we represent a UUID as a block of 16 bytes. */

typedef struct {
    unsigned char data[16];
    /**< the actual UUID */
} base_uuid_t;

/** UUIDs are formatted as: 00112233-4455-6677-8899-AABBCCDDEEFF */
#define BASE_UUID_FORMATTED_LENGTH 256

#define BASE_MD5_DIGESTSIZE 16
#define BASE_MD5_DIGEST_STRING_SIZE 33

/**
* Format a UUID into a string, following the standard format
* @param buffer The buffer to place the formatted UUID string into. It must
*               be at least APR_UUID_FORMATTED_LENGTH + 1 bytes long to hold
*               the formatted UUID and a null terminator
* @param uuid The UUID to format
*/
BASE_DECLARE(void) base_uuid_format(char *buffer, const base_uuid_t *uuid);

/**
* Generate and return a (new) UUID
* @param uuid The resulting UUID
*/
BASE_DECLARE(void) base_uuid_get(base_uuid_t *uuid);

/**
* Parse a standard-format string into a UUID
* @param uuid The resulting UUID
* @param uuid_str The formatted UUID
*/
BASE_DECLARE(base_status_t) base_uuid_parse(base_uuid_t *uuid, const char *uuid_str);

/**
* MD5 in one step
* @param digest The final MD5 digest
* @param input The message block to use
* @param inputLen The length of the message block
*/
BASE_DECLARE(base_status_t) base_md5(unsigned char digest[BASE_MD5_DIGESTSIZE], const void *input, base_size_t inputLen);
BASE_DECLARE(base_status_t) base_md5_string(char digest_str[BASE_MD5_DIGEST_STRING_SIZE], const void *input, base_size_t inputLen);

/** @} */

/**
* @defgroup base_FIFO Thread Safe FIFO bounded queue
* @ingroup base_apr
* @{
*/

/** Opaque structure used for queue API */
typedef struct base_apr_queue_t base_queue_t;

/**
* create a FIFO queue
* @param queue The new queue
* @param queue_capacity maximum size of the queue
* @param pool a pool to allocate queue from
*/
BASE_DECLARE(base_status_t) base_queue_create(base_queue_t ** queue, unsigned int queue_capacity, base_memory_pool_t *pool);

/**
* pop/get an object from the queue, blocking if the queue is already empty
*
* @param queue the queue
* @param data the data
* @returns APR_EINTR the blocking was interrupted (try again)
* @returns APR_EOF if the queue has been terminated
* @returns APR_SUCCESS on a successfull pop
*/
BASE_DECLARE(base_status_t) base_queue_pop(base_queue_t *queue, void **data);

/**
* pop/get an object from the queue, blocking if the queue is already empty
*
* @param queue the queue
* @param data the data
* @param timeout The amount of time in microseconds to wait. This is
*        a maximum, not a minimum. If the condition is signaled, we
*        will wake up before this time, otherwise the error APR_TIMEUP
*        is returned.
* @returns APR_TIMEUP the request timed out
* @returns APR_EINTR the blocking was interrupted (try again)
* @returns APR_EOF if the queue has been terminated
* @returns APR_SUCCESS on a successfull pop
*/
BASE_DECLARE(base_status_t) base_queue_pop_timeout(base_queue_t *queue, void **data, base_interval_time_t timeout);

/**
* push/add a object to the queue, blocking if the queue is already full
*
* @param queue the queue
* @param data the data
* @returns APR_EINTR the blocking was interrupted (try again)
* @returns APR_EOF the queue has been terminated
* @returns APR_SUCCESS on a successfull push
*/
BASE_DECLARE(base_status_t) base_queue_push(base_queue_t *queue, void *data);

/**
* returns the size of the queue.
*
* @warning this is not threadsafe, and is intended for reporting/monitoring
* of the queue.
* @param queue the queue
* @returns the size of the queue
*/
BASE_DECLARE(unsigned int) base_queue_size(base_queue_t *queue);

/**
* pop/get an object to the queue, returning immediatly if the queue is empty
*
* @param queue the queue
* @param data the data
* @returns APR_EINTR the blocking operation was interrupted (try again)
* @returns APR_EAGAIN the queue is empty
* @returns APR_EOF the queue has been terminated
* @returns APR_SUCCESS on a successfull push
*/
BASE_DECLARE(base_status_t) base_queue_trypop(base_queue_t *queue, void **data);

BASE_DECLARE(base_status_t) base_queue_interrupt_all(base_queue_t *queue);

BASE_DECLARE(base_status_t) base_queue_term(base_queue_t *queue);

/**
* push/add a object to the queue, returning immediatly if the queue is full
*
* @param queue the queue
* @param data the data
* @returns APR_EINTR the blocking operation was interrupted (try again)
* @returns APR_EAGAIN the queue is full
* @returns APR_EOF the queue has been terminated
* @returns APR_SUCCESS on a successfull push
*/
BASE_DECLARE(base_status_t) base_queue_trypush(base_queue_t *queue, void *data);

/** @} */

/**
* @defgroup base_file_io File I/O Handling Functions
* @ingroup base_apr
* @{
*/

/** Structure for referencing files. */
typedef struct fspr_file_t base_file_t;

typedef int32_t base_fileperms_t;
typedef int base_seek_where_t;

/**
* @defgroup fspr_file_seek_flags File Seek Flags
* @{
*/

/* flags for fspr_file_seek */
/** Set the file position */
#define BASE_SEEK_SET SEEK_SET
/** Current */
#define BASE_SEEK_CUR SEEK_CUR
/** Go to end of file */
#define BASE_SEEK_END SEEK_END
/** @} */


/**
* @defgroup base_file_permissions File Permissions flags
* @ingroup base_file_io
* @{
*/

#define BASE_FPROT_USETID 0x8000			/**< Set user id */
#define BASE_FPROT_UREAD 0x0400			/**< Read by user */
#define BASE_FPROT_UWRITE 0x0200			/**< Write by user */
#define BASE_FPROT_UEXECUTE 0x0100		/**< Execute by user */

#define BASE_FPROT_GSETID 0x4000			/**< Set group id */
#define BASE_FPROT_GREAD 0x0040			/**< Read by group */
#define BASE_FPROT_GWRITE 0x0020			/**< Write by group */
#define BASE_FPROT_GEXECUTE 0x0010		/**< Execute by group */

#define BASE_FPROT_WSTICKY 0x2000
#define BASE_FPROT_WREAD 0x0004			/**< Read by others */
#define BASE_FPROT_WWRITE 0x0002			/**< Write by others */
#define BASE_FPROT_WEXECUTE 0x0001		/**< Execute by others */

#define BASE_FPROT_OS_DEFAULT 0x0FFF		/**< use OS's default permissions */

/* additional permission flags for fspr_file_copy  and fspr_file_append */
#define BASE_FPROT_FILE_SOURCE_PERMS 0x1000	/**< Copy source file's permissions */
/** @} */

/* File lock types/flags */
/**
* @defgroup base_file_lock_types File Lock Types
* @{
*/

#define BASE_FLOCK_SHARED        1	   /**< Shared lock. More than one process
or thread can hold a shared lock
at any given time. Essentially,
this is a "read lock", preventing
writers from establishing an
exclusive lock. */
#define BASE_FLOCK_EXCLUSIVE     2	   /**< Exclusive lock. Only one process
may hold an exclusive lock at any
given time. This is analogous to
a "write lock". */

#define BASE_FLOCK_TYPEMASK      0x000F  /**< mask to extract lock type */
#define BASE_FLOCK_NONBLOCK      0x0010  /**< do not block while acquiring the
file lock */

/** @} */

/**
* @defgroup base_file_open_flags File Open Flags/Routines
* @ingroup base_file_io
* @{
*/
#define BASE_FOPEN_READ				0x00001		/**< Open the file for reading */
#define BASE_FOPEN_WRITE				0x00002		/**< Open the file for writing */
#define BASE_FOPEN_CREATE				0x00004		/**< Create the file if not there */
#define BASE_FOPEN_APPEND				0x00008		/**< Append to the end of the file */
#define BASE_FOPEN_TRUNCATE			0x00010		/**< Open the file and truncate to 0 length */
#define BASE_FOPEN_BINARY				0x00020		/**< Open the file in binary mode */
#define BASE_FOPEN_EXCL				0x00040		/**< Open should fail if APR_CREATE and file exists. */
#define BASE_FOPEN_BUFFERED			0x00080		/**< Open the file for buffered I/O */
#define BASE_FOPEN_DELONCLOSE			0x00100		/**< Delete the file after close */
#define BASE_FOPEN_XTHREAD			0x00200		/**< Platform dependent tag to open the file for use across multiple threads */
#define BASE_FOPEN_SHARELOCK			0x00400		/**< Platform dependent support for higher level locked read/write access to support writes across process/machines */
#define BASE_FOPEN_NOCLEANUP			0x00800		/**< Do not register a cleanup when the file is opened */
#define BASE_FOPEN_SENDFILE_ENABLED	0x01000		/**< Advisory flag that this file should support fspr_socket_sendfile operation */
#define BASE_FOPEN_LARGEFILE			0x04000		/**< Platform dependent flag to enable large file support */
/** @} */

/**
* Open the specified file.
* @param newf The opened file descriptor.
* @param fname The full path to the file (using / on all systems)
* @param flag Or'ed value of:
* <PRE>
*         BASE_FOPEN_READ				open for reading
*         BASE_FOPEN_WRITE				open for writing
*         BASE_FOPEN_CREATE				create the file if not there
*         BASE_FOPEN_APPEND				file ptr is set to end prior to all writes
*         BASE_FOPEN_TRUNCATE			set length to zero if file exists
*         BASE_FOPEN_BINARY				not a text file (This flag is ignored on
*											UNIX because it has no meaning)
*         BASE_FOPEN_BUFFERED			buffer the data.  Default is non-buffered
*         BASE_FOPEN_EXCL				return error if APR_CREATE and file exists
*         BASE_FOPEN_DELONCLOSE			delete the file after closing.
*         BASE_FOPEN_XTHREAD				Platform dependent tag to open the file
*											for use across multiple threads
*         BASE_FOPEN_SHARELOCK			Platform dependent support for higher
*											level locked read/write access to support
*											writes across process/machines
*         BASE_FOPEN_NOCLEANUP			Do not register a cleanup with the pool
*											passed in on the <EM>pool</EM> argument (see below).
*											The fspr_os_file_t handle in fspr_file_t will not
*											be closed when the pool is destroyed.
*         BASE_FOPEN_SENDFILE_ENABLED	Open with appropriate platform semantics
*											for sendfile operations.  Advisory only,
*											fspr_socket_sendfile does not check this flag.
* </PRE>
* @param perm Access permissions for file.
* @param pool The pool to use.
* @remark If perm is BASE_FPROT_OS_DEFAULT and the file is being created,
* appropriate default permissions will be used.
*/
BASE_DECLARE(base_status_t) base_file_open(base_file_t ** newf, const char *fname, int32_t flag, base_fileperms_t perm,
    base_memory_pool_t *pool);


BASE_DECLARE(base_status_t) base_file_seek(base_file_t *thefile, base_seek_where_t where, int64_t *offset);


BASE_DECLARE(base_status_t) base_file_copy(const char *from_path, const char *to_path, base_fileperms_t perms, base_memory_pool_t *pool);

/**
* Close the specified file.
* @param thefile The file descriptor to close.
*/
BASE_DECLARE(base_status_t) base_file_close(base_file_t *thefile);

BASE_DECLARE(base_status_t) base_file_trunc(base_file_t *thefile, int64_t offset);

BASE_DECLARE(base_status_t) base_file_lock(base_file_t *thefile, int type);

/**
* Delete the specified file.
* @param path The full path to the file (using / on all systems)
* @param pool The pool to use.
* @remark If the file is open, it won't be removed until all
* instances are closed.
*/
BASE_DECLARE(base_status_t) base_file_remove(const char *path, base_memory_pool_t *pool);

BASE_DECLARE(base_status_t) base_file_rename(const char *from_path, const char *to_path, base_memory_pool_t *pool);

/**
* Read data from the specified file.
* @param thefile The file descriptor to read from.
* @param buf The buffer to store the data to.
* @param nbytes On entry, the number of bytes to read; on exit, the number
* of bytes read.
*
* @remark fspr_file_read will read up to the specified number of
* bytes, but never more.  If there isn't enough data to fill that
* number of bytes, all of the available data is read.  The third
* argument is modified to reflect the number of bytes read.  If a
* char was put back into the stream via ungetc, it will be the first
* character returned.
*
* @remark It is not possible for both bytes to be read and an APR_EOF
* or other error to be returned.  APR_EINTR is never returned.
*/
BASE_DECLARE(base_status_t) base_file_read(base_file_t *thefile, void *buf, base_size_t *nbytes);

/**
* Write data to the specified file.
* @param thefile The file descriptor to write to.
* @param buf The buffer which contains the data.
* @param nbytes On entry, the number of bytes to write; on exit, the number
*               of bytes written.
*
* @remark fspr_file_write will write up to the specified number of
* bytes, but never more.  If the OS cannot write that many bytes, it
* will write as many as it can.  The third argument is modified to
* reflect the * number of bytes written.
*
* @remark It is possible for both bytes to be written and an error to
* be returned.  APR_EINTR is never returned.
*/
BASE_DECLARE(base_status_t) base_file_write(base_file_t *thefile, const void *buf, base_size_t *nbytes);
BASE_DECLARE(int) base_file_printf(base_file_t *thefile, const char *format, ...);

BASE_DECLARE(base_status_t) base_file_mktemp(base_file_t ** thefile, char *templ, int32_t flags, base_memory_pool_t *pool);

BASE_DECLARE(base_size_t) base_file_get_size(base_file_t *thefile);

BASE_DECLARE(base_status_t) base_file_exists(const char *filename, base_memory_pool_t *pool);

BASE_DECLARE(base_status_t) base_directory_exists(const char *dirname, base_memory_pool_t *pool);

/**
* Create a new directory on the file system.
* @param path the path for the directory to be created. (use / on all systems)
* @param perm Permissions for the new direcoty.
* @param pool the pool to use.
*/
BASE_DECLARE(base_status_t) base_dir_make(const char *path, base_fileperms_t perm, base_memory_pool_t *pool);

/** Creates a new directory on the file system, but behaves like
* 'mkdir -p'. Creates intermediate directories as required. No error
* will be reported if PATH already exists.
* @param path the path for the directory to be created. (use / on all systems)
* @param perm Permissions for the new direcoty.
* @param pool the pool to use.
*/
BASE_DECLARE(base_status_t) base_dir_make_recursive(const char *path, base_fileperms_t perm, base_memory_pool_t *pool);

typedef struct base_dir base_dir_t;

struct base_array_header_t {
    /** The pool the array is allocated out of */
    base_memory_pool_t *pool;
    /** The amount of memory allocated for each element of the array */
    int elt_size;
    /** The number of active elements in the array */
    int nelts;
    /** The number of elements allocated in the array */
    int nalloc;
    /** The elements in the array */
    char *elts;
};
typedef struct base_array_header_t base_array_header_t;

BASE_DECLARE(base_status_t) base_dir_open(base_dir_t ** new_dir, const char *dirname, base_memory_pool_t *pool);
BASE_DECLARE(base_status_t) base_dir_close(base_dir_t *thedir);
BASE_DECLARE(const char *) base_dir_next_file(base_dir_t *thedir, char *buf, base_size_t len);
BASE_DECLARE(uint32_t) base_dir_count(base_dir_t *thedir);

/** @} */

/**
* @defgroup base_thread_proc Threads and Process Functions
* @ingroup base_apr
* @{
*/

/** Opaque Thread structure. */
typedef struct fspr_thread_t base_thread_t;

/** Opaque Thread attributes structure. */
typedef struct fspr_threadattr_t base_threadattr_t;

/**
* The prototype for any APR thread worker functions.
* typedef void *(BASE_THREAD_FUNC *base_thread_start_t)(base_thread_t*, void*);
*/
typedef void *(BASE_THREAD_FUNC * base_thread_start_t) (base_thread_t *, void *);

//APR_DECLARE(fspr_status_t) fspr_threadattr_stacksize_set(fspr_threadattr_t *attr, base_size_t stacksize)
BASE_DECLARE(base_status_t) base_threadattr_stacksize_set(base_threadattr_t *attr, base_size_t stacksize);

BASE_DECLARE(base_status_t) base_threadattr_priority_set(base_threadattr_t *attr, base_thread_priority_t priority);


/**
* Create and initialize a new threadattr variable
* @param new_attr The newly created threadattr.
* @param pool The pool to use
*/
BASE_DECLARE(base_status_t) base_threadattr_create(base_threadattr_t ** new_attr, base_memory_pool_t *pool);

/**
* Set if newly created threads should be created in detached state.
* @param attr The threadattr to affect
* @param on Non-zero if detached threads should be created.
*/
BASE_DECLARE(base_status_t) base_threadattr_detach_set(base_threadattr_t *attr, int32_t on);

/**
* Create a new thread of execution
* @param new_thread The newly created thread handle.
* @param attr The threadattr to use to determine how to create the thread
* @param func The function to start the new thread in
* @param data Any data to be passed to the starting function
* @param cont The pool to use
*/
BASE_DECLARE(base_status_t) base_thread_create(base_thread_t ** new_thread, base_threadattr_t *attr,
    base_thread_start_t func, void *data, base_memory_pool_t *cont);

/** @} */

/**
* @defgroup base_network_io Network Routines
* @ingroup base_apr
* @{
*/

#define BASE_SO_LINGER 1
#define BASE_SO_KEEPALIVE 2
#define BASE_SO_DEBUG 4
#define BASE_SO_NONBLOCK 8
#define BASE_SO_REUSEADDR 16
#define BASE_SO_SNDBUF 64
#define BASE_SO_RCVBUF 128
#define BASE_SO_DISCONNECTED 256
#define BASE_SO_TCP_NODELAY 512
#define BASE_SO_TCP_KEEPIDLE 520
#define BASE_SO_TCP_KEEPINTVL 530
#define BASE_SO_IPV6_V6ONLY 16384 /* Don't accept IPv4 connections on an IPv6 listening socket. */

/**
* @def BASE_INET
* Not all platforms have these defined, so we'll define them here
* The default values come from FreeBSD 4.1.1
*/
#define BASE_INET     AF_INET
#ifdef AF_INET6
#define BASE_INET6    AF_INET6
#else
#define BASE_INET6 0
#endif

/** @def BASE_UNSPEC
* Let the system decide which address family to use
*/
#ifdef AF_UNSPEC
#define BASE_UNSPEC   AF_UNSPEC
#else
#define BASE_UNSPEC   0
#endif

/** A structure to represent sockets */
typedef struct fspr_socket_t base_socket_t;

/** Freebase's socket address type, used to ensure protocol independence */
typedef struct fspr_sockaddr_t base_sockaddr_t;

typedef enum {
    BASE_SHUTDOWN_READ,	   /**< no longer allow read request */
    BASE_SHUTDOWN_WRITE,	   /**< no longer allow write requests */
    BASE_SHUTDOWN_READWRITE /**< no longer allow read or write requests */
} base_shutdown_how_e;

/**
* @defgroup IP_Proto IP Protocol Definitions for use when creating sockets
* @{
*/
#define BASE_PROTO_TCP       6   /**< TCP  */
#define BASE_PROTO_UDP      17   /**< UDP  */
#define BASE_PROTO_SCTP    132   /**< SCTP */
/** @} */

/* function definitions */

/**
* Create a socket.
* @param new_sock The new socket that has been set up.
* @param family The address family of the socket (e.g., BASE_INET).
* @param type The type of the socket (e.g., SOCK_STREAM).
* @param protocol The protocol of the socket (e.g., BASE_PROTO_TCP).
* @param pool The pool to use
*/
BASE_DECLARE(base_status_t) base_socket_create(base_socket_t ** new_sock, int family, int type, int protocol, base_memory_pool_t *pool);

/**
* Shutdown either reading, writing, or both sides of a socket.
* @param sock The socket to close
* @param how How to shutdown the socket.  One of:
* <PRE>
*            BASE_SHUTDOWN_READ         no longer allow read requests
*            BASE_SHUTDOWN_WRITE        no longer allow write requests
*            BASE_SHUTDOWN_READWRITE    no longer allow read or write requests
* </PRE>
* @see base_shutdown_how_e
* @remark This does not actually close the socket descriptor, it just
*      controls which calls are still valid on the socket.
*/
BASE_DECLARE(base_status_t) base_socket_shutdown(base_socket_t *sock, base_shutdown_how_e how);

/**
* Close a socket.
* @param sock The socket to close
*/
BASE_DECLARE(base_status_t) base_socket_close(base_socket_t *sock);

/**
* Bind the socket to its associated port
* @param sock The socket to bind
* @param sa The socket address to bind to
* @remark This may be where we will find out if there is any other process
*      using the selected port.
*/
BASE_DECLARE(base_status_t) base_socket_bind(base_socket_t *sock, base_sockaddr_t *sa);

/**
* Listen to a bound socket for connections.
* @param sock The socket to listen on
* @param backlog The number of outstanding connections allowed in the sockets
*                listen queue.  If this value is less than zero, the listen
*                queue size is set to zero.
*/
BASE_DECLARE(base_status_t) base_socket_listen(base_socket_t *sock, int32_t backlog);

/**
* Accept a new connection request
* @param new_sock A copy of the socket that is connected to the socket that
*                 made the connection request.  This is the socket which should
*                 be used for all future communication.
* @param sock The socket we are listening on.
* @param pool The pool for the new socket.
*/
BASE_DECLARE(base_status_t) base_socket_accept(base_socket_t ** new_sock, base_socket_t *sock, base_memory_pool_t *pool);

/**
* Issue a connection request to a socket either on the same machine
* or a different one.
* @param sock The socket we wish to use for our side of the connection
* @param sa The address of the machine we wish to connect to.
*/
BASE_DECLARE(base_status_t) base_socket_connect(base_socket_t *sock, base_sockaddr_t *sa);

/**
* Get socket fd for the base socket passed
* @param sock The socket we wish to have fd
*/
BASE_DECLARE(int) base_socket_fd_get(base_socket_t *sock);

BASE_DECLARE(uint16_t) base_sockaddr_get_port(base_sockaddr_t *sa);
BASE_DECLARE(const char *) base_get_addr(char *buf, base_size_t len, base_sockaddr_t *in);
BASE_DECLARE(base_status_t) base_getnameinfo(char **hostname, base_sockaddr_t *sa, int32_t flags);
BASE_DECLARE(int32_t) base_sockaddr_get_family(base_sockaddr_t *sa);
BASE_DECLARE(base_status_t) base_sockaddr_ip_get(char **addr, base_sockaddr_t *sa);
BASE_DECLARE(int) base_sockaddr_equal(const base_sockaddr_t *sa1, const base_sockaddr_t *sa2);


/**
* Create fspr_sockaddr_t from hostname, address family, and port.
* @param sa The new fspr_sockaddr_t.
* @param hostname The hostname or numeric address string to resolve/parse, or
*               NULL to build an address that corresponds to 0.0.0.0 or ::
* @param family The address family to use, or BASE_UNSPEC if the system should
*               decide.
* @param port The port number.
* @param flags Special processing flags:
* <PRE>
*       APR_IPV4_ADDR_OK          first query for IPv4 addresses; only look
*                                 for IPv6 addresses if the first query failed;
*                                 only valid if family is APR_UNSPEC and hostname
*                                 isn't NULL; mutually exclusive with
*                                 APR_IPV6_ADDR_OK
*       APR_IPV6_ADDR_OK          first query for IPv6 addresses; only look
*                                 for IPv4 addresses if the first query failed;
*                                 only valid if family is APR_UNSPEC and hostname
*                                 isn't NULL and APR_HAVE_IPV6; mutually exclusive
*                                 with APR_IPV4_ADDR_OK
* </PRE>
* @param pool The pool for the fspr_sockaddr_t and associated storage.
*/
BASE_DECLARE(base_status_t) base_sockaddr_info_get(base_sockaddr_t ** sa, const char *hostname,
    int32_t family, base_port_t port, int32_t flags, base_memory_pool_t *pool);

BASE_DECLARE(base_status_t) base_sockaddr_create(base_sockaddr_t **sa, base_memory_pool_t *pool);

BASE_DECLARE(base_status_t) base_sockaddr_new(base_sockaddr_t ** sa, const char *ip, base_port_t port, base_memory_pool_t *pool);

/**
* Send data over a network.
* @param sock The socket to send the data over.
* @param buf The buffer which contains the data to be sent.
* @param len On entry, the number of bytes to send; on exit, the number
*            of bytes sent.
* @remark
* <PRE>
* This functions acts like a blocking write by default.  To change
* this behavior, use fspr_socket_timeout_set() or the APR_SO_NONBLOCK
* socket option.
*
* It is possible for both bytes to be sent and an error to be returned.
*
* APR_EINTR is never returned.
* </PRE>
*/
BASE_DECLARE(base_status_t) base_socket_send(base_socket_t *sock, const char *buf, base_size_t *len);

/**
* @param sock The socket to send from
* @param where The fspr_sockaddr_t describing where to send the data
* @param flags The flags to use
* @param buf  The data to send
* @param len  The length of the data to send
*/
BASE_DECLARE(base_status_t) base_socket_sendto(base_socket_t *sock, base_sockaddr_t *where, int32_t flags, const char *buf,
    base_size_t *len);

BASE_DECLARE(base_status_t) base_socket_send_nonblock(base_socket_t *sock, const char *buf, base_size_t *len);

/**
* @param from The fspr_sockaddr_t to fill in the recipient info
* @param sock The socket to use
* @param flags The flags to use
* @param buf  The buffer to use
* @param len  The length of the available buffer
*
*/
BASE_DECLARE(base_status_t) base_socket_recvfrom(base_sockaddr_t *from, base_socket_t *sock, int32_t flags, char *buf, size_t *len);

BASE_DECLARE(base_status_t) base_socket_atmark(base_socket_t *sock, int *atmark);

/**
* Read data from a network.
* @param sock The socket to read the data from.
* @param buf The buffer to store the data in.
* @param len On entry, the number of bytes to receive; on exit, the number
*            of bytes received.
* @remark
* <PRE>
* This functions acts like a blocking read by default.  To change
* this behavior, use fspr_socket_timeout_set() or the APR_SO_NONBLOCK
* socket option.
* The number of bytes actually received is stored in argument 3.
*
* It is possible for both bytes to be received and an APR_EOF or
* other error to be returned.
*
* APR_EINTR is never returned.
* </PRE>
*/
BASE_DECLARE(base_status_t) base_socket_recv(base_socket_t *sock, char *buf, base_size_t *len);

/**
* Setup socket options for the specified socket
* @param sock The socket to set up.
* @param opt The option we would like to configure.  One of:
* <PRE>
*            APR_SO_DEBUG      --  turn on debugging information
*            APR_SO_KEEPALIVE  --  keep connections active
*            APR_SO_LINGER     --  lingers on close if data is present
*            APR_SO_NONBLOCK   --  Turns blocking on/off for socket
*                                  When this option is enabled, use
*                                  the APR_STATUS_IS_EAGAIN() macro to
*                                  see if a send or receive function
*                                  could not transfer data without
*                                  blocking.
*            APR_SO_REUSEADDR  --  The rules used in validating addresses
*                                  supplied to bind should allow reuse
*                                  of local addresses.
*            APR_SO_SNDBUF     --  Set the SendBufferSize
*            APR_SO_RCVBUF     --  Set the ReceiveBufferSize
* </PRE>
* @param on Value for the option.
*/
BASE_DECLARE(base_status_t) base_socket_opt_set(base_socket_t *sock, int32_t opt, int32_t on);

/**
* Query socket timeout for the specified socket
* @param sock The socket to query
* @param t Socket timeout returned from the query.
* <PRE>
*   t > 0  -- read and write calls return APR_TIMEUP if specified time
*             elapsess with no data read or written
*   t == 0 -- read and write calls never block
*   t < 0  -- read and write calls block
* </PRE>
*/
BASE_DECLARE(base_status_t) base_socket_timeout_get(base_socket_t *sock, base_interval_time_t *t);

/**
* Setup socket timeout for the specified socket
* @param sock The socket to set up.
* @param t Value for the timeout.
* <PRE>
*   t > 0  -- read and write calls return APR_TIMEUP if specified time
*             elapsess with no data read or written
*   t == 0 -- read and write calls never block
*   t < 0  -- read and write calls block
* </PRE>
*/
BASE_DECLARE(base_status_t) base_socket_timeout_set(base_socket_t *sock, base_interval_time_t t);

/**
* Join a Multicast Group
* @param sock The socket to join a multicast group
* @param join The address of the multicast group to join
* @param iface Address of the interface to use.  If NULL is passed, the
*              default multicast interface will be used. (OS Dependent)
* @param source Source Address to accept transmissions from (non-NULL
*               implies Source-Specific Multicast)
*/
BASE_DECLARE(base_status_t) base_mcast_join(base_socket_t *sock, base_sockaddr_t *join, base_sockaddr_t *iface, base_sockaddr_t *source);

/**
* Set the Multicast Time to Live (ttl) for a multicast transmission.
* @param sock The socket to set the multicast ttl
* @param ttl Time to live to Assign. 0-255, default=1
* @remark If the TTL is 0, packets will only be seen by sockets on the local machine,
*     and only when multicast loopback is enabled.
*/
BASE_DECLARE(base_status_t) base_mcast_hops(base_socket_t *sock, uint8_t ttl);

BASE_DECLARE(base_status_t) base_mcast_loopback(base_socket_t *sock, uint8_t opt);
BASE_DECLARE(base_status_t) base_mcast_interface(base_socket_t *sock, base_sockaddr_t *iface);

/** @} */

typedef enum {
    BASE_NO_DESC,		   /**< nothing here */
    BASE_POLL_SOCKET,	   /**< descriptor refers to a socket */
    BASE_POLL_FILE,		   /**< descriptor refers to a file */
    BASE_POLL_LASTDESC	   /**< descriptor is the last one in the list */
} base_pollset_type_t;

typedef union {
    base_file_t *f;		   /**< file */
    base_socket_t *s;	   /**< socket */
} base_descriptor_t;

struct base_pollfd {
    base_memory_pool_t *p;		  /**< associated pool */
    base_pollset_type_t desc_type;
    /**< descriptor type */
    int16_t reqevents;	/**< requested events */
    int16_t rtnevents;	/**< returned events */
    base_descriptor_t desc;	 /**< @see fspr_descriptor */
    void *client_data;		/**< allows app to associate context */
};



/**
* @defgroup fspr_poll Poll Routines
* @ingroup base_apr
* @{
*/
/** Poll descriptor set. */
typedef struct base_pollfd base_pollfd_t;

/** Opaque structure used for pollset API */
typedef struct fspr_pollset_t base_pollset_t;

/**
* Poll options
*/
#define BASE_POLLIN 0x001			/**< Can read without blocking */
#define BASE_POLLPRI 0x002			/**< Priority data available */
#define BASE_POLLOUT 0x004			/**< Can write without blocking */
#define BASE_POLLERR 0x010			/**< Pending error */
#define BASE_POLLHUP 0x020			/**< Hangup occurred */
#define BASE_POLLNVAL 0x040		/**< Descriptior invalid */

/**
* Setup a pollset object
* @param pollset  The pointer in which to return the newly created object
* @param size The maximum number of descriptors that this pollset can hold
* @param pool The pool from which to allocate the pollset
* @param flags Optional flags to modify the operation of the pollset.
*
* @remark If flags equals APR_POLLSET_THREADSAFE, then a pollset is
* created on which it is safe to make concurrent calls to
* fspr_pollset_add(), fspr_pollset_remove() and fspr_pollset_poll() from
* separate threads.  This feature is only supported on some
* platforms; the fspr_pollset_create() call will fail with
* APR_ENOTIMPL on platforms where it is not supported.
*/
BASE_DECLARE(base_status_t) base_pollset_create(base_pollset_t ** pollset, uint32_t size, base_memory_pool_t *pool, uint32_t flags);

/**
* Add a socket or file descriptor to a pollset
* @param pollset The pollset to which to add the descriptor
* @param descriptor The descriptor to add
* @remark If you set client_data in the descriptor, that value
*         will be returned in the client_data field whenever this
*         descriptor is signalled in fspr_pollset_poll().
* @remark If the pollset has been created with APR_POLLSET_THREADSAFE
*         and thread T1 is blocked in a call to fspr_pollset_poll() for
*         this same pollset that is being modified via fspr_pollset_add()
*         in thread T2, the currently executing fspr_pollset_poll() call in
*         T1 will either: (1) automatically include the newly added descriptor
*         in the set of descriptors it is watching or (2) return immediately
*         with APR_EINTR.  Option (1) is recommended, but option (2) is
*         allowed for implementations where option (1) is impossible
*         or impractical.
*/
BASE_DECLARE(base_status_t) base_pollset_add(base_pollset_t *pollset, const base_pollfd_t *descriptor);

/**
* Remove a descriptor from a pollset
* @param pollset The pollset from which to remove the descriptor
* @param descriptor The descriptor to remove
* @remark If the pollset has been created with APR_POLLSET_THREADSAFE
*         and thread T1 is blocked in a call to fspr_pollset_poll() for
*         this same pollset that is being modified via fspr_pollset_remove()
*         in thread T2, the currently executing fspr_pollset_poll() call in
*         T1 will either: (1) automatically exclude the newly added descriptor
*         in the set of descriptors it is watching or (2) return immediately
*         with APR_EINTR.  Option (1) is recommended, but option (2) is
*         allowed for implementations where option (1) is impossible
*         or impractical.
*/
BASE_DECLARE(base_status_t) base_pollset_remove(base_pollset_t *pollset, const base_pollfd_t *descriptor);

/**
* Poll the sockets in the poll structure
* @param aprset The poll structure we will be using.
* @param numsock The number of sockets we are polling
* @param nsds The number of sockets signalled.
* @param timeout The amount of time in microseconds to wait.  This is
*                a maximum, not a minimum.  If a socket is signalled, we
*                will wake up before this time.  A negative number means
*                wait until a socket is signalled.
* @remark The number of sockets signalled is returned in the third argument.
*         This is a blocking call, and it will not return until either a
*         socket has been signalled, or the timeout has expired.
*/
BASE_DECLARE(base_status_t) base_poll(base_pollfd_t *aprset, int32_t numsock, int32_t *nsds, base_interval_time_t timeout);

/**
* Block for activity on the descriptor(s) in a pollset
* @param pollset The pollset to use
* @param timeout Timeout in microseconds
* @param num Number of signalled descriptors (output parameter)
* @param descriptors Array of signalled descriptors (output parameter)
*/
BASE_DECLARE(base_status_t) base_pollset_poll(base_pollset_t *pollset, base_interval_time_t timeout, int32_t *num, const base_pollfd_t **descriptors);

/*!
\brief Create a set of file descriptors to poll from a socket
\param poll the polfd to create
\param sock the socket to add
\param flags the flags to modify the behaviour
\param pool the memory pool to use
\return BASE_STATUS_SUCCESS when successful
*/
BASE_DECLARE(base_status_t) base_socket_create_pollset(base_pollfd_t ** poll, base_socket_t *sock, int16_t flags, base_memory_pool_t *pool);

BASE_DECLARE(base_interval_time_t) base_interval_time_from_timeval(struct timeval *tvp);


/*!
\brief Create a pollfd out of a socket
\param pollfd the pollfd to create
\param sock the socket to add
\param flags the flags to modify the behaviour
\param client_data custom user data
\param pool the memory pool to use
\return BASE_STATUS_SUCCESS when successful
*/
BASE_DECLARE(base_status_t) base_socket_create_pollfd(base_pollfd_t **pollfd, base_socket_t *sock, int16_t flags, void *client_data, base_memory_pool_t *pool);
BASE_DECLARE(base_status_t) base_match_glob(const char *pattern, base_array_header_t ** result, base_memory_pool_t *pool);
BASE_DECLARE(base_status_t) base_os_sock_get(base_os_socket_t *thesock, base_socket_t *sock);
BASE_DECLARE(base_status_t) base_os_sock_put(base_socket_t **sock, base_os_socket_t *thesock, base_memory_pool_t *pool);
BASE_DECLARE(base_status_t) base_socket_addr_get(base_sockaddr_t ** sa, base_bool_t remote, base_socket_t *sock);
/**
* Create an anonymous pipe.
* @param in The file descriptor to use as input to the pipe.
* @param out The file descriptor to use as output from the pipe.
* @param pool The pool to operate on.
*/
BASE_DECLARE(base_status_t) base_file_pipe_create(base_file_t ** in, base_file_t ** out, base_memory_pool_t *pool);

/**
* Get the timeout value for a pipe or manipulate the blocking state.
* @param thepipe The pipe we are getting a timeout for.
* @param timeout The current timeout value in microseconds.
*/
BASE_DECLARE(base_status_t) base_file_pipe_timeout_get(base_file_t *thepipe, base_interval_time_t *timeout);

/**
* Set the timeout value for a pipe or manipulate the blocking state.
* @param thepipe The pipe we are setting a timeout on.
* @param timeout The timeout value in microseconds.  Values < 0 mean wait
*        forever, 0 means do not wait at all.
*/
BASE_DECLARE(base_status_t) base_file_pipe_timeout_set(base_file_t *thepipe, base_interval_time_t timeout);


/**
* stop the current thread
* @param thd The thread to stop
* @param retval The return value to pass back to any thread that cares
*/
BASE_DECLARE(base_status_t) base_thread_exit(base_thread_t *thd, base_status_t retval);

/**
* block until the desired thread stops executing.
* @param retval The return value from the dead thread.
* @param thd The thread to join
*/
BASE_DECLARE(base_status_t) base_thread_join(base_status_t *retval, base_thread_t *thd);

/**
* Return a human readable string describing the specified error.
* @param statcode The error code the get a string for.
* @param buf A buffer to hold the error string.
* @bufsize Size of the buffer to hold the string.
*/

BASE_DECLARE(char *) base_strerror(base_status_t statcode, char *buf, base_size_t bufsize);



/** @} */

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
