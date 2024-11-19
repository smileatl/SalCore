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
*
*
* base_scheduler.c -- Switch Scheduler
*
*/

#include <base.h>

struct base_scheduler_task_container {
    base_scheduler_task_t task;
    int64_t executed;
    int in_thread;
    int destroyed;
    int running;
    int destroy_requested;
    base_scheduler_func_t func;
    base_memory_pool_t *pool;
    uint32_t flags;
    char *desc;
    struct base_scheduler_task_container *next;
};
typedef struct base_scheduler_task_container base_scheduler_task_container_t;

static struct {
    base_scheduler_task_container_t *task_list;
    base_mutex_t *task_mutex;
    uint32_t task_id;
    int task_thread_running;
    base_queue_t *event_queue;
    base_memory_pool_t *memory_pool;
} globals = { 0 };

static void base_scheduler_execute(base_scheduler_task_container_t *tp)
{
    base_event_t *event;
    //base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_DEBUG, "Executing task %u %s (%s)\n", tp->task.task_id, tp->desc, base_str_nil(tp->task.group));

    tp->func(&tp->task);

    base_mutex_lock(globals.task_mutex);
    if (tp->task.repeat) {
        tp->task.runtime = base_epoch_time_now(NULL) + tp->task.repeat;
    }

    if (!tp->destroy_requested && tp->task.runtime > tp->executed) {
        tp->executed = 0;
        if (base_event_create(&event, BASE_EVENT_RE_SCHEDULE) == BASE_STATUS_SUCCESS) {
            base_event_add_header(event, BASE_STACK_BOTTOM, "Task-ID", "%u", tp->task.task_id);
            base_event_add_header_string(event, BASE_STACK_BOTTOM, "Task-Desc", tp->desc);
            base_event_add_header_string(event, BASE_STACK_BOTTOM, "Task-Group", base_str_nil(tp->task.group));
            base_event_add_header(event, BASE_STACK_BOTTOM, "Task-Runtime", "%" BASE_INT64_T_FMT, tp->task.runtime);
            base_queue_push(globals.event_queue, event);
            event = NULL;
        }
    } else {
        tp->destroyed = 1;
    }
    base_mutex_unlock(globals.task_mutex);
}

static void *BASE_THREAD_FUNC task_own_thread(base_thread_t *thread, void *obj)
{
    base_scheduler_task_container_t *tp = (base_scheduler_task_container_t *) obj;
    base_memory_pool_t *pool;

    pool = tp->pool;
    tp->pool = NULL;

    base_scheduler_execute(tp);
    base_core_destroy_memory_pool(&pool);
    tp->in_thread = 0;

    return NULL;
}

static int task_thread_loop(int done)
{
    base_scheduler_task_container_t *tofree, *tp, *last = NULL;


    base_mutex_lock(globals.task_mutex);

    for (tp = globals.task_list; tp; tp = tp->next) {
        if (done) {
            tp->destroyed = 1;
        } else if (!tp->destroyed) {
            int64_t now = base_epoch_time_now(NULL);
            if (now >= tp->task.runtime && !tp->in_thread) {
                int32_t diff = (int32_t) (now - tp->task.runtime);
                if (diff > 1) {
                    base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_WARNING, "Task was executed late by %d seconds %u %s (%s)\n",
                        diff, tp->task.task_id, tp->desc, base_str_nil(tp->task.group));
                }
                tp->executed = now;
                if (base_test_flag(tp, SSHF_OWN_THREAD)) {
                    base_thread_t *thread;
                    base_threadattr_t *thd_attr;
                    base_core_new_memory_pool(&tp->pool);
                    base_threadattr_create(&thd_attr, tp->pool);
                    base_threadattr_detach_set(thd_attr, 1);
                    tp->in_thread = 1;
                    base_thread_create(&thread, thd_attr, task_own_thread, tp, tp->pool);
                } else {
                    tp->running = 1;
                    base_mutex_unlock(globals.task_mutex);
                    base_scheduler_execute(tp);
                    base_mutex_lock(globals.task_mutex);
                    tp->running = 0;
                }
            }
        }
    }
    base_mutex_unlock(globals.task_mutex);
    base_mutex_lock(globals.task_mutex);
    for (tp = globals.task_list; tp;) {
        if (tp->destroyed && !tp->in_thread) {
            base_event_t *event;

            tofree = tp;
            tp = tp->next;
            base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_DEBUG, "Deleting task %u %s (%s)\n",
                tofree->task.task_id, tofree->desc, base_str_nil(tofree->task.group));


            if (base_event_create(&event, BASE_EVENT_DEL_SCHEDULE) == BASE_STATUS_SUCCESS) {
                base_event_add_header(event, BASE_STACK_BOTTOM, "Task-ID", "%u", tofree->task.task_id);
                base_event_add_header_string(event, BASE_STACK_BOTTOM, "Task-Desc", tofree->desc);
                base_event_add_header_string(event, BASE_STACK_BOTTOM, "Task-Group", base_str_nil(tofree->task.group));
                base_event_add_header(event, BASE_STACK_BOTTOM, "Task-Runtime", "%" BASE_INT64_T_FMT, tofree->task.runtime);
                base_queue_push(globals.event_queue, event);
                event = NULL;
            }

            if (last) {
                last->next = tofree->next;
            } else {
                globals.task_list = tofree->next;
            }
            base_safe_free(tofree->task.group);
            if (tofree->task.cmd_arg && base_test_flag(tofree, SSHF_FREE_ARG)) {
                free(tofree->task.cmd_arg);
            }
            base_safe_free(tofree->desc);
            free(tofree);
        } else {
            last = tp;
            tp = tp->next;
        }
    }
    base_mutex_unlock(globals.task_mutex);

    return done;
}

static void *BASE_THREAD_FUNC base_scheduler_task_thread(base_thread_t *thread, void *obj)
{
    void *pop;
    globals.task_thread_running = 1;

    base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_NOTICE, "Starting task thread\n");
    while (globals.task_thread_running == 1) {
        if (task_thread_loop(0)) {
            break;
        }
        if (base_queue_pop_timeout(globals.event_queue, &pop, 500000) == BASE_STATUS_SUCCESS) {
            base_event_t *event = (base_event_t *) pop;
            base_event_fire(&event);
        }
    }

    task_thread_loop(1);

    base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_NOTICE, "Task thread ending\n");

    while(base_queue_trypop(globals.event_queue, &pop) == BASE_STATUS_SUCCESS) {
        base_event_t *event = (base_event_t *) pop;
        base_event_destroy(&event);
    }

    globals.task_thread_running = 0;

    return NULL;
}

BASE_DECLARE(uint32_t) base_scheduler_add_task(time_t task_runtime,
    base_scheduler_func_t func,
    const char *desc, const char *group, uint32_t cmd_id, void *cmd_arg, base_scheduler_flag_t flags)
{
    uint32_t task_id;

    base_scheduler_add_task_ex(task_runtime, func, desc, group, cmd_id, cmd_arg, flags, &task_id);

    return task_id;
}

BASE_DECLARE(uint32_t) base_scheduler_add_task_ex(time_t task_runtime,
    base_scheduler_func_t func,
    const char *desc, const char *group, uint32_t cmd_id, void *cmd_arg, base_scheduler_flag_t flags, uint32_t *task_id)
{
    uint32_t result;
    base_scheduler_task_container_t *container, *tp;
    base_event_t *event;
    base_time_t now = base_epoch_time_now(NULL);
    base_ssize_t hlen = -1;

    base_mutex_lock(globals.task_mutex);
    base_zmalloc(container, sizeof(*container));
    base_assert(func);
    base_assert(task_id);

    if (task_runtime < now) {
        container->task.repeat = (uint32_t)task_runtime;
        task_runtime += now;
    }

    container->func = func;
    container->task.created = now;
    container->task.runtime = task_runtime;
    container->task.group = strdup(group ? group : "none");
    container->task.cmd_id = cmd_id;
    container->task.cmd_arg = cmd_arg;
    container->flags = flags;
    container->desc = strdup(desc ? desc : "none");
    container->task.hash = base_ci_hashfunc_default(container->task.group, &hlen);

    for (tp = globals.task_list; tp && tp->next; tp = tp->next);

    if (tp) {
        tp->next = container;
    } else {
        globals.task_list = container;
    }

    for (container->task.task_id = 0; !container->task.task_id; container->task.task_id = ++globals.task_id);

    tp = container;
    base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_DEBUG, "Added task %u %s (%s) to run at %" BASE_INT64_T_FMT "\n",
        tp->task.task_id, tp->desc, base_str_nil(tp->task.group), tp->task.runtime);

    if (base_event_create(&event, BASE_EVENT_ADD_SCHEDULE) == BASE_STATUS_SUCCESS) {
        base_event_add_header(event, BASE_STACK_BOTTOM, "Task-ID", "%u", tp->task.task_id);
        base_event_add_header_string(event, BASE_STACK_BOTTOM, "Task-Desc", tp->desc);
        base_event_add_header_string(event, BASE_STACK_BOTTOM, "Task-Group", base_str_nil(tp->task.group));
        base_event_add_header(event, BASE_STACK_BOTTOM, "Task-Runtime", "%" BASE_INT64_T_FMT, tp->task.runtime);
        base_queue_push(globals.event_queue, event);
        event = NULL;
    }

    result = *task_id = container->task.task_id;

    base_mutex_unlock(globals.task_mutex);

    return result;
}

BASE_DECLARE(uint32_t) base_scheduler_del_task_id(uint32_t task_id)
{
    base_scheduler_task_container_t *tp;
    uint32_t delcnt = 0;

    base_mutex_lock(globals.task_mutex);
    for (tp = globals.task_list; tp; tp = tp->next) {
        if (tp->task.task_id == task_id) {
            if (base_test_flag(tp, SSHF_NO_DEL)) {
                base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_WARNING, "Attempt made to delete undeletable task #%u (group %s)\n",
                    tp->task.task_id, tp->task.group);
                break;
            }

            if (tp->running) {
                base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_INFO, "Attempt made to delete running task #%u (group %s)\n",
                    tp->task.task_id, tp->task.group);
                tp->destroy_requested++;
            } else {
                tp->destroyed++;
            }

            delcnt++;
            break;
        }
    }
    base_mutex_unlock(globals.task_mutex);

    return delcnt;
}

BASE_DECLARE(uint32_t) base_scheduler_del_task_group(const char *group)
{
    base_scheduler_task_container_t *tp;
    uint32_t delcnt = 0;
    base_ssize_t hlen = -1;
    unsigned long hash = 0;

    if (zstr(group)) {
        return 0;
    }

    hash = base_ci_hashfunc_default(group, &hlen);

    base_mutex_lock(globals.task_mutex);
    for (tp = globals.task_list; tp; tp = tp->next) {
        if (tp->destroyed) {
            continue;
        }
        if (hash == tp->task.hash && !strcmp(tp->task.group, group)) {
            if (base_test_flag(tp, SSHF_NO_DEL)) {
                base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_WARNING, "Attempt made to delete undeletable task #%u (group %s)\n",
                    tp->task.task_id, group);
                continue;
            }
            if (tp->running) {
                base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_INFO, "Attempt made to delete running task #%u (group %s)\n",
                    tp->task.task_id, tp->task.group);
                tp->destroy_requested++;
            } else {
                tp->destroyed++;
            }
            delcnt++;
        }
    }
    base_mutex_unlock(globals.task_mutex);

    return delcnt;
}

base_thread_t *task_thread_p = NULL;

BASE_DECLARE(void) base_scheduler_task_thread_start(void)
{

    base_threadattr_t *thd_attr;

    base_core_new_memory_pool(&globals.memory_pool);
    base_threadattr_create(&thd_attr, globals.memory_pool);
    base_mutex_init(&globals.task_mutex, BASE_MUTEX_NESTED, globals.memory_pool);
    base_queue_create(&globals.event_queue, 250000, globals.memory_pool);

    base_thread_create(&task_thread_p, thd_attr, base_scheduler_task_thread, NULL, globals.memory_pool);
}

BASE_DECLARE(void) base_scheduler_task_thread_stop(void)
{
    base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_CONSOLE, "Stopping Task Thread\n");
    if (globals.task_thread_running == 1) {
        int sanity = 0;
        base_status_t st;

        globals.task_thread_running = -1;

        base_thread_join(&st, task_thread_p);

        while (globals.task_thread_running) {
            base_yield(100000);
            if (++sanity > 10) {
                break;
            }
        }
    }

    base_core_destroy_memory_pool(&globals.memory_pool);

}
