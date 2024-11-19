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
* Michael Jerris <mike@jerris.com>
* Portions created by the Initial Developer are Copyright (C)
* the Initial Developer. All Rights Reserved.
*
* Contributor(s):
*
* Michael Jerris <mike@jerris.com>
*
*
* base_core_db.c -- sqlite wrapper and extensions
*
*/

#include <base.h>
#include "private/base_core_pvt.h"

#include <sqlite3.h>

static void db_pick_path(const char *dbname, char *buf, base_size_t size)
{
    memset(buf, 0, size);
    if (base_is_file_path(dbname)) {
        strncpy(buf, dbname, size-1);
    } else {
        base_snprintf(buf, size, "%s%s%s.db", BASE_GLOBAL_dirs.db_dir, BASE_PATH_SEPARATOR, dbname);
    }
}

BASE_DECLARE(int) base_core_db_open(const char *filename, base_core_db_t **ppDb)
{
    return sqlite3_open(filename, ppDb);
}

BASE_DECLARE(int) base_core_db_open_v2(const char *filename, base_core_db_t **ppDb)
{
    return sqlite3_open_v2(filename, ppDb, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_URI, NULL);
}

BASE_DECLARE(int) base_core_db_close(base_core_db_t *db)
{
    return sqlite3_close(db);
}

BASE_DECLARE(const unsigned char *) base_core_db_column_text(base_core_db_stmt_t *stmt, int iCol)
{
    return sqlite3_column_text(stmt, iCol);
}

BASE_DECLARE(const char *) base_core_db_column_name(base_core_db_stmt_t *stmt, int N)
{
    return sqlite3_column_name(stmt, N);
}

BASE_DECLARE(int) base_core_db_column_count(base_core_db_stmt_t *pStmt)
{
    return sqlite3_column_count(pStmt);
}

BASE_DECLARE(const char *) base_core_db_errmsg(base_core_db_t *db)
{
    return sqlite3_errmsg(db);
}

BASE_DECLARE(int) base_core_db_exec(base_core_db_t *db, const char *sql, base_core_db_callback_func_t callback, void *data, char **errmsg)
{
    int ret = 0;
    int sane = 300;
    char *err = NULL;

    while (--sane > 0) {
        ret = sqlite3_exec(db, sql, callback, data, &err);
        if (ret == SQLITE_BUSY || ret == SQLITE_LOCKED) {
            base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_WARNING, "SQLite is %s, sane=%d [%s]\n", (ret == SQLITE_BUSY ? "BUSY" : "LOCKED"), sane, sql);
            if (sane > 1) {
                base_core_db_free(err);
                base_yield(100000);
                continue;
            }
        } else {
            break;
        }
    }

    if (errmsg) {
        *errmsg = err;
    } else if (err) {
        base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_ERROR, "SQL ERR [%s]\n", err);
        base_core_db_free(err);
        err = NULL;
    }

    return ret;
}

BASE_DECLARE(int) base_core_db_finalize(base_core_db_stmt_t *pStmt)
{
    return sqlite3_finalize(pStmt);
}

BASE_DECLARE(int) base_core_db_prepare(base_core_db_t *db, const char *zSql, int nBytes, base_core_db_stmt_t **ppStmt, const char **pzTail)
{
    return sqlite3_prepare(db, zSql, nBytes, ppStmt, pzTail);
}

BASE_DECLARE(int) base_core_db_step(base_core_db_stmt_t *stmt)
{
    return sqlite3_step(stmt);
}

BASE_DECLARE(int) base_core_db_reset(base_core_db_stmt_t *pStmt)
{
    return sqlite3_reset(pStmt);
}

BASE_DECLARE(int) base_core_db_bind_int(base_core_db_stmt_t *pStmt, int i, int iValue)
{
    return sqlite3_bind_int(pStmt, i, iValue);
}

BASE_DECLARE(int) base_core_db_bind_int64(base_core_db_stmt_t *pStmt, int i, int64_t iValue)
{
    return sqlite3_bind_int64(pStmt, i, iValue);
}

BASE_DECLARE(int) base_core_db_bind_text(base_core_db_stmt_t *pStmt, int i, const char *zData, int nData, base_core_db_destructor_type_t xDel)
{
    return sqlite3_bind_text(pStmt, i, zData, nData, xDel);
}

BASE_DECLARE(int) base_core_db_bind_double(base_core_db_stmt_t *pStmt, int i, double dValue)
{
    return sqlite3_bind_double(pStmt, i, dValue);
}

BASE_DECLARE(int64_t) base_core_db_last_insert_rowid(base_core_db_t *db)
{
    return sqlite3_last_insert_rowid(db);
}

BASE_DECLARE(int) base_core_db_get_table(base_core_db_t *db, const char *sql, char ***resultp, int *nrow, int *ncolumn, char **errmsg)
{
    return sqlite3_get_table(db, sql, resultp, nrow, ncolumn, errmsg);
}

BASE_DECLARE(void) base_core_db_free_table(char **result)
{
    sqlite3_free_table(result);
}

BASE_DECLARE(void) base_core_db_free(char *z)
{
    sqlite3_free(z);
}

BASE_DECLARE(int) base_core_db_changes(base_core_db_t *db)
{
    return sqlite3_changes(db);
}

BASE_DECLARE(int) base_core_db_load_extension(base_core_db_t *db, const char *extension)
{
    int ret = 0;
    char *err = NULL;

#ifdef SQLITE_DBCONFIG_ENABLE_LOAD_EXTENSION
    sqlite3_db_config(db, SQLITE_DBCONFIG_ENABLE_LOAD_EXTENSION, 1, 0);
    ret = sqlite3_load_extension(db, extension, 0, &err);
#else
    sqlite3_enable_load_extension(db, 1);
    ret = sqlite3_load_extension(db, extension, 0, &err);
    sqlite3_enable_load_extension(db, 0);
#endif

    if (err) {
        base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_ERROR, "LOAD EXTENSION ERR [%s]\n", err);
        base_core_db_free(err);
        err = NULL;
    }
    return ret;
}

static int base_core_db_connection_setup(base_core_db_t *db, base_bool_t in_memory) {
    int db_ret;

    if ((db_ret = base_core_db_exec(db, "PRAGMA synchronous=OFF;", NULL, NULL, NULL)) != SQLITE_OK) {
        goto end;
    }
    if ((db_ret = base_core_db_exec(db, "PRAGMA count_changes=OFF;", NULL, NULL, NULL)) != SQLITE_OK) {
        goto end;
    }
    if ((db_ret = base_core_db_exec(db, "PRAGMA temp_store=MEMORY;", NULL, NULL, NULL)) != SQLITE_OK) {
        goto end;
    }
    if (!in_memory) {
        if ((db_ret = base_core_db_exec(db, "PRAGMA cache_size=8000;", NULL, NULL, NULL)) != SQLITE_OK) {
            goto end;
        }
    } else {
        if ((db_ret = base_core_db_exec(db, "PRAGMA cache_size=-8192;", NULL, NULL, NULL)) != SQLITE_OK) {
            goto end;
        }
        if ((db_ret = base_core_db_exec(db, "PRAGMA journal_mode=OFF;", NULL, NULL, NULL)) != SQLITE_OK) {
            goto end;
        }
    }

end:
    return db_ret;
}

BASE_DECLARE(base_core_db_t *) base_core_db_open_file(const char *filename)
{
    base_core_db_t *db;
    char path[1024];
    int db_ret;

    db_pick_path(filename, path, sizeof(path));
    if ((db_ret = base_core_db_open(path, &db)) != SQLITE_OK) {
        goto end;
    }
    if ((db_ret = base_core_db_connection_setup(db, BASE_FALSE)) != SQLITE_OK) {
        goto end;
    }

end:
    if (db_ret != SQLITE_OK) {
        base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_ERROR, "SQL ERR [%s]\n", base_core_db_errmsg(db));
        base_core_db_close(db);
        db = NULL;
    }
    return db;
}

BASE_DECLARE(base_core_db_t *) base_core_db_open_in_memory(const char *uri)
{
    base_core_db_t *db;
    int db_ret;

    if ((db_ret = base_core_db_open_v2(uri, &db)) != SQLITE_OK) {
        goto end;
    }
    if ((db_ret = base_core_db_connection_setup(db, BASE_TRUE)) != SQLITE_OK) {
        goto end;
    }

end:
    if (db_ret != SQLITE_OK) {
        base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_ERROR, "SQL ERR [%s]\n", base_core_db_errmsg(db));
        base_core_db_close(db);
        db = NULL;
    }
    return db;
}

BASE_DECLARE(void) base_core_db_test_reactive(base_core_db_t *db, char *test_sql, char *drop_sql, char *reactive_sql)
{
    char *errmsg;

    if (!base_test_flag((&runtime), SCF_CLEAR_SQL)) {
        return;
    }

    if (!base_test_flag((&runtime), SCF_AUTO_SCHEMAS)) {
        base_core_db_exec(db, test_sql, NULL, NULL, NULL);
        return;
    }


    if (db) {
        if (test_sql) {
            base_core_db_exec(db, test_sql, NULL, NULL, &errmsg);

            if (errmsg) {
                base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_DEBUG, "SQL ERR [%s]\n[%s]\nAuto Generating Table!\n", errmsg, test_sql);
                base_core_db_free(errmsg);
                errmsg = NULL;
                if (drop_sql) {
                    base_core_db_exec(db, drop_sql, NULL, NULL, &errmsg);
                }
                if (errmsg) {
                    base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_DEBUG, "SQL ERR [%s]\n[%s]\n", errmsg, reactive_sql);
                    base_core_db_free(errmsg);
                    errmsg = NULL;
                }
                base_core_db_exec(db, reactive_sql, NULL, NULL, &errmsg);
                if (errmsg) {
                    base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_DEBUG, "SQL ERR [%s]\n[%s]\n", errmsg, reactive_sql);
                    base_core_db_free(errmsg);
                    errmsg = NULL;
                }
            }
        }
    }

}


BASE_DECLARE(base_status_t) base_core_db_persistant_execute_trans(base_core_db_t *db, char *sql, uint32_t retries)
{
    char *errmsg;
    base_status_t status = BASE_STATUS_FALSE;
    uint8_t forever = 0;
    unsigned begin_retries = 100;
    uint8_t again = 0;

    if (!retries) {
        forever = 1;
        retries = 1000;
    }

again:

    while (begin_retries > 0) {
        again = 0;

        base_core_db_exec(db, "BEGIN", NULL, NULL, &errmsg);

        if (errmsg) {
            begin_retries--;
            if (strstr(errmsg, "cannot start a transaction within a transaction")) {
                again = 1;
            } else {
                base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_DEBUG, "SQL Retry [%s]\n", errmsg);
            }
            base_core_db_free(errmsg);
            errmsg = NULL;

            if (again) {
                base_core_db_exec(db, "COMMIT", NULL, NULL, NULL);
                goto again;
            }

            base_yield(100000);

            if (begin_retries == 0) {
                goto done;
            }
        } else {
            break;
        }

    }

    while (retries > 0) {
        base_core_db_exec(db, sql, NULL, NULL, &errmsg);
        if (errmsg) {
            base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_ERROR, "SQL ERR [%s]\n", errmsg);
            base_core_db_free(errmsg);
            errmsg = NULL;
            base_yield(100000);
            retries--;
            if (retries == 0 && forever) {
                retries = 1000;
                continue;
            }
        } else {
            status = BASE_STATUS_SUCCESS;
            break;
        }
    }

done:

    base_core_db_exec(db, "COMMIT", NULL, NULL, NULL);

    return status;
}

BASE_DECLARE(base_status_t) base_core_db_persistant_execute(base_core_db_t *db, char *sql, uint32_t retries)
{
    char *errmsg;
    base_status_t status = BASE_STATUS_FALSE;
    uint8_t forever = 0;

    if (!retries) {
        forever = 1;
        retries = 1000;
    }

    while (retries > 0) {
        base_core_db_exec(db, sql, NULL, NULL, &errmsg);
        if (errmsg) {
            //base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_ERROR, "SQL ERR [%s]\n", errmsg);
            base_core_db_free(errmsg);
            base_yield(100000);
            retries--;
            if (retries == 0 && forever) {
                retries = 1000;
                continue;
            }
        } else {
            status = BASE_STATUS_SUCCESS;
            break;
        }
    }

    return status;
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
