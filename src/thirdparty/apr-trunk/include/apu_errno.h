/* Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef APU_ERRNO_H
#define APU_ERRNO_H

/**
 * @file apu_errno.h
 * @brief APR-Util Error Codes
 */

#include "apr.h"
#include "apr_errno.h"
#include "apr_pools.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup apu_errno Error Codes
 * @ingroup APR
 * @{
 */

/**
 * @defgroup APR_Util_Error APR_Util Error Values
 * <PRE>
 * <b>APU ERROR VALUES</b>
 * APR_ENOKEY         The key provided was empty or NULL
 * APR_ENOIV          The initialisation vector provided was NULL
 * APR_EKEYTYPE       The key type was not recognised
 * APR_ENOSPACE       The buffer supplied was not big enough
 * APR_ECRYPT         An error occurred while encrypting or decrypting
 * APR_EPADDING       Padding was not supported
 * APR_EKEYLENGTH     The key length was incorrect
 * APR_ENOCIPHER      The cipher provided was not recognised
 * APR_ENODIGEST      The digest provided was not recognised
 * APR_ENOENGINE      The engine provided was not recognised
 * APR_EINITENGINE    The engine could not be initialised
 * APR_EREINIT        Underlying crypto has already been initialised
 * APR_ENOVERIFY      The signature verification failed
 * APR_SERVER_DOWN    The server is down
 * APR_AUTH_UNKNOWN   Unknown SASL mechanism 
 * APR_PROXY_AUTH     Proxy authorization has failed
 * APR_INAPPROPRIATE_AUTH  Authentication not appropriate for this entry
 * APR_INVALID_CREDENTIALS Invalid credentials were presented
 * APR_INSUFFICIENT_ACCESS The user has insufficient access
 * APR_INSUFFICIENT_RIGHTS The user has insufficient rights
 * APR_CONSTRAINT_VIOLATION A constraint was violated
 * APR_FILTER_ERROR   The filter was malformed
 * APR_NO_SUCH_OBJECT The object does not exist
 * APR_NO_SUCH_ATTRIBUTE The attribute does not exist
 * APR_COMPARE_TRUE   The comparison returned true
 * APR_COMPARE_FALSE  The comparison returned false
 * APR_NO_RESULTS_RETURNED No results were returned
 * APR_WANT_READ      Call me again when the socket is ready for reading
 * APR_WANT_WRITE     Call me again when the socket is ready for writing
 * APR_USER_CANCELLED User has cancelled the request
 * </PRE>
 *
 * <PRE>
 * <b>APR STATUS VALUES</b>
 * APR_INCHILD        Program is currently executing in the child
 * </PRE>
 * @{
 */
/** @see APR_STATUS_IS_ENOKEY */
#define APR_ENOKEY           (APR_UTIL_START_STATUS + 1)
/** @see APR_STATUS_IS_ENOIV */
#define APR_ENOIV            (APR_UTIL_START_STATUS + 2)
/** @see APR_STATUS_IS_EKEYTYPE */
#define APR_EKEYTYPE         (APR_UTIL_START_STATUS + 3)
/** @see APR_STATUS_IS_ENOSPACE */
#define APR_ENOSPACE         (APR_UTIL_START_STATUS + 4)
/** @see APR_STATUS_IS_ECRYPT */
#define APR_ECRYPT           (APR_UTIL_START_STATUS + 5)
/** @see APR_STATUS_IS_EPADDING */
#define APR_EPADDING         (APR_UTIL_START_STATUS + 6)
/** @see APR_STATUS_IS_EKEYLENGTH */
#define APR_EKEYLENGTH       (APR_UTIL_START_STATUS + 7)
/** @see APR_STATUS_IS_ENOCIPHER */
#define APR_ENOCIPHER        (APR_UTIL_START_STATUS + 8)
/** @see APR_STATUS_IS_ENODIGEST */
#define APR_ENODIGEST        (APR_UTIL_START_STATUS + 9)
/** @see APR_STATUS_IS_ENOENGINE */
#define APR_ENOENGINE        (APR_UTIL_START_STATUS + 10)
/** @see APR_STATUS_IS_EINITENGINE */
#define APR_EINITENGINE      (APR_UTIL_START_STATUS + 11)
/** @see APR_STATUS_IS_EREINIT */
#define APR_EREINIT          (APR_UTIL_START_STATUS + 12)
/** @see APR_STATUS_IS_ENOVERIFY */
#define APR_ENOVERIFY        (APR_UTIL_START_STATUS + 13)
/** @see APR_STATUS_IS_SERVER_DOWN */
#define APR_SERVER_DOWN                (APR_UTIL_START_STATUS + 101)
/** @see APR_STATUS_IS_AUTH_UNKNOWN */
#define APR_AUTH_UNKNOWN               (APR_UTIL_START_STATUS + 102)
/** @see APR_STATUS_IS_PROXY_AUTH */
#define APR_PROXY_AUTH                 (APR_UTIL_START_STATUS + 103)
/** @see APR_STATUS_IS_INAPPROPRIATE_AUTH */
#define APR_INAPPROPRIATE_AUTH         (APR_UTIL_START_STATUS + 104)
/** @see APR_STATUS_IS_INVALID_CREDENTIALS */
#define APR_INVALID_CREDENTIALS        (APR_UTIL_START_STATUS + 105)
/** @see APR_STATUS_IS_INSUFFICIENT_ACCESS */
#define APR_INSUFFICIENT_ACCESS        (APR_UTIL_START_STATUS + 106)
/** @see APR_STATUS_IS_INSUFFICIENT_RIGHTS */
#define APR_INSUFFICIENT_RIGHTS        (APR_UTIL_START_STATUS + 107)
/** @see APR_STATUS_IS_CONSTRAINT_VIOLATION */
#define APR_CONSTRAINT_VIOLATION       (APR_UTIL_START_STATUS + 108)
/** @see APR_STATUS_IS_FILTER_ERROR */
#define APR_FILTER_ERROR               (APR_UTIL_START_STATUS + 109)
/** @see APR_STATUS_IS_NO_SUCH_OBJECT */
#define APR_NO_SUCH_OBJECT             (APR_UTIL_START_STATUS + 110)
/** @see APR_STATUS_IS_NO_SUCH_ATTRIBUTE */
#define APR_NO_SUCH_ATTRIBUTE          (APR_UTIL_START_STATUS + 111)
/** @see APR_STATUS_IS_COMPARE_TRUE */
#define APR_COMPARE_TRUE               (APR_UTIL_START_STATUS + 112)
/** @see APR_STATUS_IS_COMPARE_FALSE */
#define APR_COMPARE_FALSE              (APR_UTIL_START_STATUS + 113)
/** @see APR_STATUS_IS_NO_RESULTS_RETURNED */
#define APR_NO_RESULTS_RETURNED        (APR_UTIL_START_STATUS + 114)
/** @see APR_STATUS_IS_WANT_READ */
#define APR_WANT_READ                  (APR_UTIL_START_STATUS + 115)
/** @see APR_STATUS_IS_WANT_WRITE */
#define APR_WANT_WRITE                 (APR_UTIL_START_STATUS + 116)
/** @see APR_STATUS_IS_USER_CANCELLED */
#define APR_USER_CANCELLED             (APR_UTIL_START_STATUS + 117)
/** @} */

/**
 * @defgroup APU_STATUS_IS Status Value Tests
 * @warning For any particular error condition, more than one of these tests
 *      may match. This is because platform-specific error codes may not
 *      always match the semantics of the POSIX codes these tests (and the
 *      corresponding APR error codes) are named after. A notable example
 *      are the APR_STATUS_IS_ENOENT and APR_STATUS_IS_ENOTDIR tests on
 *      Win32 platforms. The programmer should always be aware of this and
 *      adjust the order of the tests accordingly.
 * @{
 */

/** @} */

/**
 * @addtogroup APR_Util_Error
 * @{
 */
/**
 * The key was empty or not provided
 */
#define APR_STATUS_IS_ENOKEY(s)        ((s) == APR_ENOKEY)
/**
 * The initialisation vector was not provided
 */
#define APR_STATUS_IS_ENOIV(s)        ((s) == APR_ENOIV)
/**
 * The key type was not recognised
 */
#define APR_STATUS_IS_EKEYTYPE(s)        ((s) == APR_EKEYTYPE)
/**
 * The buffer provided was not big enough
 */
#define APR_STATUS_IS_ENOSPACE(s)        ((s) == APR_ENOSPACE)
/**
 * An error occurred while encrypting or decrypting
 */
#define APR_STATUS_IS_ECRYPT(s)        ((s) == APR_ECRYPT)
/**
 * An error occurred while padding
 */
#define APR_STATUS_IS_EPADDING(s)        ((s) == APR_EPADDING)
/**
 * An error occurred with the key length
 */
#define APR_STATUS_IS_EKEYLENGTH(s)        ((s) == APR_EKEYLENGTH)
/**
 * The cipher provided was not recognised
 */
#define APR_STATUS_IS_ENOCIPHER(s)        ((s) == APR_ENOCIPHER)
/**
 * The digest provided was not recognised
 */
#define APR_STATUS_IS_ENODIGEST(s)        ((s) == APR_ENODIGEST)
/**
 * The engine provided was not recognised
 */
#define APR_STATUS_IS_ENOENGINE(s)        ((s) == APR_ENOENGINE)
/**
 * The engine could not be initialised
 */
#define APR_STATUS_IS_EINITENGINE(s)        ((s) == APR_EINITENGINE)
/**
 * Crypto has already been initialised
 */
#define APR_STATUS_IS_EREINIT(s)        ((s) == APR_EREINIT)
/**
 * The signature verification failed
 */
#define APR_STATUS_IS_ENOVERIFY(s)        ((s) == APR_ENOVERIFY)
/**
 * The server is down
 */
#define APR_STATUS_IS_SERVER_DOWN(s)        ((s) == APR_SERVER_DOWN)
/**
 * Authentication mechanism not supoorted by this server
 */
#define APR_STATUS_IS_AUTH_UNKNOWN(s)        ((s) == APR_AUTH_UNKNOWN)
/**
 * Proxy authorization has failed
 */
#define APR_STATUS_IS_PROXY_AUTH(s)        ((s) == APR_PROXY_AUTH)
/**
 * Inappropriate authentication was specified (e.g., simple auth
 * was specified but the entry does not have a userPassword attribute).
 */
#define APR_STATUS_IS_INAPPROPRIATE_AUTH(s)        ((s) == APR_INAPPROPRIATE_AUTH)
/**
 * Invalid credentials were presented (e.g., the wrong password).
 */
#define APR_STATUS_IS_INVALID_CREDENTIALS(s)        ((s) == APR_INVALID_CREDENTIALS)
/**
 * The user has insufficient access to perform the operation.
 */
#define APR_STATUS_IS_INSUFFICIENT_ACCESS(s)        ((s) == APR_INSUFFICIENT_ACCESS)
/**
 * The user has insufficient rights to perform the operation.
 */
#define APR_STATUS_IS_INSUFFICIENT_RIGHTS(s)        ((s) == APR_INSUFFICIENT_RIGHTS)
/**
 * A constraint was violated.
 */
#define APR_STATUS_IS_CONSTRAINT_VIOLATION(s)        ((s) == APR_CONSTRAINT_VIOLATION)
/**
 * The filter was malformed.
 */
#define APR_STATUS_IS_FILTER_ERROR(s)        ((s) == APR_FILTER_ERROR)
/**
 * No such object.
 */
#define APR_STATUS_IS_NO_SUCH_OBJECT(s)        ((s) == APR_NO_SUCH_OBJECT)
/**
 * No such attribute.
 */
#define APR_STATUS_IS_NO_SUCH_ATTRIBUTE(s)        ((s) == APR_NO_SUCH_ATTRIBUTE)
/**
 * Comparison is true.
 */
#define APR_STATUS_IS_COMPARE_TRUE(s)        ((s) == APR_COMPARE_TRUE)
/**
 * Comparison is false.
 */
#define APR_STATUS_IS_COMPARE_FALSE(s)        ((s) == APR_COMPARE_FALSE)
/**
 * No results were returned.
 */
#define APR_STATUS_IS_NO_RESULTS_RETURNED(s)        ((s) == APR_NO_RESULTS_RETURNED)
/**
 * Call us back when the socket is ready for a read.
 */
#define APR_STATUS_IS_WANT_READ(s)        ((s) == APR_WANT_READ)
/**
 * Call us back when the socket is ready for a write.
 */
#define APR_STATUS_IS_WANT_WRITE(s)        ((s) == APR_WANT_WRITE)
/**
 * User has cancelled the request.
 */
#define APR_STATUS_IS_USER_CANCELLED(s)        ((s) == APR_USER_CANCELLED)
/** @} */

/**
 * This structure allows the underlying API error codes to be returned
 * along with plain text error messages that explain to us mere mortals
 * what really happened.
 */
typedef struct apu_err_t {
    /** What was APR trying to do when the error occurred */
    const char *reason;
    /** Error message from the underlying system */
    const char *msg;
    /** Native return code from the underlying system */
    int rc;
} apu_err_t;

/**
 * Populate a apu_err_t structure with the given error, allocated
 * from the given pool.
 *
 * If the result parameter points at a NULL pointer, a apu_err_t
 * structure will be allocated, otherwise the apu_err_t structure
 * will be reused.
 * @param result If NULL, the apu_err_t structure is allocated and
 *   returned, otherwise the existing apu_err_t is used.
 * @param p The pool to use.
 * @param reason The reason string, may be NULL.
 * @param rc The underlying result code.
 * @param fmt The format of the string
 * @param ... The arguments to use while printing the data
 * @return The apu_err_t structure on success, NULL if out of memory.
 */
APR_DECLARE_NONSTD(apu_err_t *) apr_errprintf(apu_err_t *result,
        apr_pool_t *p, const char *reason, int rc, const char *fmt, ...)
        __attribute__((format(printf,5,6)))
        __attribute__((nonnull(2)));

/** @} */

#ifdef __cplusplus
}
#endif

#endif  /* ! APU_ERRNO_H */
