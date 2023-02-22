	/* (c) 2007,2008 Andrei Nigmatulin */

#ifndef FPM_SIGNALS_H
#define FPM_SIGNALS_H 1

#include "zend_result.h"

#include <signal.h>

zend_result fpm_signals_init_main(void);
zend_result fpm_signals_init_child(void);
int fpm_signals_get_fd(void);
zend_result fpm_signals_init_mask(void);
zend_result fpm_signals_block(void);
zend_result fpm_signals_child_block(void);
zend_result fpm_signals_unblock(void);

extern const char *fpm_signal_names[NSIG + 1];

#endif
