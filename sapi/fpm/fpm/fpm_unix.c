
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
			zlog(ZLOG_STUFF, ZLOG_SYSERROR, "[pool %s] cannot get uid for user '%s'", wp->config->name, c->listen_owner);
			return -1;
		}

		wp->socket_uid = pwd->pw_uid;
		wp->socket_gid = pwd->pw_gid;
	}

	if (c->listen_group && *c->listen_group) {
		struct group *grp;

		grp = getgrnam(c->listen_group);
		if (!grp) {
			zlog(ZLOG_STUFF, ZLOG_SYSERROR, "[pool %s] cannot get gid for group '%s'", wp->config->name, c->listen_group);
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
	int is_root = !geteuid();

	if (is_root) {
		if (wp->config->user && *wp->config->user) {
			if (strlen(wp->config->user) == strspn(wp->config->user, "0123456789")) {
				wp->set_uid = strtoul(wp->config->user, 0, 10);
			} else {
				struct passwd *pwd;

				pwd = getpwnam(wp->config->user);
				if (!pwd) {
					zlog(ZLOG_STUFF, ZLOG_ERROR, "[pool %s] cannot get uid for user '%s'", wp->config->name, wp->config->user);
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
					zlog(ZLOG_STUFF, ZLOG_ERROR, "[pool %s] cannot get gid for group '%s'", wp->config->name, wp->config->group);
					return -1;
				}
				wp->set_gid = grp->gr_gid;
			}
		}

#ifndef I_REALLY_WANT_ROOT_PHP
		if (wp->set_uid == 0 || wp->set_gid == 0) {
			zlog(ZLOG_STUFF, ZLOG_ERROR, "[pool %s] please specify user and group other than root", wp->config->name);
			return -1;
		}
#endif
	} else { /* not root */
		if (wp->config->user && *wp->config->user) {
			zlog(ZLOG_STUFF, ZLOG_WARNING, "[pool %s] 'user' directive is ignored", wp->config->name);
		}
		if (wp->config->group && *wp->config->group) {
			zlog(ZLOG_STUFF, ZLOG_WARNING, "[pool %s] 'group' directive is ignored", wp->config->name);
		}
		if (wp->config->chroot && *wp->config->chroot) {
			zlog(ZLOG_STUFF, ZLOG_WARNING, "[pool %s] 'chroot' directive is ignored", wp->config->name);
		}

		{ /* set up HOME and USER anyway */
			struct passwd *pwd;

			pwd = getpwuid(getuid());
			if (pwd) {
				wp->user = strdup(pwd->pw_name);
				wp->home = strdup(pwd->pw_dir);
			}
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
			zlog(ZLOG_STUFF, ZLOG_SYSERROR, "[pool %s] setrlimit(RLIMIT_NOFILE, %d) failed (%d)", wp->config->name, wp->config->rlimit_files, errno);
		}
	}

	if (wp->config->rlimit_core) {
		struct rlimit r;

		r.rlim_max = r.rlim_cur = wp->config->rlimit_core == -1 ? (rlim_t) RLIM_INFINITY : (rlim_t) wp->config->rlimit_core;

		if (0 > setrlimit(RLIMIT_CORE, &r)) {
			zlog(ZLOG_STUFF, ZLOG_SYSERROR, "[pool %s] setrlimit(RLIMIT_CORE, %d) failed (%d)", wp->config->name, wp->config->rlimit_core, errno);
		}
	}

	if (is_root && wp->config->chroot && *wp->config->chroot) {
		if (0 > chroot(wp->config->chroot)) {
			zlog(ZLOG_STUFF, ZLOG_SYSERROR, "[pool %s] chroot(%s) failed",  wp->config->name, wp->config->chroot);
			return -1;
		}
		made_chroot = 1;
	}

	if (wp->config->chdir && *wp->config->chdir) {
		if (0 > chdir(wp->config->chdir)) {
			zlog(ZLOG_STUFF, ZLOG_SYSERROR, "[pool %s] chdir(%s) failed", wp->config->name, wp->config->chdir);
			return -1;
		}
	} else if (made_chroot) {
		chdir("/");
	}

	if (is_root) {
		if (wp->set_gid) {
			if (0 > setgid(wp->set_gid)) {
				zlog(ZLOG_STUFF, ZLOG_SYSERROR, "[pool %s] setgid(%d) failed", wp->config->name, wp->set_gid);
				return -1;
			}
		}
		if (wp->set_uid) {
			if (0 > initgroups(wp->config->user, wp->set_gid)) {
				zlog(ZLOG_STUFF, ZLOG_SYSERROR, "[pool %s] initgroups(%s, %d) failed", wp->config->name, wp->config->user, wp->set_gid);
				return -1;
			}
			if (0 > setuid(wp->set_uid)) {
				zlog(ZLOG_STUFF, ZLOG_SYSERROR, "[pool %s] setuid(%d) failed", wp->config->name, wp->set_uid);
				return -1;
			}
		}
	}

#ifdef HAVE_PRCTL
	if (0 > prctl(PR_SET_DUMPABLE, 1, 0, 0, 0)) {
		zlog(ZLOG_STUFF, ZLOG_SYSERROR, "[pool %s] prctl(PR_SET_DUMPABLE) failed", wp->config->name);
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

	fpm_pagesize = getpagesize();
	if (fpm_global_config.daemonize) {
		switch (fork()) {
			case -1 :
				zlog(ZLOG_STUFF, ZLOG_SYSERROR, "daemonized fork() failed");
				return -1;
			case 0 :
				break;
			default :
				fpm_cleanups_run(FPM_CLEANUP_PARENT_EXIT);
				exit(0);
		}
	}

	setsid();
	if (0 > fpm_clock_init()) {
		return -1;
	}

	fpm_globals.parent_pid = getpid();
	for (wp = fpm_worker_all_pools; wp; wp = wp->next) {
		if (0 > fpm_unix_conf_wp(wp)) {
			return -1;
		}
	}

	fpm_stdio_init_final();
	return 0;
}
/* }}} */

