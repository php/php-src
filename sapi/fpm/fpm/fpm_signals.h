	/* (c) 2007,2008 Andrei Nigmatulin */

#ifndef FPM_SIGNALS_H
#define FPM_SIGNALS_H 1

#include <signal.h>

int fpm_signals_init_main();
int fpm_signals_init_child();
int fpm_signals_get_fd();
int fpm_signals_init_mask();
int fpm_signals_block();
int fpm_signals_child_block();
int fpm_signals_unblock();

extern const char *fpm_signal_names[NSIG + 1];

#endif
