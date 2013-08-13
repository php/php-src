
	/* $Id: fpm_unix.c,v 1.25.2.1 2008/12/13 03:18:23 anight Exp $ */
	/* (c) 2007,2008 Andrei Nigmatulin */

#include "fpm_config.h"

#include <string.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>

#ifdef HAVE_PRCTL
#include <sys/prctl.h>
#endif

#include "fpm.h"
#include "fpm_conf.h"
#include "fpm_cleanup.h"
#include "fpm_clock.h"
#include "fpm_stdio.h"
#include "fpm_unix.h"
#include "fpm_signals.h"
#include "zlog.h"

size_t fpm_pagesize;

int fpm_unix_resolve_socket_premissions(struct fpm_worker_pool_s *wp) /* {{{ */
{
	struct fpm_worker_pool_config_s *c = wp->config;

	/* uninitialized */
	wp->socket_uid = -1;
	wp->socket_gid = -1;
	wp->socket_mode = 0666;

	if (!c) {
		return 0;
	}

	if (c->listen_owner && *c->listen_owner) {
		struct passwd *pwd;

		pwd = getpwnam(c->listen_owner);
		if (!pwd) {
			zlog(ZLOG_SYSERROR, "[pool %s] cannot get uid for user '%s'", wp->config->name, c->listen_owner);
			return -1;
		}

		wp->socket_uid = pwd->pw_uid;
		wp->socket_gid = pwd->pw_gid;
	}

	if (c->listen_group && *c->listen_group) {
		struct group *grp;

		grp = getgrnam(c->listen_group);
		if (!grp) {
			zlog(ZLOG_SYSERROR, "[pool %s] cannot get gid for group '%s'", wp->config->name, c->listen_group);
			return -1;
		}
		wp->socket_gid = grp->gr_gid;
	}

	if (c->listen_mode && *c->listen_mode) {
		wp->socket_mode = strtoul(c->listen_mode, 0, 8);
	}
	return 0;
}
/* }}} */

static int fpm_unix_conf_wp(struct fpm_worker_pool_s *wp) /* {{{ */
{
	struct passwd *pwd;
	int is_root = !geteuid();

	if (is_root) {
		if (wp->config->user && *wp->config->user) {
			if (strlen(wp->config->user) == strspn(wp->config->user, "0123456789")) {
				wp->set_uid = strtoul(wp->config->user, 0, 10);
			} else {
				struct passwd *pwd;

				pwd = getpwnam(wp->config->user);
				if (!pwd) {
					zlog(ZLOG_ERROR, "[pool %s] cannot get uid for user '%s'", wp->config->name, wp->config->user);
					return -1;
				}

				wp->set_uid = pwd->pw_uid;
				wp->set_gid = pwd->pw_gid;

				wp->user = strdup(pwd->pw_name);
				wp->home = strdup(pwd->pw_dir);
			}
		}

		if (wp->config->group && *wp->config->group) {
			if (strlen(wp->config->group) == strspn(wp->config->group, "0123456789")) {
				wp->set_gid = strtoul(wp->config->group, 0, 10);
			} else {
				struct group *grp;

				grp = getgrnam(wp->config->group);
				if (!grp) {
					zlog(ZLOG_ERROR, "[pool %s] cannot get gid for group '%s'", wp->config->name, wp->config->group);
					return -1;
				}
				wp->set_gid = grp->gr_gid;
			}
		}

		if (!fpm_globals.run_as_root) {
			if (wp->set_uid == 0 || wp->set_gid == 0) {
				zlog(ZLOG_ERROR, "[pool %s] please specify user and group other than root", wp->config->name);
				return -1;
			}
		}
	} else { /* not root */
		if (wp->config->user && *wp->config->user) {
			zlog(ZLOG_NOTICE, "[pool %s] 'user' directive is ignored when FPM is not running as root", wp->config->name);
		}
		if (wp->config->group && *wp->config->group) {
			zlog(ZLOG_NOTICE, "[pool %s] 'group' directive is ignored when FPM is not running as root", wp->config->name);
		}
		if (wp->config->chroot && *wp->config->chroot) {
			zlog(ZLOG_NOTICE, "[pool %s] 'chroot' directive is ignored when FPM is not running as root", wp->config->name);
		}
		if (wp->config->process_priority != 64) {
			zlog(ZLOG_NOTICE, "[pool %s] 'process.priority' directive is ignored when FPM is not running as root", wp->config->name);
		}

		/* set up HOME and USER anyway */
		pwd = getpwuid(getuid());
		if (pwd) {
			wp->user = strdup(pwd->pw_name);
			wp->home = strdup(pwd->pw_dir);
		}
	}
	return 0;
}
/* }}} */

int fpm_unix_init_child(struct fpm_worker_pool_s *wp) /* {{{ */
{
	int is_root = !geteuid();
	int made_chroot = 0;

	if (wp->config->rlimit_files) {
		struct rlimit r;

		r.rlim_max = r.rlim_cur = (rlim_t) wp->config->rlimit_files;

		if (0 > setrlimit(RLIMIT_NOFILE, &r)) {
			zlog(ZLOG_SYSERROR, "[pool %s] failed to set rlimit_files for this pool. Please check your system limits or decrease rlimit_files. setrlimit(RLIMIT_NOFILE, %d)", wp->config->name, wp->config->rlimit_files);
		}
	}

	if (wp->config->rlimit_core) {
		struct rlimit r;

		r.rlim_max = r.rlim_cur = wp->config->rlimit_core == -1 ? (rlim_t) RLIM_INFINITY : (rlim_t) wp->config->rlimit_core;

		if (0 > setrlimit(RLIMIT_CORE, &r)) {
			zlog(ZLOG_SYSERROR, "[pool %s] failed to set rlimit_core for this pool. Please check your system limits or decrease rlimit_core. setrlimit(RLIMIT_CORE, %d)", wp->config->name, wp->config->rlimit_core);
		}
	}

	if (is_root && wp->config->chroot && *wp->config->chroot) {
		if (0 > chroot(wp->config->chroot)) {
			zlog(ZLOG_SYSERROR, "[pool %s] failed to chroot(%s)",  wp->config->name, wp->config->chroot);
			return -1;
		}
		made_chroot = 1;
	}

	if (wp->config->chdir && *wp->config->chdir) {
		if (0 > chdir(wp->config->chdir)) {
			zlog(ZLOG_SYSERROR, "[pool %s] failed to chdir(%s)", wp->config->name, wp->config->chdir);
			return -1;
		}
	} else if (made_chroot) {
		chdir("/");
	}

	if (is_root) {

		if (wp->config->process_priority != 64) {
			if (setpriority(PRIO_PROCESS, 0, wp->config->process_priority) < 0) {
				zlog(ZLOG_SYSERROR, "[pool %s] Unable to set priority for this new process", wp->config->name);
				return -1;
			}
		}

		if (wp->set_gid) {
			if (0 > setgid(wp->set_gid)) {
				zlog(ZLOG_SYSERROR, "[pool %s] failed to setgid(%d)", wp->config->name, wp->set_gid);
				return -1;
			}
		}
		if (wp->set_uid) {
			if (0 > initgroups(wp->config->user, wp->set_gid)) {
				zlog(ZLOG_SYSERROR, "[pool %s] failed to initgroups(%s, %d)", wp->config->name, wp->config->user, wp->set_gid);
				return -1;
			}
			if (0 > setuid(wp->set_uid)) {
				zlog(ZLOG_SYSERROR, "[pool %s] failed to setuid(%d)", wp->config->name, wp->set_uid);
				return -1;
			}
		}
	}

#ifdef HAVE_PRCTL
	if (0 > prctl(PR_SET_DUMPABLE, 1, 0, 0, 0)) {
		zlog(ZLOG_SYSERROR, "[pool %s] failed to prctl(PR_SET_DUMPABLE)", wp->config->name);
	}
#endif

	if (0 > fpm_clock_init()) {
		return -1;
	}
	return 0;
}
/* }}} */

int fpm_unix_init_main() /* {{{ */
{
	struct fpm_worker_pool_s *wp;
	int is_root = !geteuid();

	if (fpm_global_config.rlimit_files) {
		struct rlimit r;

		r.rlim_max = r.rlim_cur = (rlim_t) fpm_global_config.rlimit_files;

		if (0 > setrlimit(RLIMIT_NOFILE, &r)) {
			zlog(ZLOG_SYSERROR, "failed to set rlimit_core for this pool. Please check your system limits or decrease rlimit_files. setrlimit(RLIMIT_NOFILE, %d)", fpm_global_config.rlimit_files);
			return -1;
		}
	}

	if (fpm_global_config.rlimit_core) {
		struct rlimit r;

		r.rlim_max = r.rlim_cur = fpm_global_config.rlimit_core == -1 ? (rlim_t) RLIM_INFINITY : (rlim_t) fpm_global_config.rlimit_core;

		if (0 > setrlimit(RLIMIT_CORE, &r)) {
			zlog(ZLOG_SYSERROR, "failed to set rlimit_core for this pool. Please check your system limits or decrease rlimit_core. setrlimit(RLIMIT_CORE, %d)", fpm_global_config.rlimit_core);
			return -1;
		}
	}

	fpm_pagesize = getpagesize();
	if (fpm_global_config.daemonize) {
		/*
		 * If daemonize, the calling process will die soon
		 * and the master process continues to initialize itself.
		 *
		 * The parent process has then to wait for the master
		 * process to initialize to return a consistent exit
		 * value. For this pupose, the master process will
		 * send \"1\" into the pipe if everything went well 
		 * and \"0\" otherwise.
		 */


		struct timeval tv;
		fd_set rfds;
		int ret;

		if (pipe(fpm_globals.send_config_pipe) == -1) {
			zlog(ZLOG_SYSERROR, "failed to create pipe");
			return -1;
		}

		/* then fork */
		pid_t pid = fork();
		switch (pid) {

			case -1 : /* error */
				zlog(ZLOG_SYSERROR, "failed to daemonize");
				return -1;

			case 0 : /* children */
				close(fpm_globals.send_config_pipe[0]); /* close the read side of the pipe */
				break;

			default : /* parent */
				close(fpm_globals.send_config_pipe[1]); /* close the write side of the pipe */

				/*
				 * wait for 10s before exiting with error
				 * the child is supposed to send 1 or 0 into the pipe to tell the parent
				 * how it goes for it
				 */
				FD_ZERO(&rfds);
				FD_SET(fpm_globals.send_config_pipe[0], &rfds);

				tv.tv_sec = 10;
				tv.tv_usec = 0;

				zlog(ZLOG_DEBUG, "The calling process is waiting for the master process to ping via fd=%d", fpm_globals.send_config_pipe[0]);
				ret = select(fpm_globals.send_config_pipe[0] + 1, &rfds, NULL, NULL, &tv);
				if (ret == -1) {
					zlog(ZLOG_SYSERROR, "failed to select");
					exit(FPM_EXIT_SOFTWARE);
				}
				if (ret) { /* data available */
					int readval;
					ret = read(fpm_globals.send_config_pipe[0], &readval, sizeof(readval));
					if (ret == -1) {
						zlog(ZLOG_SYSERROR, "failed to read from pipe");
						exit(FPM_EXIT_SOFTWARE);
					}

					if (ret == 0) {
						zlog(ZLOG_ERROR, "no data have been read from pipe");
						exit(FPM_EXIT_SOFTWARE);
					} else {
						if (readval == 1) {
							zlog(ZLOG_DEBUG, "I received a valid acknoledge from the master process, I can exit without error");
							fpm_cleanups_run(FPM_CLEANUP_PARENT_EXIT);
							exit(FPM_EXIT_OK);
						} else {
							zlog(ZLOG_DEBUG, "The master process returned an error !");
							exit(FPM_EXIT_SOFTWARE);
						}
					}
				} else { /* no date sent ! */
					zlog(ZLOG_ERROR, "the master process didn't send back its status (via the pipe to the calling process)");
				  exit(FPM_EXIT_SOFTWARE);
				}
				exit(FPM_EXIT_SOFTWARE);
		}
	}

	/* continue as a child */
	setsid();
	if (0 > fpm_clock_init()) {
		return -1;
	}

	if (fpm_global_config.process_priority != 64) {
		if (is_root) {
			if (setpriority(PRIO_PROCESS, 0, fpm_global_config.process_priority) < 0) {
				zlog(ZLOG_SYSERROR, "Unable to set priority for the master process");
				return -1;
			}
		} else {
			zlog(ZLOG_NOTICE, "'process.priority' directive is ignored when FPM is not running as root");
		}
	}

	fpm_globals.parent_pid = getpid();
	for (wp = fpm_worker_all_pools; wp; wp = wp->next) {
		if (0 > fpm_unix_conf_wp(wp)) {
			return -1;
		}
	}

	zlog_set_level(fpm_globals.log_level);
	return 0;
}
/* }}} */

