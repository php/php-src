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
ZEND_API void zend_debug_mask_signals();
ZEND_API void zend_debug_unmask_signals();
ZEND_API void zend_debug_ensure_signals_masked();
# else
#  define zend_debug_mask_signals()   do {} while (0)
#  define zend_debug_unmask_signals() do {} while (0)
#  define zend_debug_ensure_signals_masked() do {} while (0)
# endif

# define ZEND_SIGNAL_BLOCK_INTERRUPTIONS() \
  sigset_t _oldmask; \
  zend_debug_mask_signals(); \
  MASK_ALL_SIGNALS()
# define ZEND_SIGNAL_UNBLOCK_INTERRUPTIONS() \
  zend_debug_unmask_signals(); \
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
# define zend_debug_ensure_signals_masked()  do {} while(0)

#endif /* HAVE_SIGPROCMASK */
#endif /* ZEND_SIGNAL_H */
