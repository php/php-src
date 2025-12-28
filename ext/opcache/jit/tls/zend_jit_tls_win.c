/*
 *  +----------------------------------------------------------------------+
 *  | Zend JIT                                                             |
 *  +----------------------------------------------------------------------+
 *  | Copyright (c) The PHP Group                                          |
 *  +----------------------------------------------------------------------+
 *  | This source file is subject to version 3.01 of the PHP license,      |
 *  | that is bundled with this package in the file LICENSE, and is        |
 *  | available through the world-wide-web at the following url:           |
 *  | https://www.php.net/license/3_01.txt                                 |
 *  | If you did not receive a copy of the PHP license and are unable to   |
 *  | obtain it through the world-wide-web, please send a note to          |
 *  | license@php.net so we can mail you a copy immediately.               |
 *  +----------------------------------------------------------------------+
 *  | Authors: Dmitry Stogov <dmitry@php.net>                              |
 *  +----------------------------------------------------------------------+
 */

#include "Zend/zend_portability.h"
#include "Zend/zend_types.h"
#include "TSRM/TSRM.h"
#include "zend_accelerator_debug.h"

#include <stdint.h>
#include <stddef.h>

TSRMLS_CACHE_EXTERN();

extern uint32_t _tls_index;
extern char *_tls_start;
extern char *_tls_end;

zend_result zend_jit_resolve_tsrm_ls_cache_offsets(
	size_t *tcb_offset,
	size_t *module_index,
	size_t *module_offset
) {
	/* To find offset of "_tsrm_ls_cache" in TLS segment we perform a linear scan of local TLS memory */
	/* Probably, it might be better solution */
#ifdef _WIN64
	void ***tls_mem = ((void****)__readgsqword(0x58))[_tls_index];
#else
	void ***tls_mem = ((void****)__readfsdword(0x2c))[_tls_index];
#endif
	void *val = _tsrm_ls_cache;
	size_t offset = 0;
	size_t size = (char*)&_tls_end - (char*)&_tls_start;

	while (offset < size) {
		if (*tls_mem == val) {
			*module_index  = _tls_index * sizeof(void*);
			*module_offset = offset;
			return SUCCESS;
		}
		tls_mem++;
		offset += sizeof(void*);
	}

	if (offset >= size) {
		zend_accel_error_noreturn(ACCEL_LOG_FATAL, "Could not enable JIT: offset >= size");
	}

	return FAILURE;
}
