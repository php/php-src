
	/* $Id: fpm_cleanup.c,v 1.8 2008/05/24 17:38:47 anight Exp $ */
	/* (c) 2007,2008 Andrei Nigmatulin */

#include "fpm_config.h"

#include <stdlib.h>

#include "fpm_arrays.h"
#include "fpm_cleanup.h"
#include "zlog.h"

struct cleanup_s {
	int type;
	void (*cleanup)(int, void *);
	void *arg;
};

static struct fpm_array_s cleanups = { .sz = sizeof(struct cleanup_s) };

int fpm_cleanup_add(int type, void (*cleanup)(int, void *), void *arg) /* {{{ */
{
	struct cleanup_s *c;

	c = fpm_array_push(&cleanups);

	if (!c) {
		return -1;
	}

	c->type = type;
	c->cleanup = cleanup;
	c->arg = arg;

	return 0;
}
/* }}} */

void fpm_cleanups_run(int type) /* {{{ */
{
	struct cleanup_s *c = fpm_array_item_last(&cleanups);
	int cl = cleanups.used;

	for ( ; cl--; c--) {
		if (c->type & type) {
			c->cleanup(type, c->arg);
		}
	}

	fpm_array_free(&cleanups);
}
/* }}} */

