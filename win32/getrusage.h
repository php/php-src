/*
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
   | Authors: Kalle Sommer Nielsen <kalle@php.net>                        |
   +----------------------------------------------------------------------+
 */

#ifndef HAVE_GETRUSAGE_H
# define HAVE_GETRUSAGE_H

/*
 * Note
 *
 * RUSAGE_CHILDREN is not implemented, and the RUSAGE_THREAD will
 * therefore instead be used instead to emulate the behavior.
 */

# define RUSAGE_SELF		0
# define RUSAGE_CHILDREN	1

# define RUSAGE_THREAD		RUSAGE_CHILDREN

/*
 * Implementation support
 *
 *  RUSAGE_SELF
 *		ru_utime
 *		ru_stime
 *		ru_majflt
 *		ru_maxrss
 *
 *  RUSAGE_THREAD
 *		ru_utime
 *		ru_stime
 *
 * Not implemented:
 *		ru_ixrss		(unused)
 *		ru_idrss		(unused)
 *		ru_isrss		(unused)
 *		ru_minflt
 *		ru_nswap		(unused)
 *		ru_inblock
 *		ru_oublock
 *		ru_msgsnd		(unused)
 *		ru_msgrcv		(unused)
 *		ru_nsignals		(unused)
 *		ru_nvcsw
 *		ru_nivcsw
 */

struct rusage
{
	/* User time used */
	struct timeval ru_utime;

	/* System time used */
	struct timeval ru_stime;

	/* Integral max resident set size */
	zend_long ru_maxrss;

	/* Page faults */
	zend_long ru_majflt;
#if 0
	/* Integral shared text memory size */
	zend_long ru_ixrss;

	/* Integral unshared data size */
	zend_long ru_idrss;

	/* Integral unshared stack size */
	zend_long ru_isrss;

	/* Page reclaims */
	zend_long ru_minflt;

	/* Swaps */
	zend_long ru_nswap;

	/* Block input operations */
	zend_long ru_inblock;

	/* Block output operations */
	zend_long ru_oublock;

	/* Messages sent */
	zend_long ru_msgsnd;

	/* Messages received */
	zend_long ru_msgrcv;

	/* Signals received */
	zend_long ru_nsignals;

	/* Voluntary context switches */
	zend_long ru_nvcsw;

	/* Involuntary context switches */
	zend_long ru_nivcsw;
#endif
};

PHPAPI int getrusage(int who, struct rusage *usage);

#endif
