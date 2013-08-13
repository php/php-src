
	/* $Id: fpm_signals.h,v 1.5 2008/05/24 17:38:47 anight Exp $ */
	/* (c) 2007,2008 Andrei Nigmatulin */

#ifndef FPM_SIGNALS_H
#define FPM_SIGNALS_H 1

#include <signal.h>

int fpm_signals_init_main();
int fpm_signals_init_child();
int fpm_signals_get_fd();

extern const char *fpm_signal_names[NSIG + 1];

#endif
