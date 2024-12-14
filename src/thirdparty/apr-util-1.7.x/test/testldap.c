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
 
 /* Setup for legacy API:
  *  - Create or edit the file data/host.data and add an
  *     ldap server DN.  Multiple DNs may be listed on
  *     a single line.
  *  - Copy the server certificates to the data/ directory.
  *     All DER type certificates must have the .der extention.
  *     All BASE64 or PEM certificates must have the .b64
  *     extension.  All certificate files copied to the /data
  *     directory will be added to the ldap certificate store.
  */

 /* Setup:
  *  - Set environment variables as below for each URL being
  *     tested, along with dns and credentials.
  *  - Copy the server certificates to the data/ directory.
  *     All DER type certificates must have the .der extention.
  *     All BASE64 or PEM certificates must have the .b64
  *     extension.  All certificate files copied to the /data
  *     directory will be added to the ldap certificate store.
  */

 /* Legacy tests:
  *
  * This test covers the following three types of connections:
  *  - Unsecure ldap://
  *  - Secure ldaps://
  *  - Secure ldap://+Start_TLS
  *
  *  - (TBD) Mutual authentication 
  *
  * There are other variations that should be tested:
  *  - All of the above with multiple redundant LDAP servers
  *     This can be done by listing more than one server DN
  *      in the host.data file.  The DNs should all be listed
  *      on one line separated by a space.
  *  - All of the above with multiple certificates
  *     If more than one certificate is found in the data/
  *      directory, each certificate found will be added
  *      to the certificate store.
  *  - All of the above on alternate ports
  *     An alternate port can be specified as part of the
  *      host in the host.data file.  The ":port" should 
  *      follow each DN listed.  Default is 389 and 636.
  *  - Secure connections with mutual authentication
  */

 /* This test covers the following three types of connections:
  *
  *  - Unsecure ldap://
  *  - Secure ldaps://
  *  - Secure ldap://+Start_TLS
  *  - Unix ldapi://
  *
  * Environment variables:
  *
  * One or more of the following, based on what you have available.
  *
  * - TESTLDAP=ldap://localhost:389
  * - TESTLDAP_TLS=ldap://starttls.server:389
  * - TESTLDAPS=ldaps://ssl.server:636
  * - TESTLDAPI=/tmp/slapd-server.socket
  *
  * Simple bind:
  *
  * - TESTLDAP_DN=cn=Directory Manager
  * - TESTLDAP_PASS=opensesame
  *
  * SASL PLAIN bind:
  *
  * - TESTLDAP_USER=sasluser
  * - TESTLDAP_AUTHNAME=sasluser
  * - TESTLDAP_PASS=opensesame
  *
  * SASL EXTERNAL bind is attempted where supported.
  */

 /*
  * For a working example of an asynchronous LDAP conversation involving a bind,
  * followed by a search, followed by a compare, start at test_ldap_connection()
  * below.
  */


#include "testutil.h"

#include "apr.h"
#include "apr_general.h"
#include "apr_ldap.h"
#include "apr_file_io.h"
#include "apr_file_info.h"
#include "apr_strings.h"
#if APR_HAVE_STDLIB_H
#include <stdlib.h>
#endif
#define APR_WANT_STDIO
#define APR_WANT_STRFUNC
#include "apr_want.h"

#define DIRNAME "data"
#define FILENAME DIRNAME "/host.data"
#define CERTFILEDER DIRNAME "/*.der"
#define CERTFILEB64 DIRNAME "/*.b64"

#if APR_HAS_LDAP

/* Legacy API tests
 *
 * These are deprecated in 1.7, removed in v2.0
 */

static char ldap_host[256];

static int get_ldap_host(void)
{
    apr_status_t rv;
    apr_file_t *thefile = NULL;
    char *ptr;

    ldap_host[0] = '\0';
    rv = apr_file_open(&thefile, FILENAME, 
                       APR_FOPEN_READ,
                       APR_UREAD | APR_UWRITE | APR_GREAD, p);
    if (rv != APR_SUCCESS) {
        return 0;
    }

    rv = apr_file_gets(ldap_host, sizeof(ldap_host), thefile);
    if (rv != APR_SUCCESS) {
        return 0;
    }

    ptr = strstr (ldap_host, "\r\n");
    if (ptr) {
        *ptr = '\0';
    }
    apr_file_close(thefile);

    return 1;

}

static int add_ldap_certs(abts_case *tc)
{
    apr_status_t status;
    apr_dir_t *thedir;
    apr_finfo_t dirent;
    apr_ldap_err_t *result = NULL;

    if ((status = apr_dir_open(&thedir, DIRNAME, p)) == APR_SUCCESS) {
        apr_ldap_opt_tls_cert_t *cert = (apr_ldap_opt_tls_cert_t *)apr_pcalloc(p, sizeof(apr_ldap_opt_tls_cert_t));

        do {
            status = apr_dir_read(&dirent, APR_FINFO_MIN | APR_FINFO_NAME, thedir);
            if (APR_STATUS_IS_INCOMPLETE(status)) {
                continue; /* ignore un-stat()able files */
            }
            else if (status != APR_SUCCESS) {
                break;
            }

            if (strstr(dirent.name, ".der")) {
                cert->type = APR_LDAP_CA_TYPE_DER;
                cert->path = apr_pstrcat (p, DIRNAME, "/", dirent.name, NULL);
                apr_ldap_set_option(p, NULL, APR_LDAP_OPT_TLS_CERT, (void *)cert, &result);
                ABTS_TRUE(tc, result->rc == LDAP_SUCCESS);
            }
            if (strstr(dirent.name, ".b64")) {
                cert->type = APR_LDAP_CA_TYPE_BASE64;
                cert->path = apr_pstrcat (p, DIRNAME, "/", dirent.name, NULL);
                apr_ldap_set_option(p, NULL, APR_LDAP_OPT_TLS_CERT, (void *)cert, &result);
                ABTS_TRUE(tc, result->rc == LDAP_SUCCESS);
            }

        } while (1);

        apr_dir_close(thedir);
    }
    return 0;
}

static void test_legacy_ldap_connection(abts_case *tc, LDAP *ldap)
{
    int version  = LDAP_VERSION3;
    int failures, result;
    
    /* always default to LDAP V3 */
    ldap_set_option(ldap, LDAP_OPT_PROTOCOL_VERSION, &version);

    for (failures=0; failures<10; failures++)
    {
        result = ldap_simple_bind_s(ldap,
                                    (char *)NULL,
                                    (char *)NULL);
        if (LDAP_SERVER_DOWN != result)
            break;
    }

    ABTS_TRUE(tc, result == LDAP_SUCCESS);
    if (result != LDAP_SUCCESS) {
        abts_log_message("%s\n", ldap_err2string(result));
    }

    ldap_unbind_s(ldap);

    return;
}

static void test_legacy_ldap(abts_case *tc, void *data)
{
    apr_pool_t *pool = p;
    LDAP *ldap;
    apr_ldap_err_t *result = NULL;


    ABTS_ASSERT(tc, "failed to get host", ldap_host[0] != '\0');
    
    apr_ldap_init(pool, &ldap,
                  ldap_host, LDAP_PORT,
                  APR_LDAP_NONE, &(result));

    ABTS_TRUE(tc, ldap != NULL);
    ABTS_PTR_NOTNULL(tc, result);

    if (result->rc == LDAP_SUCCESS) {
        test_legacy_ldap_connection(tc, ldap);
    }
}

static void test_legacy_ldaps(abts_case *tc, void *data)
{
    apr_pool_t *pool = p;
    LDAP *ldap;
    apr_ldap_err_t *result = NULL;

    apr_ldap_init(pool, &ldap,
                  ldap_host, LDAPS_PORT,
                  APR_LDAP_SSL, &(result));

    ABTS_TRUE(tc, ldap != NULL);
    ABTS_PTR_NOTNULL(tc, result);

    if (result->rc == LDAP_SUCCESS) {
        add_ldap_certs(tc);

        test_legacy_ldap_connection(tc, ldap);
    }
}

static void test_legacy_ldap_tls(abts_case *tc, void *data)
{
    apr_pool_t *pool = p;
    LDAP *ldap;
    apr_ldap_err_t *result = NULL;

    apr_ldap_init(pool, &ldap,
                  ldap_host, LDAP_PORT,
                  APR_LDAP_STARTTLS, &(result));

    ABTS_TRUE(tc, ldap != NULL);
    ABTS_PTR_NOTNULL(tc, result);

    if (result->rc == LDAP_SUCCESS) {
        add_ldap_certs(tc);

        test_legacy_ldap_connection(tc, ldap);
    }
}


/* API tests
 *
 * These are supported in 1.7 and higher.
 */

static apr_status_t bind_interact_simple(apr_ldap_t *ld, unsigned int flags, apr_ldap_bind_interact_t *interact, void *ctx)
{ 

    switch (interact->id) {
    case APR_LDAP_INTERACT_DN: {
        char *dn = getenv("TESTLDAP_DN");
        apr_buffer_str_set(&interact->result, dn ? dn : "", APR_BUFFER_STRING);
        break;
    }
    case APR_LDAP_INTERACT_PASS: {
        char *pass = getenv("TESTLDAP_PASS");
        apr_buffer_str_set(&interact->result, pass ? pass : "opensesame", APR_BUFFER_STRING);
        break;
    }
    default:
        break;
    }

    return APR_SUCCESS;
} 
  
static apr_status_t bind_interact_external(apr_ldap_t *ld, unsigned int flags, apr_ldap_bind_interact_t *interact, void *ctx)
{
    return APR_SUCCESS;
}

static apr_status_t bind_interact_plain(apr_ldap_t *ld, unsigned int flags, apr_ldap_bind_interact_t *interact, void *ctx)
{

    switch (interact->id) {
    case APR_LDAP_INTERACT_USER: {
        char *user = getenv("TESTLDAP_USER");
        apr_buffer_str_set(&interact->result, user ? user : "", APR_BUFFER_STRING);
        break;
    }   
    case APR_LDAP_INTERACT_AUTHNAME: {
        char *authname = getenv("TESTLDAP_AUTHNAME");
        apr_buffer_str_set(&interact->result, authname ? authname : "", APR_BUFFER_STRING);
        break;
    }
    case APR_LDAP_INTERACT_PASS: {
        char *pass = getenv("TESTLDAP_PASS");
        apr_buffer_str_set(&interact->result, pass ? pass : "opensesame", APR_BUFFER_STRING);
        break;
    }    
    default:
        break;
    }   

    return APR_SUCCESS;
}

static apr_status_t bind_interact_abandon(apr_ldap_t *ld, unsigned int flags, apr_ldap_bind_interact_t *interact, void *ctx)
{

    switch (interact->id) {
    case APR_LDAP_INTERACT_USER: {
        apr_buffer_str_set(&interact->result, "cn=notexist", APR_BUFFER_STRING);
        break;
    }
    case APR_LDAP_INTERACT_AUTHNAME: {
        apr_buffer_str_set(&interact->result, "cn=notexist", APR_BUFFER_STRING);
        break;
    }
    case APR_LDAP_INTERACT_PASS: {
        apr_buffer_str_set(&interact->result, "wrongpassword", APR_BUFFER_STRING);
        break;
    }
    default:
        break;
    }

    return APR_SUCCESS;
}

typedef struct test_ldap_connection_t {
    abts_case *tc;
    apr_pool_t *pool;
    apr_ldap_t *ldap;
    apr_pool_t *bind_pool;
    const char *mech;
    apr_ldap_bind_interact_cb *interact;
    apr_pollcb_t *poll;
    apr_pollfd_t socket_read;
    apr_pollfd_t socket_write;
    apu_err_t err;
    int must_bind;
} test_ldap_connection_t;

static apr_status_t test_ldap_compare_cb(apr_ldap_t *ldap,
                                         apr_status_t status,
                                         const char *matcheddn,
                                         apr_ldap_control_t **serverctrls,
                                         void *ctx, apu_err_t *err)
{
    test_ldap_connection_t *test = (test_ldap_connection_t *)ctx;

    /*
     * Step 13: compare result callback triggered, result is complete.
     */

    abts_log_message("comparison matcheddn: \n", matcheddn);

    ABTS_INT_EQUAL(test->tc, APR_COMPARE_TRUE, status);

    return APR_SUCCESS;
}      

static apr_status_t test_ldap_search_entry_cb(apr_ldap_t *ldap,
                                              const char *dn,
                                              int eidx,
                                              int nattrs,
                                              int aidx,
                                              const char *attr,
                                              int nvals,
                                              int vidx,
                                              apr_buffer_t *val,
                                              int binary,
                                              void *ctx, apu_err_t *err)
{
    test_ldap_connection_t *test = (test_ldap_connection_t *)ctx;

    /*
     * Step 8: search entry callback triggered, start processing results.
     */

    if (!nattrs && !vidx && attr) {
        /* first attribute and first value and attr present? output dn */
        abts_log_message("dn: %s", dn);
    }

    if (val) {
        abts_log_message("%s: %s", attr, apr_buffer_pstrdup(test->pool, val));
    }

    return APR_SUCCESS;
}


static apr_status_t test_ldap_search_result_cb(apr_ldap_t *ldap, 
                                               apr_status_t status,
                                               apr_size_t nentries,
                                               const char *matcheddn,
                                               apr_ldap_control_t **serverctrls,
                                               void *ctx, apu_err_t *err)
{
    char errbuf[128];
    test_ldap_connection_t *test = (test_ldap_connection_t *)ctx;

    /*
     * Step 9: search result callback triggered, finish up.
     */

    abts_log_message("\n");

    ABTS_INT_EQUAL(test->tc, APR_SUCCESS, status);

    if (APR_SUCCESS == status) {

        /*
         * Step 10: search successful, it is time to trigger the compare.
         */

        apr_buffer_t val;
        apr_buffer_str_set(&val, "top", APR_BUFFER_STRING);

        /*
         * Step 11: we're writable, trigger a compare, then wait for readable.
         */

        status = apr_ldap_compare(test->pool, test->ldap, "" /* root dn */,
                                 "objectclass" /* attribute */, &val /* value */,
                                 NULL /* serverctls */, NULL /* clientctls */,
                                 apr_time_from_sec(5) /* timeout */,
                                 test_ldap_compare_cb, test, &test->err);

        ABTS_INT_EQUAL(test->tc, APR_WANT_READ, status);

        switch (status) {
        case APR_SUCCESS:
        case APR_WANT_READ:
        case APR_WANT_WRITE:
            break;
        default:
            abts_log_message("apr_ldap_result: %s [%s]\n", test->err.reason,
                              apu_strerror(status, errbuf, sizeof(errbuf)));
        }

    }
    else {
        abts_log_message("apr_ldap_search() failed: %s\n", apu_strerror(status, errbuf, sizeof(errbuf)));
    }

    return status;
}

static apr_status_t test_ldap_bind_cb(apr_ldap_t *ldap, apr_status_t status,
                                         const char *matcheddn,
                                         apr_ldap_control_t **serverctrls,
                                         void *ctx, apu_err_t *err)
{
    char errbuf[128];
    test_ldap_connection_t *test = (test_ldap_connection_t *)ctx;

    if (APR_SUCCESS == status) {

        /*
         * Step 5: bind successful, it is time to trigger the search.
         */

        const char *attrs[2];
        attrs[0] = "+";
        attrs[1] = NULL;

        /*
         * Step 6: we're writable, trigger a search, then wait for readable.
         */

        status = apr_ldap_search(test->pool, test->ldap, "" /* root dn */,
                                 APR_LDAP_SCOPE_BASE, "(objectclass=*)" /* filter */,
                                 attrs /* attrs */, APR_LDAP_OPT_OFF /* attrsonly */,
                                 NULL /* serverctls */, NULL /* clientctls */,
                                 apr_time_from_sec(5) /* timelimit */, 0 /* sizelimit */,
                                 test_ldap_search_result_cb, test_ldap_search_entry_cb, test, &test->err);

        ABTS_INT_EQUAL(test->tc, APR_WANT_READ, status);

        switch (status) {
        case APR_SUCCESS:
        case APR_WANT_READ:
        case APR_WANT_WRITE:
            break;
        default:
            abts_log_message("apr_ldap_result: %s [%s]\n", test->err.reason,
                              apu_strerror(status, errbuf, sizeof(errbuf)));
        }

    }
    else {
        abts_log_message("apr_ldap_bind() failed: %s\n", apu_strerror(status, errbuf, sizeof(errbuf)));
    }

    return status;
}

static apr_status_t test_ldap_initialise_cb(apr_ldap_t *ldap, apr_status_t status,
                                            void *ctx, apu_err_t *err)
{
    char errbuf[128];
    test_ldap_connection_t *test = (test_ldap_connection_t *)ctx;

    if (APR_SUCCESS == status) {

        /*
         * Step 2: we're writable, trigger the initial bind, then wait for readable.
         */

        status = apr_ldap_bind(test->pool, test->ldap, test->mech, test->interact, NULL,
                               apr_time_from_sec(0), test_ldap_bind_cb, test, &test->err);

        switch (status) {
        case APR_SUCCESS:
        case APR_WANT_READ:
        case APR_WANT_WRITE:
            return status;
        default:
            abts_log_message("apr_ldap_result: %s [%s]\n", test->err.reason,
                              apu_strerror(status, errbuf, sizeof(errbuf)));
            return status;
        }

    }
    else {
        abts_log_message("initialise failed: %s\n", apu_strerror(status, errbuf, sizeof(errbuf)));
    }

    return status;
}

static apr_status_t test_ldap_connection_cb(void *baton, apr_pollfd_t *descriptor)
{
    char errbuf[128];
    test_ldap_connection_t *test = (test_ldap_connection_t *) baton;

    apr_status_t status = APR_SUCCESS;

    /* remove our event */
    apr_pollcb_remove(test->poll, descriptor);

    /* are we ready to write? */
    if (descriptor->rtnevents & APR_POLLOUT) {

        /*
         * Step 4: bind callback triggered, continue the bind processing.
         */

        /* handle oustanding tasks */
        status = apr_ldap_process(test->pool, test->ldap, apr_time_from_sec(0), &test->err);

        switch (status) {
        case APR_SUCCESS:
        case APR_WANT_READ:
        case APR_WANT_WRITE:
            break;
        default:
            abts_log_message("apr_ldap_result: %s [%s]\n", test->err.reason,
                              apu_strerror(status, errbuf, sizeof(errbuf)));
        }

    }

    /* are we ready to read? */
    else if (descriptor->rtnevents & APR_POLLIN) {

        /*
         * Step 3: we're readable, fetch the bind result, then request a write to be scheduled.
         */

        /*
         * Step 7: we're readable, fetch the search result, then trigger search callback.
         */

        /*
         * Step 12: we're readable, fetch the compare result, then trigger compare callback.
         */

        status = apr_ldap_result(test->pool, test->ldap, -1, &test->err);

        switch (status) {
        case APR_SUCCESS:
        case APR_WANT_READ:
        case APR_WANT_WRITE:
            break;
        default:
            abts_log_message("apr_ldap_result: %s [%s]\n", test->err.reason,
                              apu_strerror(status, errbuf, sizeof(errbuf)));
        }

    }

    return status;
}

/*
 * Asynchronous test of an LDAP connection.
 *
 * Like all async code, we jump around a lot. In this example we make
 * an attempt to bind, and that that was successful, we make an
 * attempt to search for the root DSE, then if that was successful,
 * we attempt a compare.
 *
 * Start at "Step 1" and follow each step through the callbacks.
 */

static void test_ldap_connection(abts_case *tc, apr_pool_t *pool, apr_ldap_t *ldap,
                                 const char *mech, apr_ldap_bind_interact_cb *interact)
{
    char errbuf[128];
    apu_err_t err;
    test_ldap_connection_t test;
    apr_ldap_opt_t opt;
    apr_status_t status;

    memset(&test, 0, sizeof(test_ldap_connection_t));
 
    test.tc = tc;
    test.pool = pool;
    test.ldap = ldap;
    test.mech = mech;
    test.interact = interact;

    /* always default to LDAP V3 */
    opt.pv = APR_LDAP_VERSION3;
    apr_ldap_option_set(pool, ldap, APR_LDAP_OPT_PROTOCOL_VERSION, &opt, &err);


    /*
     * Step 1: call us back when we're writable.
     */

    /* set the initialise callback */
    apr_ldap_prepare(pool, ldap, test_ldap_initialise_cb, &test);


    apr_ldap_option_get(pool, ldap, APR_LDAP_OPT_HANDLE, &opt, &err);
    ABTS_TRUE(tc, opt.handle != NULL);

    status = apr_ldap_connect(pool, ldap, apr_time_from_sec(5), &err);

    if (APR_SUCCESS != status) {
        abts_log_message("%s - %s (%d)\n", err.reason, err.msg, err.rc);
        return;
    }


    status = apr_ldap_option_get(pool, ldap, APR_LDAP_OPT_DESC, &opt, &err);
    ABTS_TRUE(tc, status == APR_SUCCESS);
    ABTS_ASSERT(tc, "failed to get descriptor", opt.socket != NULL);

    status = apr_pollcb_create(&test.poll, 1, pool, APR_POLLSET_DEFAULT);
    if (APR_ENOTIMPL == status) {
        abts_log_message("apr_pollcb_create() not implemented on this platform, skipping test\n");
        return;
    }
    ABTS_TRUE(tc, status == APR_SUCCESS);
    if (APR_SUCCESS != status) {
        return;
    }

    /* set up read descriptor */
    test.socket_read.desc_type = APR_POLL_SOCKET;
    test.socket_read.reqevents = APR_POLLIN;
    test.socket_read.desc.s = opt.socket;
    test.socket_read.client_data = opt.socket;

    /* set up write descriptor */
    test.socket_write.desc_type = APR_POLL_SOCKET;
    test.socket_write.reqevents = APR_POLLOUT;
    test.socket_write.desc.s = opt.socket;
    test.socket_write.client_data = opt.socket;

    status = APR_WANT_WRITE;

    do {

        if (APR_SUCCESS == status) {
            abts_log_message("apr_pollcb_poll() complete\n");
            break;
        }
        else if (APR_WANT_READ == status) {

            /* wait for socket to be readable, then process another result */
            status = apr_pollcb_add(test.poll, &test.socket_read);
            ABTS_INT_EQUAL(tc, APR_SUCCESS, status);
            if (APR_SUCCESS != status) {
                break;
            }

        }
        else if (APR_WANT_WRITE == status) {

            /* wait for socket to be writeable, then process result */
            status = apr_pollcb_add(test.poll, &test.socket_write);
            ABTS_INT_EQUAL(tc, APR_SUCCESS, status);
            if (APR_SUCCESS != status) {
                break;
            }

        }
        else {
            abts_log_message("apr_ldap_process/apr_ldap_result: %s [%s]\n", test.err.reason,
                              apu_strerror(status, errbuf, sizeof(errbuf)));
            break;
        }

        status = apr_pollcb_poll(test.poll, -1, test_ldap_connection_cb, &test);

    } while (1);


    /*
     * Step 14: bind, search, compare, result is complete, event loop unwound.
     */

    return;
}

/*
 * Synchronous test of an LDAP connection.
 *
 * The code below is identical to the above code, but uses the event loop inside
 * apr_ldap_poll().
 *
 * The initial callback test_ldap_initialise_cb is set, all queries are bootstrapped
 * from this callback.
 */

#if 0
static void test_ldap_connection(abts_case *tc, apr_pool_t *pool, apr_ldap_t *ldap,
                                 const char *mech, apr_ldap_bind_interact_cb *interact)
{
    char errbuf[128];
    apu_err_t err;
    test_ldap_connection_t test;
    apr_ldap_opt_t opt;
    apr_status_t status;

    memset(&test, 0, sizeof(test_ldap_connection_t));

    test.tc = tc;
    test.pool = pool;
    test.ldap = ldap;
    test.mech = mech;
    test.interact = interact;


    /* always default to LDAP V3 */
    opt.pv = APR_LDAP_VERSION3;
    apr_ldap_option_set(pool, ldap, APR_LDAP_OPT_PROTOCOL_VERSION, &opt, &err);


    /* set the initialise callback */
    apr_ldap_prepare(pool, ldap, test_ldap_initialise_cb, &test);


    /* initial connect */
    status = apr_ldap_connect(pool, ldap, apr_time_from_sec(5), &err);

    if (APR_SUCCESS != status) {
        abts_log_message("%s - %s (%d)\n", err.reason, err.msg, err.rc);
        return;
    }


    /* run the callbacks */
    status = apr_ldap_poll(pool, ldap, apr_time_from_sec(5), &err);

    if (APR_SUCCESS == status) {
        abts_log_message("apr_pollcb_poll() complete\n");
    }
    else {
        abts_log_message("apr_ldap_process/apr_ldap_result: %s [%s]\n", test.err.reason,
                              apu_strerror(status, errbuf, sizeof(errbuf)));
    }


    return;
}
#endif


static apr_status_t test_ldap_abandon_cb(void *baton, apr_pollfd_t *descriptor)
{
    char errbuf[128];
    test_ldap_connection_t *test = (test_ldap_connection_t *) baton;

    apr_status_t status = APR_SUCCESS;

    /* remove our event */
    apr_pollcb_remove(test->poll, descriptor);

    /* are we ready to write? */
    if (descriptor->rtnevents & APR_POLLOUT) {

        if (test->must_bind) {

            /*
             * Step B: we're writable, trigger the initial bind from the bind_pool, then wait for readable.
             */

            status = apr_ldap_bind(test->bind_pool, test->ldap, test->mech, test->interact, NULL, apr_time_from_sec(0), test_ldap_bind_cb, test, &test->err);

            test->must_bind = 0;

            ABTS_INT_EQUAL(test->tc, APR_WANT_READ, status);

            if (APR_SUCCESS == status) {
                /* should not have happened */
                return status;
            }
            else if (APR_WANT_READ == status) {

                /*
                 * Step C: spanner in the works - lets abandon the bind.
                 */

                apr_pool_destroy(test->bind_pool);

                test->bind_pool = NULL;

                abts_log_message("bind abandon triggered\n");

                /* and then continue as if nothing had happened */

                return status;

            }
            else {
                return status;
            }

        }

        /*
         * Step E: write callback triggered, send the abandon.
         */

        /* handle oustanding tasks */
        status = apr_ldap_process(test->pool, test->ldap, apr_time_from_sec(0), &test->err);

        ABTS_INT_EQUAL(test->tc, APR_SUCCESS, status);

        switch (status) {
        case APR_SUCCESS:
        case APR_WANT_READ:
        case APR_WANT_WRITE:
            return status;
        default:
            abts_log_message("apr_ldap_result: %s [%s]\n", test->err.reason,
                              apu_strerror(status, errbuf, sizeof(errbuf)));
            return status;
        }

    }

    /* are we ready to read? */
    if (descriptor->rtnevents & APR_POLLIN) {

        /*
         * Step D: we're readable, but our bind has been abandoned.
         */

        status = apr_ldap_result(test->pool, test->ldap, -1, &test->err);

        ABTS_INT_EQUAL(test->tc, APR_WANT_WRITE, status);

        switch (status) {
        case APR_SUCCESS:
        case APR_WANT_READ:
        case APR_WANT_WRITE:
            return status;
        default:
            abts_log_message("apr_ldap_result: %s [%s]\n", test->err.reason,
                              apu_strerror(status, errbuf, sizeof(errbuf)));
            return status;
        }

    }

    return status;
}

static void test_ldap_abandon(abts_case *tc, apr_pool_t *pool, apr_ldap_t *ldap)
{
    char errbuf[128];
    apu_err_t err;
    test_ldap_connection_t test;
    apr_ldap_opt_t opt;
    apr_status_t status;

    memset(&test, 0, sizeof(test_ldap_connection_t));

    test.tc = tc;
    test.pool = pool;
    test.ldap = ldap;
    test.mech = "PLAIN";
    test.interact = bind_interact_abandon;

    test.must_bind = 1;

    apr_pool_create(&test.bind_pool, test.pool);


    /* always default to LDAP V3 */
    opt.pv = APR_LDAP_VERSION3;
    apr_ldap_option_set(pool, ldap, APR_LDAP_OPT_PROTOCOL_VERSION, &opt, &err);

    apr_ldap_option_get(pool, ldap, APR_LDAP_OPT_HANDLE, &opt, &err);
    ABTS_TRUE(tc, opt.handle != NULL);

    status = apr_ldap_connect(pool, ldap, apr_time_from_sec(5), &err);

    if (APR_SUCCESS != status) {
        abts_log_message("%s - %s (%d)\n", err.reason, err.msg, err.rc);
        return;
    }

    status = apr_ldap_option_get(pool, ldap, APR_LDAP_OPT_DESC, &opt, &err);
    ABTS_TRUE(tc, status == APR_SUCCESS);
    ABTS_ASSERT(tc, "failed to get descriptor", opt.socket != NULL);

    status = apr_pollcb_create(&test.poll, 1, pool, APR_POLLSET_DEFAULT);
    if (APR_ENOTIMPL == status) {
        abts_log_message("apr_pollcb_create() not implemented on this platform, skipping test\n");
        return;
    }
    ABTS_TRUE(tc, status == APR_SUCCESS);
    if (APR_SUCCESS != status) {
        return;
    }

    /* set up read descriptor */
    test.socket_read.desc_type = APR_POLL_SOCKET;
    test.socket_read.reqevents = APR_POLLIN;
    test.socket_read.desc.s = opt.socket;
    test.socket_read.client_data = opt.socket;

    /* set up write descriptor */
    test.socket_write.desc_type = APR_POLL_SOCKET;
    test.socket_write.reqevents = APR_POLLOUT;
    test.socket_write.desc.s = opt.socket;
    test.socket_write.client_data = opt.socket;


    /*
     * Step A: call us back when we're writable.
     */

    status = APR_WANT_WRITE;

    do {

        if (APR_SUCCESS == status) {
            abts_log_message("apr_pollcb_poll() complete\n");
            break;
        }
        else if (APR_WANT_READ == status) {

            /* wait for socket to be readable, then process another result */
            status = apr_pollcb_add(test.poll, &test.socket_read);
            ABTS_INT_EQUAL(tc, APR_SUCCESS, status);
            if (APR_SUCCESS != status) {
                break;
            }

        }
        else if (APR_WANT_WRITE == status) {

            /* wait for socket to be writeable, then process result */
            status = apr_pollcb_add(test.poll, &test.socket_write);
            ABTS_INT_EQUAL(tc, APR_SUCCESS, status);
            if (APR_SUCCESS != status) {
                break;
            }

        }
        else {
            abts_log_message("apr_ldap_result: %s [%s]\n", test.err.reason,
                              apu_strerror(status, errbuf, sizeof(errbuf)));
            break;
        }

        status = apr_pollcb_poll(test.poll, -1, test_ldap_abandon_cb, &test);

    } while (1);

    /*
     * Step F: bind, result, abandon is complete, event loop unwound.
     */

    return;
}

static void test_ldap_global_opts(abts_case *tc, void *data)
{
    apr_pool_t *pool;
    apu_err_t err;
    apr_ldap_opt_t opt;
    apr_status_t status;

    char errbuf[128];

    apr_pool_create(&pool, p);

    status = apr_ldap_option_get(pool, NULL, APR_LDAP_OPT_API_INFO, &opt, &err);

    if (status != APR_SUCCESS) {
        abts_log_message("apr_ldap_option_get: %s [%s]\n", err.reason,
                          apu_strerror(status, errbuf, sizeof(errbuf)));
    }

    ABTS_TRUE(tc, status == APR_SUCCESS);
    ABTS_TRUE(tc, err.rc == 0);
    ABTS_TRUE(tc, opt.info.vendor_name != NULL);

    opt.ldfi.name = "THREAD_SAFE";
    status = apr_ldap_option_get(pool, NULL, APR_LDAP_OPT_API_FEATURE_INFO, &opt, &err);
    /* feature may exist, or may not */
    ABTS_TRUE(tc, status == APR_SUCCESS || status == APR_EINVAL);

    status = apr_ldap_option_get(pool, NULL, APR_LDAP_OPT_DEBUG_LEVEL, &opt, &err);
    if (APR_ENOTIMPL == status) {
        abts_log_message("apr_ldap_option_get(APR_LDAP_OPT_DEBUG_LEVEL) not implemented on this platform, skipping test\n");
        return; 
    }
    else { 
        ABTS_TRUE(tc, status == APR_SUCCESS);
    } 

    apr_pool_destroy(pool);
}

static void test_ldap_opts(abts_case *tc, void *data)
{
    apr_pool_t *pool;
    apr_ldap_t *ldap = NULL;
    apu_err_t err;
    const char *url;

    apr_pool_create(&pool, p);

    url = apr_psprintf(pool, "ldap://%s:%d", "localhost", APR_LDAP_PORT);

    apr_ldap_initialise(pool, &ldap, &(err));

    ABTS_TRUE(tc, ldap != NULL);

    if (ldap) {

        apr_status_t status;
        apr_ldap_opt_t opt;

        opt.uri = url;
        status = apr_ldap_option_set(pool, ldap, APR_LDAP_OPT_URI, &opt, &err);
        ABTS_TRUE(tc, status == APR_SUCCESS); 
        status = apr_ldap_option_get(pool, ldap, APR_LDAP_OPT_URI, &opt, &err);
        ABTS_TRUE(tc, status == APR_SUCCESS);
        ABTS_ASSERT(tc, "failed to set uri", !strcmp(opt.uri, url));

        opt.pv = APR_LDAP_VERSION3;
        status = apr_ldap_option_set(pool, ldap, APR_LDAP_OPT_PROTOCOL_VERSION, &opt, &err);
        ABTS_TRUE(tc, status == APR_SUCCESS);
        status = apr_ldap_option_get(pool, ldap, APR_LDAP_OPT_PROTOCOL_VERSION, &opt, &err);
        ABTS_TRUE(tc, status == APR_SUCCESS);
        ABTS_ASSERT(tc, "failed to set protocol version", opt.pv == APR_LDAP_VERSION3);

        opt.deref = APR_LDAP_DEREF_ALWAYS;
        status = apr_ldap_option_set(pool, ldap, APR_LDAP_OPT_DEREF, &opt, &err);
        ABTS_TRUE(tc, status == APR_SUCCESS);
        status = apr_ldap_option_get(pool, ldap, APR_LDAP_OPT_DEREF, &opt, &err);
        ABTS_TRUE(tc, status == APR_SUCCESS);
        ABTS_ASSERT(tc, "failed to set deref", opt.deref == APR_LDAP_DEREF_ALWAYS);

        opt.refs = APR_LDAP_OPT_ON;
        status = apr_ldap_option_set(pool, ldap, APR_LDAP_OPT_REFERRALS, &opt, &err);
        ABTS_TRUE(tc, status == APR_SUCCESS);
        status = apr_ldap_option_get(pool, ldap, APR_LDAP_OPT_REFERRALS, &opt, &err);
        ABTS_TRUE(tc, status == APR_SUCCESS);
        ABTS_ASSERT(tc, "failed to set referrals", opt.refs == APR_LDAP_OPT_ON);

#if 0
        opt.refhoplimit = 5;
        status = apr_ldap_option_set(pool, ldap, APR_LDAP_OPT_REFHOPLIMIT, &opt, &err);
        if (APR_ENOTIMPL != status) {
            ABTS_TRUE(tc, status == APR_SUCCESS); 
            status = apr_ldap_option_get(pool, ldap, APR_LDAP_OPT_REFHOPLIMIT, &opt, &err);
            ABTS_TRUE(tc, status == APR_SUCCESS); 
            ABTS_ASSERT(tc, "failed to set refhoplimit", opt.refhoplimit == 5);
        }
#endif

    }

    apr_pool_destroy(pool);
}

static void test_ldap(abts_case *tc, void *data)
{
    apr_pool_t *pool;
    apr_ldap_t *ldap;
    apu_err_t err;
    const char *url;
    apr_ldap_opt_t opt;
    apr_status_t status;

    const char *testldap = getenv("TESTLDAP");

    if (!testldap) {
        ABTS_NOT_IMPL(tc, "Environment TESTLDAP unset - skipping");
        return;
    }

    apr_pool_create(&pool, p);

    url = apr_psprintf(pool, "ldap://%s:%d", testldap, APR_LDAP_PORT);
    
    apr_ldap_initialise(pool, &ldap, &(err));

    ABTS_TRUE(tc, ldap != NULL);

    opt.uri = url;
    status = apr_ldap_option_set(pool, ldap, APR_LDAP_OPT_URI, &opt, &err);

    if (status == APR_SUCCESS) {
        test_ldap_connection(tc, pool, ldap, NULL, bind_interact_simple);
    }

    apr_pool_destroy(pool);
}

static void test_ldaps(abts_case *tc, void *data)
{
    apr_pool_t *pool;
    apr_ldap_t *ldap;
    apu_err_t err;
    const char *url; 
    apr_ldap_opt_t opt;
    apr_status_t status;

    const char *testldaps = getenv("TESTLDAPS");

    if (!testldaps) {
        ABTS_NOT_IMPL(tc, "Environment TESTLDAPS unset - skipping");
        return;
    }

    apr_pool_create(&pool, p);

    url = apr_psprintf(pool, "ldaps://%s:%d", testldaps, APR_LDAPS_PORT);

    apr_ldap_initialise(pool, &ldap, &(err));

    ABTS_TRUE(tc, ldap != NULL);

    opt.uri = url;
    status = apr_ldap_option_set(pool, ldap, APR_LDAP_OPT_URI, &opt, &err);

    if (status == APR_SUCCESS) {
        add_ldap_certs(tc);

        test_ldap_connection(tc, pool, ldap, "EXTERNAL", bind_interact_external);
        test_ldap_connection(tc, pool, ldap, "PLAIN", bind_interact_plain);
        test_ldap_connection(tc, pool, ldap, NULL, bind_interact_simple);
    }

    apr_pool_destroy(pool);
}

static void test_ldap_tls(abts_case *tc, void *data)
{
    apr_pool_t *pool;
    apr_ldap_t *ldap;
    apu_err_t err;
    const char *url;
    apr_ldap_opt_t opt;
    apr_status_t status;

    const char *testldap_tls = getenv("TESTLDAP_TLS");

    if (!testldap_tls) {
        ABTS_NOT_IMPL(tc, "Environment TESTLDAP_TLS unset - skipping");
        return;
    }

    apr_pool_create(&pool, p);

    opt.tls = APR_LDAP_TLS_STARTTLS;

    url = apr_psprintf(pool, "ldap://%s:%d", testldap_tls, APR_LDAP_PORT);

    apr_ldap_initialise(pool, &ldap, &(err));

    if (err.rc == APR_SUCCESS) {
        apr_ldap_option_set(pool, ldap, APR_LDAP_OPT_TLS, &opt, &(err));
    }

    ABTS_TRUE(tc, ldap != NULL);

    opt.uri = url;
    status = apr_ldap_option_set(pool, ldap, APR_LDAP_OPT_URI, &opt, &err);

    if (status == APR_SUCCESS) {
        add_ldap_certs(tc);

        test_ldap_connection(tc, pool, ldap, "EXTERNAL", bind_interact_external);
        test_ldap_connection(tc, pool, ldap, "PLAIN", bind_interact_plain);
        test_ldap_connection(tc, pool, ldap, NULL, bind_interact_simple);
    }

    apr_pool_destroy(pool);
}

static void test_ldapi(abts_case *tc, void *data)
{
    apr_pool_t *pool;
    apr_ldap_t *ldap;
    apu_err_t err;
    const char *url;
    apr_ldap_opt_t opt;
    apr_status_t status;

    const char *testldapi = getenv("TESTLDAPI");

    if (!testldapi) {
        ABTS_NOT_IMPL(tc, "Environment TESTLDAPI unset - skipping");
        return;
    }

    apr_pool_create(&pool, p);

    opt.tls = APR_LDAP_TLS_STARTTLS;

    url = apr_psprintf(pool, "ldapi://%s", apr_pescape_urlencoded(pool, testldapi));

    apr_ldap_initialise(pool, &ldap, &(err));

    ABTS_TRUE(tc, ldap != NULL);

    opt.uri = url;
    status = apr_ldap_option_set(pool, ldap, APR_LDAP_OPT_URI, &opt, &err);
    ABTS_TRUE(tc, status == APR_SUCCESS);

    abts_log_message("Initialiased LDAP url [%s]: %s - %s (%d)\n", url, err.reason, err.msg, err.rc);

    if (status == APR_SUCCESS) {
        test_ldap_connection(tc, pool, ldap, "EXTERNAL", bind_interact_external);
        test_ldap_connection(tc, pool, ldap, "PLAIN", bind_interact_plain);
        test_ldap_connection(tc, pool, ldap, NULL, bind_interact_simple);

        test_ldap_abandon(tc, pool, ldap);
    }

    status = apr_ldap_unbind(ldap, NULL, NULL, &err);
    ABTS_TRUE(tc, status == APR_SUCCESS);

    apr_pool_destroy(pool);
}

#endif /* APR_HAS_LDAP */

abts_suite *testldap(abts_suite *suite)
{
#if APR_HAS_LDAP
    apr_ldap_err_t *result = NULL;
    suite = ADD_SUITE(suite);

    /* legacy tests */

    apr_ldap_ssl_init(p, NULL, 0, &result);

    if (get_ldap_host()) {
        abts_run_test(suite, test_legacy_ldap, NULL);
        abts_run_test(suite, test_legacy_ldaps, NULL);
        abts_run_test(suite, test_legacy_ldap_tls, NULL);
    }

    /* api tests */

    abts_run_test(suite, test_ldap_global_opts, NULL);
    abts_run_test(suite, test_ldap_opts, NULL);

    abts_run_test(suite, test_ldap, NULL);
    abts_run_test(suite, test_ldaps, NULL);
    abts_run_test(suite, test_ldap_tls, NULL);
    abts_run_test(suite, test_ldapi, NULL);

#endif /* APR_HAS_LDAP */

    return suite;
}

