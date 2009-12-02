
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

int fpm_unix_resolve_socket_premissions(struct fpm_worker_pool_s *wp)
{
	struct fpm_listen_options_s *lo = wp->config->listen_options;

	/* uninitialized */
	wp->socket_uid = -1;
	wp->socket_gid = -1;
	wp->socket_mode = 0666;

	if (!lo) return 0;

	if (lo->owner && *lo->owner) {
		struct passwd *pwd;

		pwd = getpwnam(lo->owner);

		if (!pwd) {
			zlog(ZLOG_STUFF, ZLOG_SYSERROR, "cannot get uid for user '%s', pool '%s'", lo->owner, wp->config->name);
			return -1;
		}

		wp->socket_uid = pwd->pw_uid;
		wp->socket_gid = pwd->pw_gid;
	}

	if (lo->group && *lo->group) {
		struct group *grp;

		grp = getgrnam(lo->group);

		if (!grp) {
			zlog(ZLOG_STUFF, ZLOG_SYSERROR, "cannot get gid for group '%s', pool '%s'", lo->group, wp->config->name);
			return -1;
		}

		wp->socket_gid = grp->gr_gid;
	}

	if (lo->mode && *lo->mode) {
		wp->socket_mode = strtoul(lo->mode, 0, 8);
	}

	return 0;
}

static int fpm_unix_conf_wp(struct fpm_worker_pool_s *wp)
{
	int is_root = !geteuid();

	if (is_root) {
		if (wp->config->user && *wp->config->user) {

			if (strlen(wp->config->user) == strspn(wp->config->user, "0123456789")) {
				wp->set_uid = strtoul(wp->config->user, 0, 10);
			}
			else {
				struct passwd *pwd;

				pwd = getpwnam(wp->config->user);

				if (!pwd) {
					zlog(ZLOG_STUFF, ZLOG_ERROR, "cannot get uid for user '%s', pool '%s'", wp->config->user, wp->config->name);
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
			}
			else {
				struct group *grp;

				grp = getgrnam(wp->config->group);

				if (!grp) {
					zlog(ZLOG_STUFF, ZLOG_ERROR, "cannot get gid for group '%s', pool '%s'", wp->config->group, wp->config->name);
					return -1;
				}

				wp->set_gid = grp->gr_gid;
			}
		}

#ifndef I_REALLY_WANT_ROOT_PHP
		if (wp->set_uid == 0 || wp->set_gid == 0) {
			zlog(ZLOG_STUFF, ZLOG_ERROR, "please specify user and group other than root, pool '%s'", wp->config->name);
			return -1;
		}
#endif
	}
	else { /* not root */
		if (wp->config->user && *wp->config->user) {
			zlog(ZLOG_STUFF, ZLOG_WARNING, "'user' directive is ignored, pool '%s'", wp->config->name);
		}
		if (wp->config->group && *wp->config->group) {
			zlog(ZLOG_STUFF, ZLOG_WARNING, "'group' directive is ignored, pool '%s'", wp->config->name);
		}
		if (wp->config->chroot && *wp->config->chroot) {
			zlog(ZLOG_STUFF, ZLOG_WARNING, "'chroot' directive is ignored, pool '%s'", wp->config->name);
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

int fpm_unix_init_child(struct fpm_worker_pool_s *wp)
{
	int is_root = !geteuid();
	int made_chroot = 0;

	if (wp->config->rlimit_files) {
		struct rlimit r;

		getrlimit(RLIMIT_NOFILE, &r);

		r.rlim_cur = (rlim_t) wp->config->rlimit_files;

		if (0 > setrlimit(RLIMIT_NOFILE, &r)) {
			zlog(ZLOG_STUFF, ZLOG_SYSERROR, "setrlimit(RLIMIT_NOFILE) failed");
		}
	}

	if (wp->config->rlimit_core) {
		struct rlimit r;

		getrlimit(RLIMIT_CORE, &r);

		r.rlim_cur = wp->config->rlimit_core == -1 ? (rlim_t) RLIM_INFINITY : (rlim_t) wp->config->rlimit_core;

		if (0 > setrlimit(RLIMIT_CORE, &r)) {
			zlog(ZLOG_STUFF, ZLOG_SYSERROR, "setrlimit(RLIMIT_CORE) failed");
		}
	}

	if (is_root && wp->config->chroot && *wp->config->chroot) {
		if (0 > chroot(wp->config->chroot)) {
			zlog(ZLOG_STUFF, ZLOG_SYSERROR, "chroot(%s) failed", wp->config->chroot);
			return -1;
		}
		made_chroot = 1;
	}

	if (wp->config->chdir && *wp->config->chdir) {
		if (0 > chdir(wp->config->chdir)) {
			zlog(ZLOG_STUFF, ZLOG_SYSERROR, "chdir(%s) failed", wp->config->chdir);
			return -1;
		}
	}
	else if (made_chroot) {
		chdir("/");
	}

	if (is_root) {
		if (wp->set_gid) {
			if (0 > setgid(wp->set_gid)) {
				zlog(ZLOG_STUFF, ZLOG_SYSERROR, "setgid(%d) failed", wp->set_gid);
				return -1;
			}
		}
		if (wp->set_uid) {
			if (0 > initgroups(wp->config->user, wp->set_gid)) {
				zlog(ZLOG_STUFF, ZLOG_SYSERROR, "initgroups(%s, %d) failed", wp->config->user, wp->set_gid);
				return -1;
			}
			if (0 > setuid(wp->set_uid)) {
				zlog(ZLOG_STUFF, ZLOG_SYSERROR, "setuid(%d) failed", wp->set_uid);
				return -1;
			}
		}
	}

#ifdef HAVE_PRCTL
	if (0 > prctl(PR_SET_DUMPABLE, 1, 0, 0, 0)) {
		zlog(ZLOG_STUFF, ZLOG_SYSERROR, "prctl(PR_SET_DUMPABLE) failed");
	}
#endif

	if (0 > fpm_clock_init()) {
		return -1;
	}

	return 0;
}

int fpm_unix_init_main()
{
	struct fpm_worker_pool_s *wp;

	fpm_pagesize = getpagesize();

	if (fpm_global_config.daemonize) {

		switch (fork()) {

			case -1 :

				zlog(ZLOG_STUFF, ZLOG_SYSERROR, "fork() failed");
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

	{
		struct rlimit r;
		getrlimit(RLIMIT_NOFILE, &r);

		zlog(ZLOG_STUFF, ZLOG_NOTICE, "getrlimit(nofile): max:%lld, cur:%lld",
			(long long) r.rlim_max, (long long) r.rlim_cur);
	}

	return 0;
}
