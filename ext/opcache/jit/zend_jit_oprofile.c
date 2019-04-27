/*
   +----------------------------------------------------------------------+
   | Zend JIT                                                             |
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
   | Authors: Dmitry Stogov <dmitry@php.net>                              |
   +----------------------------------------------------------------------+
*/

#define HAVE_OPROFILE 1

#include <opagent.h>

static op_agent_t op_agent = NULL;

static void zend_jit_oprofile_register(const char *name,
                                       const void *start,
                                       size_t      size)
{
	if (op_agent) {
		op_write_native_code(op_agent, name, (uint64_t)(zend_uintptr_t)start, start, size);
	}
}

static int zend_jit_oprofile_startup(void)
{
	op_agent = op_open_agent();
	if (!op_agent) {
		fprintf(stderr, "OpAgent initialization failed [%d]!\n", errno);
		return 0;
	}
	return 1;
}

static void zend_jit_oprofile_shutdown(void)
{
	if (op_agent) {
//???		sleep(60);
		op_close_agent(op_agent);
	}
}
