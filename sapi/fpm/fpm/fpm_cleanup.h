
	/* $Id: fpm_cleanup.h,v 1.5 2008/05/24 17:38:47 anight Exp $ */
	/* (c) 2007,2008 Andrei Nigmatulin */

#ifndef FPM_CLEANUP_H
#define FPM_CLEANUP_H 1

int fpm_cleanup_add(int type, void (*cleanup)(int, void *), void *);
void fpm_cleanups_run(int type);

enum {
	FPM_CLEANUP_CHILD					= (1 << 0),
	FPM_CLEANUP_PARENT_EXIT				= (1 << 1),
	FPM_CLEANUP_PARENT_EXIT_MAIN		= (1 << 2),
	FPM_CLEANUP_PARENT_EXEC				= (1 << 3),
	FPM_CLEANUP_PARENT					= (1 << 1) | (1 << 2) | (1 << 3),
	FPM_CLEANUP_ALL						= ~0,
};

#endif

