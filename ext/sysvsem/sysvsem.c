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
   | Authors: Tom May <tom@go2net.com>                                    |
   |          Gavin Sherry <gavin@linuxworld.com.au>                      |
   +----------------------------------------------------------------------+
 */

/* Latest update build anc tested on Linux 2.2.14
 *
 * This has been built and tested on Solaris 2.6 and Linux 2.1.122.
 * It may not compile or execute correctly on other systems.
 *
 * sas: Works for me on Linux 2.0.36 and FreeBSD 3.0-current
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"

#if HAVE_SYSVSEM

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>

#include "sysvsem_arginfo.h"
#include "php_sysvsem.h"
#include "ext/standard/info.h"

#if !HAVE_SEMUN

union semun {
	int val;                    /* value for SETVAL */
	struct semid_ds *buf;       /* buffer for IPC_STAT, IPC_SET */
	unsigned short int *array;  /* array for GETALL, SETALL */
	struct seminfo *__buf;      /* buffer for IPC_INFO */
};

#undef HAVE_SEMUN
#define HAVE_SEMUN 1

#endif

/* {{{ sysvsem_module_entry
 */
zend_module_entry sysvsem_module_entry = {
	STANDARD_MODULE_HEADER,
	"sysvsem",
	ext_functions,
	PHP_MINIT(sysvsem),
	NULL,
	NULL,
	NULL,
	PHP_MINFO(sysvsem),
	PHP_SYSVSEM_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_SYSVSEM
ZEND_GET_MODULE(sysvsem)
#endif


THREAD_LS sysvsem_module php_sysvsem_module;

/* Semaphore functions using System V semaphores.  Each semaphore
 * actually consists of three semaphores allocated as a unit under the
 * same key.  Semaphore 0 (SYSVSEM_SEM) is the actual semaphore, it is
 * initialized to max_acquire and decremented as processes acquire it.
 * The value of semaphore 1 (SYSVSEM_USAGE) is a count of the number
 * of processes using the semaphore.  After calling semget(), if a
 * process finds that the usage count is 1, it will set the value of
 * SYSVSEM_SEM to max_acquire.  This allows max_acquire to be set and
 * track the PHP code without having a global init routine or external
 * semaphore init code.  Except see the bug regarding a race condition
 * php_sysvsem_get().  Semaphore 2 (SYSVSEM_SETVAL) serializes the
 * calls to GETVAL SYSVSEM_USAGE and SETVAL SYSVSEM_SEM.  It can be
 * acquired only when it is zero.
 */

#define SYSVSEM_SEM		0
#define SYSVSEM_USAGE	1
#define SYSVSEM_SETVAL	2

/* {{{ release_sysvsem_sem
 */
static void release_sysvsem_sem(zend_resource *rsrc)
{
	sysvsem_sem *sem_ptr = (sysvsem_sem *)rsrc->ptr;
	struct sembuf sop[2];
	int opcount = 1;
/*
 * if count == -1, semaphore has been removed
 * Need better way to handle this
 */

	if (sem_ptr->count == -1 || !sem_ptr->auto_release) {
		efree(sem_ptr);
		return;
	}
	/* Decrement the usage count. */

	sop[0].sem_num = SYSVSEM_USAGE;
	sop[0].sem_op  = -1;
	sop[0].sem_flg = SEM_UNDO;

	/* Release the semaphore if it has been acquired but not released. */

	if (sem_ptr->count) {

		sop[1].sem_num = SYSVSEM_SEM;
		sop[1].sem_op  = sem_ptr->count;
		sop[1].sem_flg = SEM_UNDO;

		opcount++;
	}

	semop(sem_ptr->semid, sop, opcount);
	efree(sem_ptr);
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(sysvsem)
{
	php_sysvsem_module.le_sem = zend_register_list_destructors_ex(release_sysvsem_sem, NULL, "sysvsem", module_number);
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(sysvsem)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "sysvsem support", "enabled");
	php_info_print_table_end();
}
/* }}} */

#define SETVAL_WANTS_PTR

#if defined(_AIX)
#undef SETVAL_WANTS_PTR
#endif

/* {{{ proto resource sem_get(int key [, int max_acquire [, int perm [, int auto_release]])
   Return an id for the semaphore with the given key, and allow max_acquire (default 1) processes to acquire it simultaneously */
PHP_FUNCTION(sem_get)
{
	zend_long key, max_acquire = 1, perm = 0666, auto_release = 1;
	int semid;
	struct sembuf sop[3];
	int count;
	sysvsem_sem *sem_ptr;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS(), "l|lll", &key, &max_acquire, &perm, &auto_release)) {
		RETURN_THROWS();
	}

	/* Get/create the semaphore.  Note that we rely on the semaphores
	 * being zeroed when they are created.  Despite the fact that
	 * the(?)  Linux semget() man page says they are not initialized,
	 * the kernel versions 2.0.x and 2.1.z do in fact zero them.
	 */

	semid = semget(key, 3, perm|IPC_CREAT);
	if (semid == -1) {
		php_error_docref(NULL, E_WARNING, "Failed for key 0x" ZEND_XLONG_FMT ": %s", key, strerror(errno));
		RETURN_FALSE;
	}

	/* Find out how many processes are using this semaphore.  Note
	 * that on Linux (at least) there is a race condition here because
	 * semaphore undo on process exit is not atomic, so we could
	 * acquire SYSVSEM_SETVAL before a crashed process has decremented
	 * SYSVSEM_USAGE in which case count will be greater than it
	 * should be and we won't set max_acquire.  Fortunately this
	 * doesn't actually matter in practice.
	 */

	/* Wait for sem 1 to be zero . . . */

	sop[0].sem_num = SYSVSEM_SETVAL;
	sop[0].sem_op  = 0;
	sop[0].sem_flg = 0;

	/* . . . and increment it so it becomes non-zero . . . */

	sop[1].sem_num = SYSVSEM_SETVAL;
	sop[1].sem_op  = 1;
	sop[1].sem_flg = SEM_UNDO;

	/* . . . and increment the usage count. */

	sop[2].sem_num = SYSVSEM_USAGE;
	sop[2].sem_op  = 1;
	sop[2].sem_flg = SEM_UNDO;
	while (semop(semid, sop, 3) == -1) {
		if (errno != EINTR) {
			php_error_docref(NULL, E_WARNING, "Failed acquiring SYSVSEM_SETVAL for key 0x" ZEND_XLONG_FMT ": %s", key, strerror(errno));
			break;
		}
	}

	/* Get the usage count. */
	count = semctl(semid, SYSVSEM_USAGE, GETVAL, NULL);
	if (count == -1) {
		php_error_docref(NULL, E_WARNING, "Failed for key 0x" ZEND_XLONG_FMT ": %s", key, strerror(errno));
	}

	/* If we are the only user, then take this opportunity to set the max. */

	if (count == 1) {
#if HAVE_SEMUN
		/* This is correct for Linux which has union semun. */
		union semun semarg;
		semarg.val = max_acquire;
		if (semctl(semid, SYSVSEM_SEM, SETVAL, semarg) == -1) {
			php_error_docref(NULL, E_WARNING, "Failed for key 0x" ZEND_XLONG_FMT ": %s", key, strerror(errno));
		}
#elif defined(SETVAL_WANTS_PTR)
		/* This is correct for Solaris 2.6 which does not have union semun. */
		if (semctl(semid, SYSVSEM_SEM, SETVAL, &max_acquire) == -1) {
			php_error_docref(NULL, E_WARNING, "Failed for key 0x%lx: %s", key, strerror(errno));
		}
#else
		/* This works for i.e. AIX */
		if (semctl(semid, SYSVSEM_SEM, SETVAL, max_acquire) == -1) {
			php_error_docref(NULL, E_WARNING, "Failed for key 0x%lx: %s", key, strerror(errno));
		}
#endif
	}

	/* Set semaphore 1 back to zero. */

	sop[0].sem_num = SYSVSEM_SETVAL;
	sop[0].sem_op  = -1;
	sop[0].sem_flg = SEM_UNDO;
	while (semop(semid, sop, 1) == -1) {
		if (errno != EINTR) {
			php_error_docref(NULL, E_WARNING, "Failed releasing SYSVSEM_SETVAL for key 0x" ZEND_XLONG_FMT ": %s", key, strerror(errno));
			break;
		}
	}

	sem_ptr = (sysvsem_sem *) emalloc(sizeof(sysvsem_sem));
	sem_ptr->key   = key;
	sem_ptr->semid = semid;
	sem_ptr->count = 0;
	sem_ptr->auto_release = auto_release;

	RETVAL_RES(zend_register_resource(sem_ptr, php_sysvsem_module.le_sem));
	sem_ptr->id = Z_RES_HANDLE_P(return_value);
}
/* }}} */

/* {{{ php_sysvsem_semop
 */
static void php_sysvsem_semop(INTERNAL_FUNCTION_PARAMETERS, int acquire)
{
	zval *arg_id;
	zend_bool nowait = 0;
	sysvsem_sem *sem_ptr;
	struct sembuf sop;

	if (acquire) {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "r|b", &arg_id, &nowait) == FAILURE) {
			RETURN_THROWS();
		}
	} else {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &arg_id) == FAILURE) {
			RETURN_THROWS();
		}
	}

	if ((sem_ptr = (sysvsem_sem *)zend_fetch_resource(Z_RES_P(arg_id), "SysV semaphore", php_sysvsem_module.le_sem)) == NULL) {
		RETURN_THROWS();
	}

	if (!acquire && sem_ptr->count == 0) {
		php_error_docref(NULL, E_WARNING, "SysV semaphore " ZEND_LONG_FMT " (key 0x%x) is not currently acquired", Z_LVAL_P(arg_id), sem_ptr->key);
		RETURN_FALSE;
	}

	sop.sem_num = SYSVSEM_SEM;
	sop.sem_op  = acquire ? -1 : 1;
	sop.sem_flg = SEM_UNDO | (nowait ? IPC_NOWAIT : 0);

	while (semop(sem_ptr->semid, &sop, 1) == -1) {
		if (errno != EINTR) {
			if (errno != EAGAIN) {
				php_error_docref(NULL, E_WARNING, "Failed to %s key 0x%x: %s", acquire ? "acquire" : "release", sem_ptr->key, strerror(errno));
			}
			RETURN_FALSE;
		}
	}

	sem_ptr->count -= acquire ? -1 : 1;
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool sem_acquire(resource id)
   Acquires the semaphore with the given id, blocking if necessary */
PHP_FUNCTION(sem_acquire)
{
	php_sysvsem_semop(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ proto bool sem_release(resource id)
   Releases the semaphore with the given id */
PHP_FUNCTION(sem_release)
{
	php_sysvsem_semop(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto bool sem_remove(resource id)
   Removes semaphore from Unix systems */

/*
 * contributed by Gavin Sherry gavin@linuxworld.com.au
 * Fri Mar 16 00:50:13 EST 2001
 */

PHP_FUNCTION(sem_remove)
{
	zval *arg_id;
	sysvsem_sem *sem_ptr;
#if HAVE_SEMUN
	union semun un;
	struct semid_ds buf;
#endif

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &arg_id) == FAILURE) {
		RETURN_THROWS();
	}

	if ((sem_ptr = (sysvsem_sem *)zend_fetch_resource(Z_RES_P(arg_id), "SysV semaphore", php_sysvsem_module.le_sem)) == NULL) {
		RETURN_THROWS();
	}

#if HAVE_SEMUN
	un.buf = &buf;
	if (semctl(sem_ptr->semid, 0, IPC_STAT, un) < 0) {
#else
	if (semctl(sem_ptr->semid, 0, IPC_STAT, NULL) < 0) {
#endif
		php_error_docref(NULL, E_WARNING, "SysV semaphore " ZEND_LONG_FMT " does not (any longer) exist", Z_LVAL_P(arg_id));
		RETURN_FALSE;
	}

#if HAVE_SEMUN
	if (semctl(sem_ptr->semid, 0, IPC_RMID, un) < 0) {
#else
	if (semctl(sem_ptr->semid, 0, IPC_RMID, NULL) < 0) {
#endif
		php_error_docref(NULL, E_WARNING, "Failed for SysV semaphore " ZEND_LONG_FMT ": %s", Z_LVAL_P(arg_id), strerror(errno));
		RETURN_FALSE;
	}

	/* let release_sysvsem_sem know we have removed
	 * the semaphore to avoid issues with releasing.
	 */

	sem_ptr->count = -1;
	RETURN_TRUE;
}

/* }}} */

#endif /* HAVE_SYSVSEM */
