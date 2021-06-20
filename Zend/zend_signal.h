/*
  +----------------------------------------------------------------------+
  | Zend Signal Handling                                                 |
  +----------------------------------------------------------------------+
  | Copyright (c) The PHP Group                                          |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | https://www.php.net/license/3_01.txt                                 |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Lucas Nealan <lucas@php.net>                                |
  |          Arnaud Le Blanc <lbarnaud@php.net>                          |
  +----------------------------------------------------------------------+

 */

#ifndef ZEND_SIGNAL_H
#define ZEND_SIGNAL_H

#ifdef ZEND_SIGNALS

#include <signal.h>

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
	bool check;        /* check for replaced handlers on shutdown */
	bool reset;        /* reset signal handlers on each request */
	zend_signal_entry_t handlers[NSIG];
	zend_signal_queue_t pstorage[ZEND_SIGNAL_QUEUE_SIZE], *phead, *ptail, *pavail; /* pending queue */
} zend_signal_globals_t;

# ifdef ZTS
#  define SIGG(v) ZEND_TSRMG_FAST(zend_signal_globals_offset, zend_signal_globals_t *, v)
BEGIN_EXTERN_C()
ZEND_API extern int zend_signal_globals_id;
ZEND_API extern size_t zend_signal_globals_offset;
END_EXTERN_C()
# else
#  define SIGG(v) (zend_signal_globals.v)
BEGIN_EXTERN_C()
ZEND_API extern zend_signal_globals_t zend_signal_globals;
END_EXTERN_C()
# endif /* not ZTS */

# ifdef ZTS
#  define ZEND_SIGNAL_BLOCK_INTERRUPTIONS() if (EXPECTED(zend_signal_globals_id)) { SIGG(depth)++; }
#  define ZEND_SIGNAL_UNBLOCK_INTERRUPTIONS() if (EXPECTED(zend_signal_globals_id) && UNEXPECTED(((SIGG(depth)--) == SIGG(blocked)))) { zend_signal_handler_unblock(); }
# else /* ZTS */
#  define ZEND_SIGNAL_BLOCK_INTERRUPTIONS()  SIGG(depth)++;
#  define ZEND_SIGNAL_UNBLOCK_INTERRUPTIONS() if (((SIGG(depth)--) == SIGG(blocked))) { zend_signal_handler_unblock(); }
# endif /* not ZTS */

ZEND_API void zend_signal_handler_unblock(void);
void zend_signal_activate(void);
void zend_signal_deactivate(void);
BEGIN_EXTERN_C()
ZEND_API void zend_signal_startup(void);
END_EXTERN_C()
void zend_signal_init(void);

ZEND_API void zend_signal(int signo, void (*handler)(int));
ZEND_API void zend_sigaction(int signo, const struct sigaction *act, struct sigaction *oldact);

#else /* ZEND_SIGNALS */

# define ZEND_SIGNAL_BLOCK_INTERRUPTIONS()
# define ZEND_SIGNAL_UNBLOCK_INTERRUPTIONS()

# define zend_signal_activate()
# define zend_signal_deactivate()
# define zend_signal_startup()
# define zend_signal_init()

# define zend_signal(signo, handler)           signal(signo, handler)
# define zend_sigaction(signo, act, oldact)    sigaction(signo, act, oldact)

#endif /* ZEND_SIGNALS */

#endif /* ZEND_SIGNAL_H */
