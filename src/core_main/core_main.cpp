#include <iostream>
#include <cstring>
#include <csignal>
#include <memory>
#include <vector>
#include <string>
#include <fcntl.h>
#include <errno.h>

#ifdef __linux__
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <poll.h>
#include <sys/wait.h>
#endif

#include <base.h>

#define PIDFILE "core_main.pid"
static char *pfile = PIDFILE;
static int system_ready = 0;

#ifndef PATH_MAX
#define PATH_MAX 256
#endif

static void handle_SIGILL(int sig) {
    int32_t arg = 0;
    if (sig) {};
    switchh_shutdown_delay_force_exit(10);
    return;
}

static const std::string usage =
    "Usage: freeswitch [OPTIONS]\n\n"
    "These are the optional arguments you can pass to freeswitch:\n"
#ifdef WIN32
    "\t-service [name]        -- start freeswitch as a service, cannot be used if loaded as a console app\n"
    "\t-install [name]        -- install freeswitch as a service, with optional service name\n"
    "\t-uninstall             -- remove freeswitch as a service\n"
    "\t-monotonic-clock       -- use monotonic clock as timer source\n"
#else
    "\t-nf                    -- no forking\n"
    "\t-reincarnate           -- restart the switch on an uncontrolled exit\n"
    "\t-reincarnate-reexec    -- run execv on a restart (helpful for upgrades)\n"
    "\t-u [user]              -- specify user to switch to\n"
    "\t-g [group]             -- specify group to switch to\n"
#endif
#ifdef HAVE_SETRLIMIT
#ifndef FS_64BIT
    "\t-waste                 -- allow memory waste\n"
#endif
    "\t-core                  -- dump cores\n"
#endif
    "\t-help                  -- this message\n"
    "\t-version               -- print the version and exit\n"
    "\t-rp                    -- enable high(realtime) priority settings\n"
    "\t-lp                    -- enable low priority settings\n"
    "\t-np                    -- enable normal priority settings\n"
    "\t-vg                    -- run under valgrind\n"
    "\t-nosql                 -- disable internal sql scoreboard\n"
    "\t-heavy-timer           -- Heavy Timer, possibly more accurate but at a cost\n"
    "\t-nonat                 -- disable auto nat detection\n"
    "\t-nonatmap              -- disable auto nat port mapping\n"
    "\t-nocal                 -- disable clock calibration\n"
    "\t-nort                  -- disable clock clock_realtime\n"
    "\t-stop                  -- stop freeswitch\n"
    "\t-nc                    -- do not output to a console and background\n"
#ifndef WIN32
    "\t-ncwait                -- do not output to a console and background but wait until the system is ready before exiting (implies -nc)\n"
#endif
    "\t-c                     -- output to a console and stay in the foreground\n"
    "\n\tOptions to control locations of files:\n"
    "\t-base [basedir]         -- alternate prefix directory\n"
    "\t-cfgname [filename]     -- alternate filename for FreeSWITCH main configuration file\n"
    "\t-conf [confdir]         -- alternate directory for FreeSWITCH configuration files\n"
    "\t-log [logdir]           -- alternate directory for logfiles\n"
    "\t-run [rundir]           -- alternate directory for runtime files\n"
    "\t-db [dbdir]             -- alternate directory for the internal database\n"
    "\t-mod [moddir]           -- alternate directory for modules\n"
    "\t-htdocs [htdocsdir]     -- alternate directory for htdocs\n"
    "\t-scripts [scriptsdir]   -- alternate directory for scripts\n"
    "\t-temp [directory]       -- alternate directory for temporary files\n"
    "\t-grammar [directory]    -- alternate directory for grammar files\n"
    "\t-certs [directory]      -- alternate directory for certificates\n"
    "\t-recordings [directory] -- alternate directory for recordings\n"
    "\t-storage [directory]    -- alternate directory for voicemail storage\n"
    "\t-cache [directory]      -- alternate directory for cache files\n"
    "\t-sounds [directory]     -- alternate directory for sound files\n";

static bool is_option(const char *p) {
    // 函数会跳过字符串开头的空白字符，包括回车 (\r)、换行 (\n)、制表符 (\t)、空格 ( ) 以及垂直制表符 (\v)。
    while (*p == '\r' || *p == '\n' || *p == '\t' || *p == ' ' || *p == '\v') {
        p++;
    }
    return p[0] == '-';
}

int main(int argc, char *argv[]) {
    char pid_path[PATH_MAX] = "";	/* full path to the pid file */
    char pid_buffer[32] = "";	/* pid string */
    char old_pid_buffer[32] = "";	/* pid string */
    base_size_t pid_len, old_pid_len;

#ifndef WIN32
    base_bool_t nf = BASE_FALSE;  // TRUE if we are running in nofork mode
    base_bool_t do_wait = BASE_FALSE;
    int fds[2] = { 0, 0 };
#else
#endif
    bool nc = false;
    bool multi_program = false;
    pid_t pid = 0;
    int x;
    std::vector<char*> local_argv(argc);
    int local_argc = argc;
#if (defined(__SVR4) && defined(__sun))
	switch_core_flag_t flags = SCF_USE_SQL | SCF_CALIBRATE_CLOCK | SCF_USE_CLOCK_RT;
#else
	base_core_flag_t flags = SCF_USE_SQL | SCF_USE_AUTO_NAT | SCF_USE_NAT_MAPPING | SCF_CALIBRATE_CLOCK | SCF_USE_CLOCK_RT;
#endif
    base_status_t destroy_status;
    base_file_t* fd;
    base_memory_pool_t* pool = nullptr;

    for (x = 0; x < argc; x++) {
        local_argv[x] = argv[x];
    }

    for (x = 1; x < local_argc; x++) {
        if (base_strlen_zero(local_argv[x])) {
            continue;
        }
        if (!strcmp(local_argv[x], "-help") || !strcmp(local_argv[x], "-h") || !strcmp(local_argv[x], "-?")) {
            std::cout << usage << std::endl;
            exit(EXIT_SUCCESS);
        }

        if (!strcmp(local_argv[x], "-nc")) {
            nc = true;
        }
#ifndef WIN32
        else if (!strcmp(local_argv[x], "-ncwait")) {
            nc = true;
            do_wait = true;
        } else if (!strcmp(local_argv[x], "-nf")) {
            nf = false;
        }
#endif
        else if (!strcmp(local_argv[x], "-c")) {
            nc = true;
        } else if (!strcmp(local_argv[x], "-nomulti")) {
            multi_program = false;
        } else if (!strcmp(local_argv[x], "-conf")) {
            x++;
            if (base_strlen_zero(local_argv[x]) || is_option(local_argv[x])) {
                std::cerr << "When using -conf you must specify a config directory" << std::endl;
                return 255;
            }

            BASE_GLOBAL_dirs.conf_dir = strdup(local_argv[x]);
            if (!BASE_GLOBAL_dirs.conf_dir) {
                std::cerr << "Allocation error" << std::endl;
                return 255;
            }
        } else if (!strcmp(local_argv[x], "-log")) {
            x++;
            if (base_strlen_zero(local_argv[x]) || is_option(local_argv[x])) {
                std::cerr << "When using -log you must specify a log directory" << std::endl;
                return 255;
            }

            BASE_GLOBAL_dirs.log_dir = strdup(local_argv[x]);
            if (!BASE_GLOBAL_dirs.log_dir) {
                std::cerr << "Allocation error" << std::endl;
                return 255;
            }
        } else if (!strcmp(local_argv[x], "-cert")) {
            x++;
            if (base_strlen_zero(local_argv[x]) || is_option(local_argv[x])) {
                std::cerr << "When using -cert you must specify a cert directory" << std::endl;
                return 255;
            }

            BASE_GLOBAL_dirs.certs_dir = strdup(local_argv[x]);
            if (!BASE_GLOBAL_dirs.certs_dir) {
                std::cerr << "Allocation error" << std::endl;
                return 255;
            }
        } else if (!strcmp(local_argv[x], "-db")) {
            x++;
            if (base_strlen_zero(local_argv[x]) || is_option(local_argv[x])) {
                std::cerr << "When using -db you must specify a db directory" << std::endl;
                return 255;
            }

            BASE_GLOBAL_dirs.db_dir = strdup(local_argv[x]);
            if (!BASE_GLOBAL_dirs.db_dir) {
                std::cerr << "Allocation error" << std::endl;
                return 255;
            }
        } else if (!strcmp(local_argv[x], "-run")) {
            x++;
            if (base_strlen_zero(local_argv[x]) || is_option(local_argv[x])) {
                std::cerr << "When using -run you must specify a pid directory" << std::endl;
                return 255;
            }

            BASE_GLOBAL_dirs.run_dir = strdup(local_argv[x]);
            if (!BASE_GLOBAL_dirs.run_dir) {
                std::cerr << "Allocation error" << std::endl;
                return 255;
            }
        } else if (!strcmp(local_argv[x], "-pid")) {
            x++;
            if (base_strlen_zero(local_argv[x]) || is_option(local_argv[x])) {
                std::cerr << "When using -run you must specify a pid directory" << std::endl;
                return 255;
            }
            strncpy(pid_path, local_argv[x], sizeof(pid_path)-1);
        } else if (!strcmp(local_argv[x], "-support_chinese")) {
            putenv(strdup("SUPPORT_CHINESE=true"));
        } else {
            std::cerr << "Unknown option '" << local_argv[x] << "', see '" << local_argv[0] << " -help' for a list of valid options" << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    if (base_apr_initialize() != BASE_STATUS_SUCCESS) {
        std::cerr << "FATAL ERROR! Could not initialize APR" << std::endl;
        return 255;
    }

    signal(SIGILL, handle_SIGILL);
    signal(SIGTERM, handle_SIGILL);

    return 0;
}