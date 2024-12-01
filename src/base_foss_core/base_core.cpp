#include <base.h>

BASE_DECLARE_DATA base_directories BASE_GLOBAL_dirs = { 0 };
BASE_DECLARE_DATA base_filenames BASE_GLOBAL_filenames = { 0 };

BASE_DECLARE(int32_t) base_core_session_ctl(base_session_ctl_t cmd, void *val)
{
	int *intval = (int *) val;
	int oldintval = 0, newintval = 0;

	if (intval) {
		oldintval = *intval;
	}

	if (switch_test_flag((&runtime), SCF_SHUTTING_DOWN)) {
		return -1;
	}

	switch (cmd) {
	case SCSC_RECOVER:
		{
			char *arg = (char *) val;
			char *tech = NULL, *prof = NULL;
			int r, flush = 0;

			if (!zstr(arg)) {
				tech = strdup(arg);
				switch_assert(tech);

				if ((prof = strchr(tech, ':'))) {
					*prof++ = '\0';
				}

				if (!strcasecmp(tech, "flush")) {
					flush++;

					if (prof) {
						char *tech = prof;
						if ((prof = strchr(tech, ':'))) {
							*prof++ = '\0';
						}
					}
				}

			}

			if (flush) {
				switch_core_recovery_flush(tech, prof);
				r = -1;
			} else {
				r = switch_core_recovery_recover(tech, prof);
			}

			switch_safe_free(tech);
			return r;

		}
		break;
	case SCSC_DEBUG_SQL:
		{
			if (switch_test_flag((&runtime), SCF_DEBUG_SQL)) {
				switch_clear_flag((&runtime), SCF_DEBUG_SQL);
				newintval = 0;
			} else {
				switch_set_flag((&runtime), SCF_DEBUG_SQL);
				newintval = 1;
			}
		}
		break;
	case SCSC_VERBOSE_EVENTS:
		if (intval) {
			if (oldintval > -1) {
				if (oldintval) {
					switch_set_flag((&runtime), SCF_VERBOSE_EVENTS);
				} else {
					switch_clear_flag((&runtime), SCF_VERBOSE_EVENTS);
				}
			}
			newintval = switch_test_flag((&runtime), SCF_VERBOSE_EVENTS);
		}
		break;
	case SCSC_API_EXPANSION:
		if (intval) {
			if (oldintval > -1) {
				if (oldintval) {
					switch_set_flag((&runtime), SCF_API_EXPANSION);
				} else {
					switch_clear_flag((&runtime), SCF_API_EXPANSION);
				}
			}
			newintval = switch_test_flag((&runtime), SCF_API_EXPANSION);
		}
		break;
	case SCSC_THREADED_SYSTEM_EXEC:
		if (intval) {
			if (oldintval > -1) {
				if (oldintval) {
					switch_set_flag((&runtime), SCF_THREADED_SYSTEM_EXEC);
				} else {
					switch_clear_flag((&runtime), SCF_THREADED_SYSTEM_EXEC);
				}
			}
			newintval = switch_test_flag((&runtime), SCF_THREADED_SYSTEM_EXEC);
		}
		break;
	case SCSC_CALIBRATE_CLOCK:
		switch_time_calibrate_clock();
		break;
	case SCSC_FLUSH_DB_HANDLES:
		switch_cache_db_flush_handles();
		break;
	case SCSC_SEND_SIGHUP:
		handle_SIGHUP(1);
		break;
	case SCSC_SYNC_CLOCK:
		switch_time_sync();
		newintval = 0;
		break;
	case SCSC_SYNC_CLOCK_WHEN_IDLE:
		newintval = switch_core_session_sync_clock();
		break;
	case SCSC_SQL:
		if (oldintval) {
			switch_core_sqldb_resume();
		} else {
			switch_core_sqldb_pause();
		}
		break;
	case SCSC_PAUSE_ALL:
		if (oldintval) {
			switch_set_flag((&runtime), SCF_NO_NEW_SESSIONS);
		} else {
			switch_clear_flag((&runtime), SCF_NO_NEW_SESSIONS);
		}
		break;
	case SCSC_PAUSE_INBOUND:
		if (oldintval) {
			switch_set_flag((&runtime), SCF_NO_NEW_INBOUND_SESSIONS);
		} else {
			switch_clear_flag((&runtime), SCF_NO_NEW_INBOUND_SESSIONS);
		}
		break;
	case SCSC_PAUSE_OUTBOUND:
		if (oldintval) {
			switch_set_flag((&runtime), SCF_NO_NEW_OUTBOUND_SESSIONS);
		} else {
			switch_clear_flag((&runtime), SCF_NO_NEW_OUTBOUND_SESSIONS);
		}
		break;
	case SCSC_HUPALL:
		switch_core_session_hupall(SWITCH_CAUSE_MANAGER_REQUEST);
		break;
	case SCSC_CANCEL_SHUTDOWN:
		switch_clear_flag((&runtime), SCF_SHUTDOWN_REQUESTED);
		break;
	case SCSC_SAVE_HISTORY:
		switch_console_save_history();
		break;
	case SCSC_CRASH:
		switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_CRIT, "Declinatio Mortuus Obfirmo!\n");
		switch_console_save_history();
		abort();
		break;
	case SCSC_SHUTDOWN_NOW:
		switch_console_save_history();
		exit(0);
		break;
	case SCSC_REINCARNATE_NOW:
		switch_console_save_history();
		exit(SWITCH_STATUS_RESTART);
		break;
	case SCSC_SHUTDOWN_ELEGANT:
	case SCSC_SHUTDOWN_ASAP:
		{
			int x = 19;
			uint32_t count;
			switch_event_t *shutdown_requested_event = NULL;
			if (switch_event_create(&shutdown_requested_event, SWITCH_EVENT_SHUTDOWN_REQUESTED) == SWITCH_STATUS_SUCCESS) {
				switch_event_add_header(shutdown_requested_event, SWITCH_STACK_BOTTOM, "Event-Info", "%s", cmd == SCSC_SHUTDOWN_ASAP ? "ASAP" : "elegant");
				switch_event_fire(&shutdown_requested_event);
			}
			switch_set_flag((&runtime), SCF_SHUTDOWN_REQUESTED);
			if (cmd == SCSC_SHUTDOWN_ASAP) {
				switch_set_flag((&runtime), SCF_NO_NEW_SESSIONS);
			}

			while (runtime.running && switch_test_flag((&runtime), SCF_SHUTDOWN_REQUESTED) && (count = switch_core_session_count())) {
				switch_yield(500000);
				if (++x == 20) {
					switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_WARNING,
									  "Shutdown in progress, %u session(s) remain.\nShutting down %s\n",
									  count, cmd == SCSC_SHUTDOWN_ASAP ? "ASAP" : "once there are no active calls.");
					x = 0;
				}
			}

			if (switch_test_flag((&runtime), SCF_SHUTDOWN_REQUESTED)) {
				switch_set_flag((&runtime), SCF_NO_NEW_SESSIONS);
#ifdef _MSC_VER
				win_shutdown();
#endif

				if (oldintval) {
					switch_set_flag((&runtime), SCF_RESTART);
					switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "Restarting\n");
				} else {
					switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "Shutting down\n");
#ifdef HAVE_SYSTEMD
					sd_notifyf(0, "STOPPING=1\n");
#endif
#ifdef _MSC_VER
					fclose(stdin);
#endif
				}
				runtime.running = 0;
			} else {
				switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "Shutdown Cancelled\n");
				switch_clear_flag((&runtime), SCF_NO_NEW_SESSIONS);
			}
		}
		break;
	case SCSC_PAUSE_CHECK:
		newintval = !!(switch_test_flag((&runtime), SCF_NO_NEW_SESSIONS) == SCF_NO_NEW_SESSIONS);
		break;
	case SCSC_PAUSE_INBOUND_CHECK:
		newintval = !!switch_test_flag((&runtime), SCF_NO_NEW_INBOUND_SESSIONS);
		break;
	case SCSC_PAUSE_OUTBOUND_CHECK:
		newintval = !!switch_test_flag((&runtime), SCF_NO_NEW_OUTBOUND_SESSIONS);
		break;
	case SCSC_READY_CHECK:
		newintval = switch_core_ready();
		break;
	case SCSC_SHUTDOWN_CHECK:
		newintval = !!switch_test_flag((&runtime), SCF_SHUTDOWN_REQUESTED);
		break;
	case SCSC_SHUTDOWN:

#ifdef _MSC_VER
		win_shutdown();
#endif

		if (oldintval) {
			switch_set_flag((&runtime), SCF_RESTART);
			switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "Restarting\n");
		} else {
			switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "Shutting down\n");
#ifdef _MSC_VER
			fclose(stdin);
#endif
		}
		runtime.running = 0;
		break;
	case SCSC_CHECK_RUNNING:
		newintval = runtime.running;
		break;
	case SCSC_LOGLEVEL:
		if (oldintval >= SWITCH_LOG_DISABLE) {
			runtime.hard_log_level = oldintval;
		}

		if (runtime.hard_log_level > SWITCH_LOG_DEBUG) {
			runtime.hard_log_level = SWITCH_LOG_DEBUG;
		}
		newintval = runtime.hard_log_level;
		break;
	case SCSC_DEBUG_LEVEL:
		if (oldintval > -1) {
			if (oldintval > 10)
				oldintval = 10;
			runtime.debug_level = oldintval;
		}
		newintval = runtime.debug_level;
		break;
	case SCSC_MIN_IDLE_CPU:
		{
			double *dval = (double *) val;
			if (dval) {
				*dval = switch_core_min_idle_cpu(*dval);
			}
			intval = NULL;
		}
		break;
	case SCSC_MAX_SESSIONS:
		newintval = switch_core_session_limit(oldintval);
		break;
	case SCSC_LAST_SPS:
		newintval = runtime.sps_last;
		break;
	case SCSC_SPS_PEAK:
		if (oldintval == -1) {
			runtime.sps_peak = 0;
		}
		newintval = runtime.sps_peak;
		break;
	case SCSC_SPS_PEAK_FIVEMIN:
		newintval = runtime.sps_peak_fivemin;
		break;
	case SCSC_SESSIONS_PEAK:
		newintval = runtime.sessions_peak;
		break;
	case SCSC_SESSIONS_PEAK_FIVEMIN:
		newintval = runtime.sessions_peak_fivemin;
		break;
	case SCSC_MAX_DTMF_DURATION:
		newintval = switch_core_max_dtmf_duration(oldintval);
		break;
	case SCSC_MIN_DTMF_DURATION:
		newintval = switch_core_min_dtmf_duration(oldintval);
		break;
	case SCSC_DEFAULT_DTMF_DURATION:
		newintval = switch_core_default_dtmf_duration(oldintval);
		break;
	case SCSC_SPS:
		switch_mutex_lock(runtime.throttle_mutex);
		if (oldintval > 0) {
			runtime.sps_total = oldintval;
		}
		newintval = runtime.sps_total;
		switch_mutex_unlock(runtime.throttle_mutex);
		break;

	case SCSC_RECLAIM:
		switch_core_memory_reclaim_all();
		newintval = 0;
		break;
	case SCSC_MDNS_RESOLVE:
		switch_core_media_set_resolveice(!!oldintval);
		break;
	case SCSC_SHUTDOWN_CAUSE:
		runtime.shutdown_cause = oldintval;
		break;
	}

	if (intval) {
		*intval = newintval;
	}


	return 0;
}