/*
* FreeBASE Modular Media Switching Software Library / Soft-Switch Application
* Copyright (C) 2005-2015, Anthony Minessale II <anthm@freebase.org>
*
* Version: MPL 1.1
*
* The contents of this file are subject to the Mozilla Public License Version
* 1.1 (the "License"); you may not use this file except in compliance with
* the License. You may obtain a copy of the License at
* http://www.mozilla.org/MPL/F
*
* Software distributed under the License is distributed on an "AS IS" basis,
* WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
* for the specific language governing rights and limitations under the
* License.
*
* The Original Code is FreeBASE Modular Media Switching Software Library / Soft-Switch Application
*
* The Initial Developer of the Original Code is
* Michael Jerris <mike@jerris.com>
* Portions created by the Initial Developer are Copyright (C)
* the Initial Developer. All Rights Reserved.
*
* Contributor(s):
*
* Michael Jerris <mike@jerris.com>
* Eliot Gable <egable@gmail.com>
* William King <william.king@quentustech.com>
*
* base_apr.c -- apr wrappers and extensions
*
*/

#include <base.h>
#ifndef WIN32
#include <base_private.h>
#endif

/* apr headers*/
#include <fspr.h>
#include <fspr_atomic.h>
#include <fspr_pools.h>
#include <fspr_hash.h>
#include <fspr_network_io.h>
#include <fspr_errno.h>
#include <fspr_thread_proc.h>
#include <fspr_portable.h>
#include <fspr_thread_mutex.h>
#include <fspr_thread_cond.h>
#include <fspr_thread_rwlock.h>
#include <fspr_file_io.h>
#include <fspr_poll.h>
#include <fspr_strings.h>
#define APR_WANT_STDIO
#define APR_WANT_STRFUNC
#include <fspr_want.h>
#include <fspr_file_info.h>
#include <fspr_fnmatch.h>
#include <fspr_tables.h>

#ifdef WIN32
#include "fspr_arch_networkio.h"
/* Missing socket symbols */
#ifndef SOL_TCP
#define SOL_TCP IPPROTO_TCP
#endif
#endif

/* fspr_vformatter_buff_t definition*/
#include <fspr_lib.h>

#ifndef WIN32
#include <uuid/uuid.h>
#endif

/* apr stubs */

BASE_DECLARE(int) base_status_is_timeup(int status)
{
    return APR_STATUS_IS_TIMEUP(status);
}

/* Memory Pools */

BASE_DECLARE(base_thread_id_t) base_thread_self(void)
{
#ifndef WIN32
    return fspr_os_thread_current();
#else
    return (base_thread_id_t) (GetCurrentThreadId());
#endif
}

BASE_DECLARE(int) base_thread_equal(base_thread_id_t tid1, base_thread_id_t tid2)
{
#ifdef WIN32
    return (tid1 == tid2);
#else
    return fspr_os_thread_equal(tid1, tid2);
#endif

}

BASE_DECLARE(unsigned int) base_ci_hashfunc_default(const char *char_key, base_ssize_t *klen)
{
    unsigned int hash = 0;
    const unsigned char *key = (const unsigned char *) char_key;
    const unsigned char *p;
    fspr_ssize_t i;

    if (*klen == APR_HASH_KEY_STRING) {
        for (p = key; *p; p++) {
            hash = hash * 33 + tolower(*p);
        }
        *klen = p - key;
    } else {
        for (p = key, i = *klen; i; i--, p++) {
            hash = hash * 33 + tolower(*p);
        }
    }

    return hash;
}


BASE_DECLARE(unsigned int) base_hashfunc_default(const char *key, base_ssize_t *klen)
{
    return fspr_hashfunc_default(key, klen);
}

/* string functions */

BASE_DECLARE(base_status_t) base_strftime(char *s, base_size_t *retsize, base_size_t max, const char *format, base_time_exp_t *tm)
{
    const char *p = format;

    if (!p)
        return BASE_STATUS_FALSE;

    while (*p) {
        if (*p == '%') {
            switch (*(++p)) {
            case 'C':
            case 'D':
            case 'r':
            case 'R':
            case 'T':
            case 'e':
            case 'a':
            case 'A':
            case 'b':
            case 'B':
            case 'c':
            case 'd':
            case 'H':
            case 'I':
            case 'j':
            case 'm':
            case 'M':
            case 'p':
            case 'S':
            case 'U':
            case 'w':
            case 'W':
            case 'x':
            case 'X':
            case 'y':
            case 'Y':
            case 'z':
            case 'Z':
            case '%':
                p++;
                continue;
            case '\0':
            default:
                return BASE_STATUS_FALSE;
            }
        }
        p++;
    }

    return fspr_strftime(s, retsize, max, format, (fspr_time_exp_t *) tm);
}

BASE_DECLARE(base_status_t) base_strftime_nocheck(char *s, base_size_t *retsize, base_size_t max, const char *format, base_time_exp_t *tm)
{
    return fspr_strftime(s, retsize, max, format, (fspr_time_exp_t *) tm);
}

BASE_DECLARE(int) base_snprintf(char *buf, base_size_t len, const char *format, ...)
{
    va_list ap;
    int ret;
    va_start(ap, format);
    ret = fspr_vsnprintf(buf, len, format, ap);
    va_end(ap);
    return ret;
}

BASE_DECLARE(int) base_vsnprintf(char *buf, base_size_t len, const char *format, va_list ap)
{
    return fspr_vsnprintf(buf, len, format, ap);
}

BASE_DECLARE(char *) base_copy_string(char *dst, const char *src, base_size_t dst_size)
{
    if (!dst)
        return NULL;
    if (!src) {
        *dst = '\0';
        return dst;
    }
    return fspr_cpystrn(dst, src, dst_size);
}

/* thread read write lock functions */

BASE_DECLARE(base_status_t) base_thread_rwlock_create(base_thread_rwlock_t ** rwlock, base_memory_pool_t *pool)
{
    return fspr_thread_rwlock_create(rwlock, pool);
}

BASE_DECLARE(base_status_t) base_thread_rwlock_destroy(base_thread_rwlock_t *rwlock)
{
    return fspr_thread_rwlock_destroy(rwlock);
}

BASE_DECLARE(base_memory_pool_t *) base_thread_rwlock_pool_get(base_thread_rwlock_t *rwlock)
{
    return fspr_thread_rwlock_pool_get(rwlock);
}

BASE_DECLARE(base_status_t) base_thread_rwlock_rdlock(base_thread_rwlock_t *rwlock)
{
    return fspr_thread_rwlock_rdlock(rwlock);
}

BASE_DECLARE(base_status_t) base_thread_rwlock_tryrdlock(base_thread_rwlock_t *rwlock)
{
    return fspr_thread_rwlock_tryrdlock(rwlock);
}

BASE_DECLARE(base_status_t) base_thread_rwlock_wrlock(base_thread_rwlock_t *rwlock)
{
    return fspr_thread_rwlock_wrlock(rwlock);
}

BASE_DECLARE(base_status_t) base_thread_rwlock_trywrlock(base_thread_rwlock_t *rwlock)
{
    return fspr_thread_rwlock_trywrlock(rwlock);
}

BASE_DECLARE(base_status_t) base_thread_rwlock_trywrlock_timeout(base_thread_rwlock_t *rwlock, int timeout)
{
    int sanity = timeout * 2;

    while (sanity) {
        if (base_thread_rwlock_trywrlock(rwlock) == BASE_STATUS_SUCCESS) {
            return BASE_STATUS_SUCCESS;
        }
        sanity--;
        base_yield(500000);
    }

    return BASE_STATUS_FALSE;
}


BASE_DECLARE(base_status_t) base_thread_rwlock_unlock(base_thread_rwlock_t *rwlock)
{
    return fspr_thread_rwlock_unlock(rwlock);
}

/* thread mutex functions */

BASE_DECLARE(base_status_t) base_mutex_init(base_mutex_t ** lock, unsigned int flags, base_memory_pool_t *pool)
{
#ifdef WIN32
    /* Old version of APR misunderstands mutexes. On Windows, mutexes are cross-process.
    APR has no reason to not use critical sections instead of mutexes. */
    if (flags == BASE_MUTEX_NESTED) flags = BASE_MUTEX_DEFAULT;
#endif
    return fspr_thread_mutex_create(lock, flags, pool);
}

BASE_DECLARE(base_status_t) base_mutex_destroy(base_mutex_t *lock)
{
    return fspr_thread_mutex_destroy(lock);
}

BASE_DECLARE(base_status_t) base_mutex_lock(base_mutex_t *lock)
{
    return fspr_thread_mutex_lock(lock);
}

BASE_DECLARE(base_status_t) base_mutex_unlock(base_mutex_t *lock)
{
    return fspr_thread_mutex_unlock(lock);
}

BASE_DECLARE(base_status_t) base_mutex_trylock(base_mutex_t *lock)
{
    return fspr_thread_mutex_trylock(lock);
}

/* time function stubs */

BASE_DECLARE(base_time_t) base_time_now(void)
{
#if defined(HAVE_CLOCK_GETTIME) && defined(BASE_USE_CLOCK_FUNCS)
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ts.tv_sec * APR_USEC_PER_SEC + (ts.tv_nsec / 1000);
#else
    return (base_time_t) fspr_time_now();
#endif
}

BASE_DECLARE(base_status_t) base_time_exp_gmt_get(base_time_t *result, base_time_exp_t *input)
{
    return fspr_time_exp_gmt_get((fspr_time_t *) result, (fspr_time_exp_t *) input);
}

BASE_DECLARE(base_status_t) base_time_exp_get(base_time_t *result, base_time_exp_t *input)
{
    return fspr_time_exp_get((fspr_time_t *) result, (fspr_time_exp_t *) input);
}

BASE_DECLARE(base_status_t) base_time_exp_lt(base_time_exp_t *result, base_time_t input)
{
    return fspr_time_exp_lt((fspr_time_exp_t *) result, input);
}

BASE_DECLARE(base_status_t) base_time_exp_tz(base_time_exp_t *result, base_time_t input, base_int32_t offs)
{
    return fspr_time_exp_tz((fspr_time_exp_t *) result, input, (fspr_int32_t) offs);
}

BASE_DECLARE(base_status_t) base_time_exp_gmt(base_time_exp_t *result, base_time_t input)
{
    return fspr_time_exp_gmt((fspr_time_exp_t *) result, input);
}

BASE_DECLARE(base_status_t) base_rfc822_date(char *date_str, base_time_t t)
{
    return fspr_rfc822_date(date_str, t);
}

BASE_DECLARE(base_time_t) base_time_make(base_time_t sec, int32_t usec)
{
    return ((base_time_t) (sec) * APR_USEC_PER_SEC + (base_time_t) (usec));
}

/* Thread condition locks */

BASE_DECLARE(base_status_t) base_thread_cond_create(base_thread_cond_t ** cond, base_memory_pool_t *pool)
{
    return fspr_thread_cond_create(cond, pool);
}

BASE_DECLARE(base_status_t) base_thread_cond_wait(base_thread_cond_t *cond, base_mutex_t *mutex)
{
    return fspr_thread_cond_wait(cond, mutex);
}

BASE_DECLARE(base_status_t) base_thread_cond_timedwait(base_thread_cond_t *cond, base_mutex_t *mutex, base_interval_time_t timeout)
{
    fspr_status_t st = fspr_thread_cond_timedwait(cond, mutex, timeout);

    if (st == APR_TIMEUP) {
        st = BASE_STATUS_TIMEOUT;
    }

    return st;
}

BASE_DECLARE(base_status_t) base_thread_cond_signal(base_thread_cond_t *cond)
{
    return fspr_thread_cond_signal(cond);
}

BASE_DECLARE(base_status_t) base_thread_cond_broadcast(base_thread_cond_t *cond)
{
    return fspr_thread_cond_broadcast(cond);
}

BASE_DECLARE(base_status_t) base_thread_cond_destroy(base_thread_cond_t *cond)
{
    return fspr_thread_cond_destroy(cond);
}

/* file i/o stubs */

BASE_DECLARE(base_status_t) base_file_open(base_file_t ** newf, const char *fname, int32_t flag, base_fileperms_t perm,
    base_memory_pool_t *pool)
{
    return fspr_file_open(newf, fname, flag, perm, pool);
}

BASE_DECLARE(base_status_t) base_file_seek(base_file_t *thefile, base_seek_where_t where, int64_t *offset)
{
    fspr_status_t rv;
    fspr_off_t off = (fspr_off_t) (*offset);
    rv = fspr_file_seek(thefile, where, &off);
    *offset = (int64_t) off;
    return rv;
}

BASE_DECLARE(base_status_t) base_file_copy(const char *from_path, const char *to_path, base_fileperms_t perms, base_memory_pool_t *pool)
{
    return fspr_file_copy(from_path, to_path, perms, pool);
}


BASE_DECLARE(base_status_t) base_file_close(base_file_t *thefile)
{
    return fspr_file_close(thefile);
}

BASE_DECLARE(base_status_t) base_file_trunc(base_file_t *thefile, int64_t offset)
{
    return fspr_file_trunc(thefile, offset);
}

BASE_DECLARE(base_status_t) base_file_lock(base_file_t *thefile, int type)
{
    return fspr_file_lock(thefile, type);
}

BASE_DECLARE(base_status_t) base_file_rename(const char *from_path, const char *to_path, base_memory_pool_t *pool)
{
    return fspr_file_rename(from_path, to_path, pool);
}

BASE_DECLARE(base_status_t) base_file_remove(const char *path, base_memory_pool_t *pool)
{
    return fspr_file_remove(path, pool);
}

BASE_DECLARE(base_status_t) base_file_read(base_file_t *thefile, void *buf, base_size_t *nbytes)
{
    return fspr_file_read(thefile, buf, nbytes);
}

BASE_DECLARE(base_status_t) base_file_write(base_file_t *thefile, const void *buf, base_size_t *nbytes)
{
    return fspr_file_write(thefile, buf, nbytes);
}

BASE_DECLARE(int) base_file_printf(base_file_t *thefile, const char *format, ...)
{
    va_list ap;
    int ret;
    char *data;

    va_start(ap, format);

    if ((ret = base_vasprintf(&data, format, ap)) != -1) {
        base_size_t bytes = strlen(data);
        base_file_write(thefile, data, &bytes);
        free(data);
    }

    va_end(ap);

    return ret;
}

BASE_DECLARE(base_status_t) base_file_mktemp(base_file_t ** thefile, char *templ, int32_t flags, base_memory_pool_t *pool)
{
    return fspr_file_mktemp(thefile, templ, flags, pool);
}

BASE_DECLARE(base_size_t) base_file_get_size(base_file_t *thefile)
{
    struct fspr_finfo_t finfo;
    return fspr_file_info_get(&finfo, APR_FINFO_SIZE, thefile) == BASE_STATUS_SUCCESS ? (base_size_t) finfo.size : 0;
}

BASE_DECLARE(base_status_t) base_directory_exists(const char *dirname, base_memory_pool_t *pool)
{
    fspr_dir_t *dir_handle;
    base_memory_pool_t *our_pool = NULL;
    base_status_t status;

    if (!pool) {
        base_core_new_memory_pool(&our_pool);
        pool = our_pool;
    }

    if ((status = fspr_dir_open(&dir_handle, dirname, pool)) == APR_SUCCESS) {
        fspr_dir_close(dir_handle);
    }

    if (our_pool) {
        base_core_destroy_memory_pool(&our_pool);
    }

    return status;
}

BASE_DECLARE(base_status_t) base_file_exists(const char *filename, base_memory_pool_t *pool)
{
    int32_t wanted = APR_FINFO_TYPE;
    base_memory_pool_t *our_pool = NULL;
    base_status_t status = BASE_STATUS_FALSE;
    fspr_finfo_t info = { 0 };

    if (zstr(filename)) {
        return status;
    }

    if (!pool) {
        base_core_new_memory_pool(&our_pool);
    }

    fspr_stat(&info, filename, wanted, pool ? pool : our_pool);
    if (info.filetype != APR_NOFILE) {
        status = BASE_STATUS_SUCCESS;
    }

    if (our_pool) {
        base_core_destroy_memory_pool(&our_pool);
    }

    return status;
}

BASE_DECLARE(base_status_t) base_dir_make(const char *path, base_fileperms_t perm, base_memory_pool_t *pool)
{
    return fspr_dir_make(path, perm, pool);
}

BASE_DECLARE(base_status_t) base_dir_make_recursive(const char *path, base_fileperms_t perm, base_memory_pool_t *pool)
{
    return fspr_dir_make_recursive(path, perm, pool);
}

struct base_dir {
    fspr_dir_t *dir_handle;
    fspr_finfo_t finfo;
};

BASE_DECLARE(base_status_t) base_dir_open(base_dir_t ** new_dir, const char *dirname, base_memory_pool_t *pool)
{
    base_status_t status;
    base_dir_t *dir = malloc(sizeof(*dir));

    if (!dir) {
        *new_dir = NULL;
        return BASE_STATUS_FALSE;
    }

    memset(dir, 0, sizeof(*dir));
    if ((status = fspr_dir_open(&(dir->dir_handle), dirname, pool)) == APR_SUCCESS) {
        *new_dir = dir;
    } else {
        free(dir);
        *new_dir = NULL;
    }

    return status;
}

BASE_DECLARE(base_status_t) base_dir_close(base_dir_t *thedir)
{
    base_status_t status = fspr_dir_close(thedir->dir_handle);

    free(thedir);
    return status;
}

BASE_DECLARE(uint32_t) base_dir_count(base_dir_t *thedir)
{
    const char *name;
    fspr_int32_t finfo_flags = APR_FINFO_DIRENT | APR_FINFO_TYPE | APR_FINFO_NAME;
    uint32_t count = 0;

    fspr_dir_rewind(thedir->dir_handle);

    while (fspr_dir_read(&(thedir->finfo), finfo_flags, thedir->dir_handle) == BASE_STATUS_SUCCESS) {

        if (thedir->finfo.filetype != APR_REG && thedir->finfo.filetype != APR_LNK) {
            continue;
        }

        if (!(name = thedir->finfo.fname)) {
            name = thedir->finfo.name;
        }

        if (name) {
            count++;
        }
    }

    fspr_dir_rewind(thedir->dir_handle);

    return count;
}

BASE_DECLARE(const char *) base_dir_next_file(base_dir_t *thedir, char *buf, base_size_t len)
{
    const char *fname = NULL;
    fspr_int32_t finfo_flags = APR_FINFO_DIRENT | APR_FINFO_TYPE | APR_FINFO_NAME;
    const char *name;

    while (fspr_dir_read(&(thedir->finfo), finfo_flags, thedir->dir_handle) == BASE_STATUS_SUCCESS) {

        if (thedir->finfo.filetype != APR_REG && thedir->finfo.filetype != APR_LNK) {
            continue;
        }

        if (!(name = thedir->finfo.fname)) {
            name = thedir->finfo.name;
        }

        if (name) {
            base_copy_string(buf, name, len);
            fname = buf;
            break;
        } else {
            continue;
        }
    }
    return fname;
}

/* thread stubs */

#ifndef WIN32
struct fspr_threadattr_t {
    fspr_pool_t *pool;
    pthread_attr_t attr;
    int priority;
};
#else
/* this needs to be revisited when apr for windows supports thread priority settings */
/* search for WIN32 in this file */
struct fspr_threadattr_t {
    fspr_pool_t *pool;
    fspr_int32_t detach;
    fspr_size_t stacksize;
    int priority;
};
#endif


BASE_DECLARE(base_status_t) base_threadattr_create(base_threadattr_t ** new_attr, base_memory_pool_t *pool)
{
    base_status_t status;

    if ((status = fspr_threadattr_create(new_attr, pool)) == BASE_STATUS_SUCCESS) {

        (*new_attr)->priority = BASE_PRI_LOW;

    }

    return status;
}

BASE_DECLARE(base_status_t) base_threadattr_detach_set(base_threadattr_t *attr, int32_t on)
{
    return fspr_threadattr_detach_set(attr, on);
}

BASE_DECLARE(base_status_t) base_threadattr_stacksize_set(base_threadattr_t *attr, base_size_t stacksize)
{
    return fspr_threadattr_stacksize_set(attr, stacksize);
}

BASE_DECLARE(base_status_t) base_threadattr_priority_set(base_threadattr_t *attr, base_thread_priority_t priority)
{

    attr->priority = priority;

    return BASE_STATUS_SUCCESS;
}

static char TT_KEY[] = "1";

BASE_DECLARE(base_status_t) base_thread_create(base_thread_t ** new_thread, base_threadattr_t *attr,
    base_thread_start_t func, void *data, base_memory_pool_t *cont)
{
    base_core_memory_pool_set_data(cont, "_in_thread", TT_KEY);
    return fspr_thread_create(new_thread, attr, func, data, cont);
}

BASE_DECLARE(base_interval_time_t) base_interval_time_from_timeval(struct timeval *tvp)
{
    return ((base_interval_time_t)tvp->tv_sec * 1000000) + tvp->tv_usec / 1000;
}

/* socket stubs */

BASE_DECLARE(base_status_t) base_os_sock_get(base_os_socket_t *thesock, base_socket_t *sock)
{
    return fspr_os_sock_get(thesock, sock);
}

BASE_DECLARE(base_status_t) base_os_sock_put(base_socket_t **sock, base_os_socket_t *thesock, base_memory_pool_t *pool)
{
    return fspr_os_sock_put(sock, thesock, pool);
}

BASE_DECLARE(base_status_t) base_socket_addr_get(base_sockaddr_t ** sa, base_bool_t remote, base_socket_t *sock)
{
    return fspr_socket_addr_get(sa, (fspr_interface_e) remote, sock);
}

BASE_DECLARE(base_status_t) base_socket_create(base_socket_t ** new_sock, int family, int type, int protocol, base_memory_pool_t *pool)
{
    return fspr_socket_create(new_sock, family, type, protocol, pool);
}

BASE_DECLARE(base_status_t) base_socket_shutdown(base_socket_t *sock, base_shutdown_how_e how)
{
    return fspr_socket_shutdown(sock, (fspr_shutdown_how_e) how);
}

BASE_DECLARE(base_status_t) base_socket_close(base_socket_t *sock)
{
    return fspr_socket_close(sock);
}

BASE_DECLARE(base_status_t) base_socket_bind(base_socket_t *sock, base_sockaddr_t *sa)
{
    return fspr_socket_bind(sock, sa);
}

BASE_DECLARE(base_status_t) base_socket_listen(base_socket_t *sock, int32_t backlog)
{
    return fspr_socket_listen(sock, backlog);
}

BASE_DECLARE(base_status_t) base_socket_accept(base_socket_t ** new_sock, base_socket_t *sock, base_memory_pool_t *pool)
{
    return fspr_socket_accept(new_sock, sock, pool);
}

BASE_DECLARE(base_status_t) base_socket_connect(base_socket_t *sock, base_sockaddr_t *sa)
{
    return fspr_socket_connect(sock, sa);
}

BASE_DECLARE(base_status_t) base_socket_send(base_socket_t *sock, const char *buf, base_size_t *len)
{
    int status = BASE_STATUS_SUCCESS;
    base_size_t req = *len, wrote = 0, need = *len;
    int to_count = 0;

    while ((wrote < req && status == BASE_STATUS_SUCCESS) || (need == 0 && status == BASE_STATUS_BREAK) || status == 730035 || status == 35) {
        need = req - wrote;
        status = fspr_socket_send(sock, buf + wrote, &need);
        if (status == BASE_STATUS_BREAK || status == 730035 || status == 35) {
            if (++to_count > 60000) {
                status = BASE_STATUS_FALSE;
                break;
            }
            base_yield(10000);
        } else {
            to_count = 0;
        }
        wrote += need;
    }

    *len = wrote;
    return (base_status_t)status;
}

BASE_DECLARE(base_status_t) base_socket_send_nonblock(base_socket_t *sock, const char *buf, base_size_t *len)
{
    if (!sock || !buf || !len) {
        return BASE_STATUS_GENERR;
    }

    return fspr_socket_send(sock, buf, len);
}

BASE_DECLARE(base_status_t) base_socket_sendto(base_socket_t *sock, base_sockaddr_t *where, int32_t flags, const char *buf,
    base_size_t *len)
{
    if (!where || !buf || !len || !*len) {
        return BASE_STATUS_GENERR;
    }
    return fspr_socket_sendto(sock, where, flags, buf, len);
}

BASE_DECLARE(base_status_t) base_socket_recv(base_socket_t *sock, char *buf, base_size_t *len)
{
    int r;

    r = fspr_socket_recv(sock, buf, len);

    if (r == 35 || r == 730035) {
        r = BASE_STATUS_BREAK;
    }

    return (base_status_t)r;
}

BASE_DECLARE(base_status_t) base_sockaddr_create(base_sockaddr_t **sa, base_memory_pool_t *pool)
{
    base_sockaddr_t *new_sa;
    unsigned short family = APR_INET;

    new_sa = fspr_pcalloc(pool, sizeof(fspr_sockaddr_t));
    base_assert(new_sa);
    new_sa->pool = pool;

    new_sa->family = family;
    new_sa->sa.sin.sin_family = family;

    new_sa->salen = sizeof(struct sockaddr_in);
    new_sa->addr_str_len = 16;
    new_sa->ipaddr_ptr = &(new_sa->sa.sin.sin_addr);
    new_sa->ipaddr_len = sizeof(struct in_addr);

    *sa = new_sa;
    return BASE_STATUS_SUCCESS;
}

BASE_DECLARE(base_status_t) base_sockaddr_info_get(base_sockaddr_t ** sa, const char *hostname, int32_t family,
    base_port_t port, int32_t flags, base_memory_pool_t *pool)
{
    return fspr_sockaddr_info_get(sa, hostname, family, port, flags, pool);
}

BASE_DECLARE(base_status_t) base_socket_opt_set(base_socket_t *sock, int32_t opt, int32_t on)
{
    if (opt == BASE_SO_TCP_KEEPIDLE) {
#if defined(TCP_KEEPIDLE)
        int r = -10;
        r = setsockopt(sock->socketdes, SOL_TCP, TCP_KEEPIDLE, (void *)&on, sizeof(on));
        return r ? BASE_STATUS_FALSE : BASE_STATUS_SUCCESS;
#else
        return BASE_STATUS_NOTIMPL;
#endif
    }

    if (opt == BASE_SO_TCP_KEEPINTVL) {
#if defined(TCP_KEEPINTVL)
        int r = -10;
        r = setsockopt(sock->socketdes, SOL_TCP, TCP_KEEPINTVL, (void *)&on, sizeof(on));
        return r ? BASE_STATUS_FALSE : BASE_STATUS_SUCCESS;
#else
        return BASE_STATUS_NOTIMPL;
#endif
    }

    return fspr_socket_opt_set(sock, opt, on);
}

BASE_DECLARE(base_status_t) base_socket_timeout_get(base_socket_t *sock, base_interval_time_t *t)
{
    fspr_interval_time_t at = 0;
    base_status_t status = fspr_socket_timeout_get(sock, &at);
    *t = at;

    return status;
}

BASE_DECLARE(base_status_t) base_socket_timeout_set(base_socket_t *sock, base_interval_time_t t)
{
    return fspr_socket_timeout_set(sock, t);
}

BASE_DECLARE(base_status_t) base_sockaddr_ip_get(char **addr, base_sockaddr_t *sa)
{
    return fspr_sockaddr_ip_get(addr, sa);
}

BASE_DECLARE(int) base_sockaddr_equal(const base_sockaddr_t *sa1, const base_sockaddr_t *sa2)
{
    return fspr_sockaddr_equal(sa1, sa2);
}

BASE_DECLARE(base_status_t) base_mcast_join(base_socket_t *sock, base_sockaddr_t *join, base_sockaddr_t *iface, base_sockaddr_t *source)
{
    return fspr_mcast_join(sock, join, iface, source);
}

BASE_DECLARE(base_status_t) base_mcast_hops(base_socket_t *sock, uint8_t ttl)
{
    return fspr_mcast_hops(sock, ttl);
}

BASE_DECLARE(base_status_t) base_mcast_loopback(base_socket_t *sock, uint8_t opt)
{
    return fspr_mcast_loopback(sock, opt);
}

BASE_DECLARE(base_status_t) base_mcast_interface(base_socket_t *sock, base_sockaddr_t *iface)
{
    return fspr_mcast_interface(sock, iface);
}


/* socket functions */

BASE_DECLARE(const char *) base_get_addr(char *buf, base_size_t len, base_sockaddr_t *in)
{
    if (!in) {
        return BASE_BLANK_STRING;
    }

    memset(buf, 0, len);

    if (in->family == AF_INET) {
        get_addr(buf, len, (struct sockaddr *) &in->sa, in->salen);
        return buf;
    }

    get_addr6(buf, len, (struct sockaddr_in6 *) &in->sa, in->salen);
    return buf;
}

BASE_DECLARE(int) base_socket_fd_get(base_socket_t *sock)
{
    return fspr_socket_fd_get(sock);
}

BASE_DECLARE(uint16_t) base_sockaddr_get_port(base_sockaddr_t *sa)
{
    return sa->port;
}

BASE_DECLARE(int32_t) base_sockaddr_get_family(base_sockaddr_t *sa)
{
    return sa->family;
}

BASE_DECLARE(base_status_t) base_getnameinfo(char **hostname, base_sockaddr_t *sa, int32_t flags)
{
    return fspr_getnameinfo(hostname, sa, flags);
}

BASE_DECLARE(base_status_t) base_socket_atmark(base_socket_t *sock, int *atmark)
{
    return fspr_socket_atmark(sock, atmark);
}

BASE_DECLARE(base_status_t) base_socket_recvfrom(base_sockaddr_t *from, base_socket_t *sock, int32_t flags, char *buf, size_t *len)
{
    int r = BASE_STATUS_GENERR;

    if (from && sock && (r = fspr_socket_recvfrom(from, sock, flags, buf, len)) == APR_SUCCESS) {
        from->port = ntohs(from->sa.sin.sin_port);
        /* from->ipaddr_ptr = &(from->sa.sin.sin_addr);
        * from->ipaddr_ptr = inet_ntoa(from->sa.sin.sin_addr);
        */
    }

    if (r == 35 || r == 730035) {
        r = BASE_STATUS_BREAK;
    }

    return (base_status_t)r;
}

/* poll stubs */

BASE_DECLARE(base_status_t) base_pollset_create(base_pollset_t ** pollset, uint32_t size, base_memory_pool_t *pool, uint32_t flags)
{
    return fspr_pollset_create(pollset, size, pool, flags);
}

BASE_DECLARE(base_status_t) base_pollset_add(base_pollset_t *pollset, const base_pollfd_t *descriptor)
{
    if (!pollset || !descriptor) {
        return BASE_STATUS_FALSE;
    }

    return fspr_pollset_add((fspr_pollset_t *) pollset, (const fspr_pollfd_t *) descriptor);
}

BASE_DECLARE(base_status_t) base_pollset_remove(base_pollset_t *pollset, const base_pollfd_t *descriptor)
{
    if (!pollset || !descriptor) {
        return BASE_STATUS_FALSE;
    }

    return fspr_pollset_remove((fspr_pollset_t *) pollset, (const fspr_pollfd_t *) descriptor);
}

BASE_DECLARE(base_status_t) base_socket_create_pollfd(base_pollfd_t **pollfd, base_socket_t *sock, int16_t flags, void *client_data, base_memory_pool_t *pool)
{
    if (!pollfd || !sock) {
        return BASE_STATUS_FALSE;
    }

    if ((*pollfd = (base_pollfd_t*)fspr_palloc(pool, sizeof(base_pollfd_t))) == 0) {
        return BASE_STATUS_MEMERR;
    }

    memset(*pollfd, 0, sizeof(base_pollfd_t));

    (*pollfd)->desc_type = (base_pollset_type_t) APR_POLL_SOCKET;
    (*pollfd)->reqevents = flags;
    (*pollfd)->desc.s = sock;
    (*pollfd)->client_data = client_data;

    return BASE_STATUS_SUCCESS;
}


BASE_DECLARE(base_status_t) base_pollset_poll(base_pollset_t *pollset, base_interval_time_t timeout, int32_t *num, const base_pollfd_t **descriptors)
{
    fspr_status_t st = BASE_STATUS_FALSE;

    if (pollset) {
        st = fspr_pollset_poll((fspr_pollset_t *) pollset, timeout, num, (const fspr_pollfd_t **) descriptors);

        if (st == APR_TIMEUP) {
            st = BASE_STATUS_TIMEOUT;
        }
    }

    return st;
}

BASE_DECLARE(base_status_t) base_poll(base_pollfd_t *aprset, int32_t numsock, int32_t *nsds, base_interval_time_t timeout)
{
    fspr_status_t st = BASE_STATUS_FALSE;

    if (aprset) {
        st = fspr_poll((fspr_pollfd_t *) aprset, numsock, nsds, timeout);

        if (numsock == 1 && ((aprset[0].rtnevents & APR_POLLERR) || (aprset[0].rtnevents & APR_POLLHUP) || (aprset[0].rtnevents & APR_POLLNVAL))) {
            st = BASE_STATUS_GENERR;
        } else if (st == APR_TIMEUP) {
            st = BASE_STATUS_TIMEOUT;
        }
    }

    return st;
}

BASE_DECLARE(base_status_t) base_socket_create_pollset(base_pollfd_t ** poll, base_socket_t *sock, int16_t flags, base_memory_pool_t *pool)
{
    base_pollset_t *pollset;

    if (base_pollset_create(&pollset, 1, pool, 0) != BASE_STATUS_SUCCESS) {
        return BASE_STATUS_GENERR;
    }

    if (base_socket_create_pollfd(poll, sock, flags, sock, pool) != BASE_STATUS_SUCCESS) {
        return BASE_STATUS_GENERR;
    }

    if (base_pollset_add(pollset, *poll) != BASE_STATUS_SUCCESS) {
        return BASE_STATUS_GENERR;
    }

    return BASE_STATUS_SUCCESS;
}

/* apr-util stubs */

/* UUID Handling (apr-util) */

BASE_DECLARE(void) base_uuid_format(char *buffer, const base_uuid_t *uuid)
{
#ifndef WIN32
    uuid_unparse_lower(uuid->data, buffer);
#else
    RPC_CSTR buf;
    UuidToString((const UUID *) uuid, &buf);
    strcpy(buffer, (const char *) buf);
    RpcStringFree(&buf);
#endif
}

BASE_DECLARE(void) base_uuid_get(base_uuid_t *uuid)
{
    base_mutex_lock(runtime.uuid_mutex);
#ifndef WIN32
    uuid_generate(uuid->data);
#else
    UuidCreate((UUID *) uuid);
#endif
    base_mutex_unlock(runtime.uuid_mutex);
}

BASE_DECLARE(base_status_t) base_uuid_parse(base_uuid_t *uuid, const char *uuid_str)
{
#ifndef WIN32
    if (uuid_parse(uuid_str, uuid->data)) {
        return BASE_STATUS_FALSE;
    }
    return BASE_STATUS_SUCCESS;
#else
    return UuidFromString((RPC_CSTR) uuid_str, (UUID *) uuid);
#endif
}

BASE_DECLARE(base_status_t) base_md5(unsigned char digest[BASE_MD5_DIGESTSIZE], const void *input, base_size_t inputLen)
{
#if (defined(HAVE_LIBMD5) || defined(HAVE_LIBMD) || defined(HAVE_MD5INIT))
    MD5_CTX md5_context;

    MD5Init(&md5_context);
    MD5Update(&md5_context, input, inputLen);
    MD5Final(digest, &md5_context);

    return BASE_STATUS_SUCCESS;
#elif defined(HAVE_LIBCRYPTO)
#if OPENSSL_VERSION_NUMBER < 0x30000000
    MD5_CTX md5_context;

    MD5_Init(&md5_context);
    MD5_Update(&md5_context, input, inputLen);
    MD5_Final(digest, &md5_context);
#else
    EVP_MD_CTX *md5_context;

    /* MD5_Init */
    md5_context = EVP_MD_CTX_new();
    EVP_DigestInit_ex(md5_context, EVP_md5(), NULL);
    /* MD5_Update */
    EVP_DigestUpdate(md5_context, input, inputLen);
    /* MD5_Final */
    EVP_DigestFinal_ex(md5_context, digest, NULL);
    EVP_MD_CTX_free(md5_context);
#endif

    return BASE_STATUS_SUCCESS;
#else
    return BASE_STATUS_NOTIMPL;
#endif
}

BASE_DECLARE(base_status_t) base_md5_string(char digest_str[BASE_MD5_DIGEST_STRING_SIZE], const void *input, base_size_t inputLen)
{
    unsigned char digest[BASE_MD5_DIGESTSIZE];
    base_status_t status = base_md5(digest, input, inputLen);
    short i, x;
    uint8_t b;

    digest_str[BASE_MD5_DIGEST_STRING_SIZE - 1] = '\0';

    for (x = i = 0; x < BASE_MD5_DIGESTSIZE; x++) {
        b = (digest[x] >> 4) & 15;
        digest_str[i++] = b + (b > 9 ? 'a' - 10 : '0');
        b = digest[x] & 15;
        digest_str[i++] = b + (b > 9 ? 'a' - 10 : '0');
    }
    digest_str[i] = '\0';

    return status;
}

/* FIFO queues (apr-util) */

BASE_DECLARE(base_status_t) base_queue_create(base_queue_t ** queue, unsigned int queue_capacity, base_memory_pool_t *pool)
{
    return base_apr_queue_create(queue, queue_capacity, pool);
}

BASE_DECLARE(unsigned int) base_queue_size(base_queue_t *queue)
{
    return base_apr_queue_size(queue);
}

BASE_DECLARE(base_status_t) base_queue_pop(base_queue_t *queue, void **data)
{
    return base_apr_queue_pop(queue, data);
}

BASE_DECLARE(base_status_t) base_queue_pop_timeout(base_queue_t *queue, void **data, base_interval_time_t timeout)
{
    return base_apr_queue_pop_timeout(queue, data, timeout);
}

BASE_DECLARE(base_status_t) base_queue_push(base_queue_t *queue, void *data)
{
    fspr_status_t s;

    do {
        s = base_apr_queue_push(queue, data);
    } while (s == APR_EINTR);

    return s;
}

BASE_DECLARE(base_status_t) base_queue_trypop(base_queue_t *queue, void **data)
{
    return base_apr_queue_trypop(queue, data);
}

BASE_DECLARE(base_status_t) base_queue_interrupt_all(base_queue_t *queue)
{
    return base_apr_queue_interrupt_all(queue);
}

BASE_DECLARE(base_status_t) base_queue_term(base_queue_t *queue)
{
    return base_apr_queue_term(queue);
}

BASE_DECLARE(base_status_t) base_queue_trypush(base_queue_t *queue, void *data)
{
    fspr_status_t s;

    do {
        s = base_apr_queue_trypush(queue, data);
    } while (s == APR_EINTR);

    return s;
}

BASE_DECLARE(int) base_vasprintf(char **ret, const char *fmt, va_list ap)
{
#ifdef HAVE_VASPRINTF
    return vasprintf(ret, fmt, ap);
#else
    char *buf;
    int len;
    size_t buflen;
    va_list ap2;
    char *tmp = NULL;

#ifdef _MSC_VER
#if _MSC_VER >= 1500
    /* hack for incorrect assumption in msvc header files for code analysis */
    __analysis_assume(tmp);
#endif
    ap2 = ap;
#else
    va_copy(ap2, ap);
#endif

    len = vsnprintf(tmp, 0, fmt, ap2);

    if (len > 0 && (buf = malloc((buflen = (size_t) (len + 1)))) != NULL) {
        len = vsnprintf(buf, buflen, fmt, ap);
        *ret = buf;
    } else {
        *ret = NULL;
        len = -1;
    }

    va_end(ap2);
    return len;
#endif
}

BASE_DECLARE(base_status_t) base_match_glob(const char *pattern, base_array_header_t ** result, base_memory_pool_t *pool)
{
    return fspr_match_glob(pattern, (fspr_array_header_t **) result, pool);
}

/**
* Create an anonymous pipe.
* @param in The file descriptor to use as input to the pipe.
* @param out The file descriptor to use as output from the pipe.
* @param pool The pool to operate on.
*/
BASE_DECLARE(base_status_t) base_file_pipe_create(base_file_t ** in, base_file_t ** out, base_memory_pool_t *pool)
{
    return fspr_file_pipe_create((fspr_file_t **) in, (fspr_file_t **) out, pool);
}

/**
* Get the timeout value for a pipe or manipulate the blocking state.
* @param thepipe The pipe we are getting a timeout for.
* @param timeout The current timeout value in microseconds.
*/
BASE_DECLARE(base_status_t) base_file_pipe_timeout_get(base_file_t *thepipe, base_interval_time_t *timeout)
{
    return fspr_file_pipe_timeout_get((fspr_file_t *) thepipe, (fspr_interval_time_t *) timeout);
}

/**
* Set the timeout value for a pipe or manipulate the blocking state.
* @param thepipe The pipe we are setting a timeout on.
* @param timeout The timeout value in microseconds.  Values < 0 mean wait
*        forever, 0 means do not wait at all.
*/
BASE_DECLARE(base_status_t) base_file_pipe_timeout_set(base_file_t *thepipe, base_interval_time_t timeout)
{
    return fspr_file_pipe_timeout_set((fspr_file_t *) thepipe, (fspr_interval_time_t) timeout);
}


/**
* stop the current thread
* @param thd The thread to stop
* @param retval The return value to pass back to any thread that cares
*/
BASE_DECLARE(base_status_t) base_thread_exit(base_thread_t *thd, base_status_t retval)
{
    return fspr_thread_exit((fspr_thread_t *) thd, retval);
}

/**
* block until the desired thread stops executing.
* @param retval The return value from the dead thread.
* @param thd The thread to join
*/
BASE_DECLARE(base_status_t) base_thread_join(base_status_t *retval, base_thread_t *thd)
{
    if ( !thd ) {
        base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_ERROR, "ERROR: Attempting to join thread that does not exist\n");
        return BASE_STATUS_FALSE;
    }

    return fspr_thread_join((fspr_status_t *) retval, (fspr_thread_t *) thd);
}


BASE_DECLARE(base_status_t) base_atomic_init(base_memory_pool_t *pool)
{
    return fspr_atomic_init((fspr_pool_t *) pool);
}

BASE_DECLARE(uint32_t) base_atomic_read(volatile base_atomic_t *mem)
{
#ifdef fspr_atomic_t
    return fspr_atomic_read((fspr_atomic_t *)mem);
#else
    return fspr_atomic_read32((fspr_uint32_t *)mem);
#endif
}

BASE_DECLARE(void) base_atomic_set(volatile base_atomic_t *mem, uint32_t val)
{
#ifdef fspr_atomic_t
    fspr_atomic_set((fspr_atomic_t *)mem, val);
#else
    fspr_atomic_set32((fspr_uint32_t *)mem, val);
#endif
}

BASE_DECLARE(void) base_atomic_add(volatile base_atomic_t *mem, uint32_t val)
{
#ifdef fspr_atomic_t
    fspr_atomic_add((fspr_atomic_t *)mem, val);
#else
    fspr_atomic_add32((fspr_uint32_t *)mem, val);
#endif
}

BASE_DECLARE(void) base_atomic_inc(volatile base_atomic_t *mem)
{
#ifdef fspr_atomic_t
    fspr_atomic_inc((fspr_atomic_t *)mem);
#else
    fspr_atomic_inc32((fspr_uint32_t *)mem);
#endif
}

BASE_DECLARE(int) base_atomic_dec(volatile base_atomic_t *mem)
{
#ifdef fspr_atomic_t
    return fspr_atomic_dec((fspr_atomic_t *)mem);
#else
    return fspr_atomic_dec32((fspr_uint32_t *)mem);
#endif
}

BASE_DECLARE(char *) base_strerror(base_status_t statcode, char *buf, base_size_t bufsize)
{
    return fspr_strerror(statcode, buf, bufsize);
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
