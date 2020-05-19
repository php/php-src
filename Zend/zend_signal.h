/*
  +----------------------------------------------------------------------+
  | Blocking and Unblocking Signals                                      |
  +----------------------------------------------------------------------+
  | Copyright (c) The PHP Group                                          |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
 */

#ifndef ZEND_SIGNAL_H
#define ZEND_SIGNAL_H

#include <signal.h>

#ifdef HAVE_SIGPROCMASK

extern sigset_t mask_all_signals;

# if ZEND_DEBUG
extern TSRM_TLS int _signals_masked;

#  define DEBUG_BLOCK_ALL_SIGNALS() _signals_masked += 1
#  define DEBUG_UNBLOCK_ALL_SIGNALS() \
  if (--_signals_masked) \
    zend_error_noreturn(E_ERROR, "Cannot nest ZEND_SIGNAL_BLOCK_INTERRUPTIONS; it is not re-entrant")
# else
#  define DEBUG_BLOCK_ALL_SIGNALS()   do {} while (0)
#  define DEBUG_UNBLOCK_ALL_SIGNALS() do {} while (0)
# endif

# define ZEND_SIGNAL_BLOCK_INTERRUPTIONS() \
  sigset_t _oldmask; \
  DEBUG_BLOCK_ALL_SIGNALS(); \
  MASK_ALL_SIGNALS()
# define ZEND_SIGNAL_UNBLOCK_INTERRUPTIONS() \
  DEBUG_UNBLOCK_ALL_SIGNALS(); \
  UNMASK_ALL_SIGNALS()

# ifdef ZTS
#  define MASK_ALL_SIGNALS() \
  tsrm_sigmask(SIG_BLOCK, &mask_all_signals, &_oldmask)
#  define UNMASK_ALL_SIGNALS() \
  tsrm_sigmask(SIG_SETMASK, &_oldmask, NULL)
# else
#  define MASK_ALL_SIGNALS() \
  sigprocmask(SIG_BLOCK, &mask_all_signals, &_oldmask)
#  define UNMASK_ALL_SIGNALS() \
  sigprocmask(SIG_SETMASK, &_oldmask, NULL)
# endif

#else

# define ZEND_SIGNAL_BLOCK_INTERRUPTIONS()   do {} while(0)
# define ZEND_SIGNAL_UNBLOCK_INTERRUPTIONS() do {} while(0)

#endif /* HAVE_SIGPROCMASK */
#endif /* ZEND_SIGNAL_H */
