	/* (c) 2007,2008 Andrei Nigmatulin */

#ifndef FPM_SIGNALS_H
#define FPM_SIGNALS_H 1

#include <signal.h>

struct fpm_signal_event_s {
	char sig_char;
	pid_t pid;
};

int fpm_signals_init_main(void);
int fpm_signals_init_child(void);
int fpm_signals_get_fd(void);
int fpm_signals_init_mask(void);
int fpm_signals_block(void);
int fpm_signals_child_block(void);
int fpm_signals_unblock(void);

extern const char *fpm_signal_names[NSIG + 1];

#endif
