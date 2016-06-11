/*
  +----------------------------------------------------------------------+
  | Zend Signal Handling                                                 |
  +----------------------------------------------------------------------+
  | Copyright (c) 2008 The PHP Group                                     |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Lucas Nealan <lucas@php.net>                                |
  |          Arnaud Le Blanc <lbarnaud@php.net>                          |
  +----------------------------------------------------------------------+

 */

/* $Id$ */

#ifndef ZEND_SIGNAL_H
#define ZEND_SIGNAL_H

#ifdef HAVE_SIGNAL_H
#include <signal.h>
#endif

#ifndef NSIG
#define NSIG 65
#endif

#ifndef ZEND_SIGNAL_QUEUE_SIZE
#define ZEND_SIGNAL_QUEUE_SIZE 64
#endif

/* Signal structs */
typedef struct _zend_signal_entry_t {
	int   flags;          /* sigaction style flags */
	void* handler;      /* signal handler or context */
} zend_signal_entry_t;

typedef struct _zend_signal_t {
	int signo;
	siginfo_t *siginfo;
	void* context;
} zend_signal_t;

typedef struct _zend_signal_queue_t {
	zend_signal_t zend_signal;
	struct _zend_signal_queue_t *next;
} zend_signal_queue_t;

/* Signal Globals */
typedef struct _zend_signal_globals_t {
	int depth;
	int blocked;            /* 1==TRUE, 0==FALSE */
	int running;            /* in signal handler execution */
	int active;             /* internal signal handling is enabled */
	zend_bool check;        /* check for replaced handlers on shutdown */
	zend_signal_entry_t handlers[NSIG];
	zend_signal_queue_t pstorage[ZEND_SIGNAL_QUEUE_SIZE], *phead, *ptail, *pavail; /* pending queue */
} zend_signal_globals_t;

#ifdef ZTS
# define SIGG(v) ZEND_TSRMG(zend_signal_globals_id, zend_signal_globals_t *, v)
BEGIN_EXTERN_C()
ZEND_API extern int zend_signal_globals_id;
END_EXTERN_C()
# define ZEND_SIGNAL_BLOCK_INTERRUPUTIONS() if (EXPECTED(zend_signal_globals_id)) { SIGG(depth)++; }
# define ZEND_SIGNAL_UNBLOCK_INTERRUPTIONS() if (EXPECTED(zend_signal_globals_id) && UNEXPECTED(((SIGG(depth)--) == SIGG(blocked)))) { zend_signal_handler_unblock(); }
#else /* ZTS */
# define SIGG(v) (zend_signal_globals.v)
extern ZEND_API zend_signal_globals_t zend_signal_globals;
# define ZEND_SIGNAL_BLOCK_INTERRUPUTIONS()  SIGG(depth)++;
# define ZEND_SIGNAL_UNBLOCK_INTERRUPTIONS() if (((SIGG(depth)--) == SIGG(blocked))) { zend_signal_handler_unblock(); }
#endif /* not ZTS */

# define SIGNAL_BEGIN_CRITICAL() 	sigset_t oldmask; \
	zend_sigprocmask(SIG_BLOCK, &global_sigmask, &oldmask);
# define SIGNAL_END_CRITICAL()		zend_sigprocmask(SIG_SETMASK, &oldmask, NULL);

void zend_signal_handler_defer(int signo, siginfo_t *siginfo, void *context);
ZEND_API void zend_signal_handler_unblock();
void zend_signal_activate(void);
void zend_signal_deactivate(void);
void zend_signal_startup();
void zend_signal_init();
void zend_signal_shutdown(void);
ZEND_API int zend_signal(int signo, void (*handler)(int));
ZEND_API int zend_sigaction(int signo, const struct sigaction *act, struct sigaction *oldact);

#ifdef ZTS
#define zend_sigprocmask(signo, set, oldset) tsrm_sigmask((signo), (set), (oldset))
#else
#define zend_sigprocmask(signo, set, oldset) sigprocmask((signo), (set), (oldset))
#endif

#endif /* ZEND_SIGNAL_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
