
	/* $Id: fpm.c,v 1.23 2008/07/20 16:38:31 anight Exp $ */
	/* (c) 2007,2008 Andrei Nigmatulin */

#include "fpm_config.h"

#include <stdlib.h> /* for exit */

#include "fpm.h"
#include "fpm_children.h"
#include "fpm_signals.h"
#include "fpm_env.h"
#include "fpm_events.h"
#include "fpm_cleanup.h"
#include "fpm_php.h"
#include "fpm_sockets.h"
#include "fpm_unix.h"
#include "fpm_process_ctl.h"
#include "fpm_conf.h"
#include "fpm_worker_pool.h"
#include "fpm_stdio.h"
#include "zlog.h"

struct fpm_globals_s fpm_globals = {
		.parent_pid = 0, 
		.argc = NULL,
		.argv = NULL,
		.config = NULL,
		.prefix = NULL,
		.running_children = 0,
		.error_log_fd = 0,
		.log_level = 0,
		.listening_socket = 0,
		.max_requests = 0,
		.is_child = 0,
		.test_successful = 0
	};

int fpm_init(int argc, char **argv, char *config, char *prefix, int test_conf) /* {{{ */
{
	fpm_globals.argc = argc;
	fpm_globals.argv = argv;
	if (config && *config) {
		fpm_globals.config = strdup(config);
	}
	fpm_globals.prefix = prefix;

	if (0 > fpm_php_init_main()              ||
		0 > fpm_stdio_init_main()            ||
		0 > fpm_conf_init_main(test_conf)    ||
		0 > fpm_unix_init_main()             ||
		0 > fpm_pctl_init_main()             ||
		0 > fpm_env_init_main()              ||
		0 > fpm_signals_init_main()          ||
		0 > fpm_children_init_main()         ||
		0 > fpm_sockets_init_main()          ||
		0 > fpm_worker_pool_init_main()      ||
		0 > fpm_event_init_main()) {
		
		if (fpm_globals.test_successful) {
			exit(0);
		} else {
			return -1;
		}
	}

	if (0 > fpm_conf_write_pid()) {
		return -1;
	}

	fpm_stdio_init_final();
	zlog(ZLOG_NOTICE, "fpm is running, pid %d", (int) fpm_globals.parent_pid);

	return 0;
}
/* }}} */

/*	children: return listening socket
	parent: never return */
int fpm_run(int *max_requests) /* {{{ */
{
	struct fpm_worker_pool_s *wp;

	/* create initial children in all pools */
	for (wp = fpm_worker_all_pools; wp; wp = wp->next) {
		int is_parent;

		is_parent = fpm_children_create_initial(wp);

		if (!is_parent) {
			goto run_child;
		}
	}

	/* run event loop forever */
	fpm_event_loop();

run_child: /* only workers reach this point */

	fpm_cleanups_run(FPM_CLEANUP_CHILD);

	*max_requests = fpm_globals.max_requests;
	return fpm_globals.listening_socket;
}
/* }}} */

