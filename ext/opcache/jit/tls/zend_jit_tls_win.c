/*
   +----------------------------------------------------------------------+
   | Zend JIT                                                             |
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Authors: Dmitry Stogov <dmitry@php.net>                              |
   +----------------------------------------------------------------------+
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
	/* Offset of _tsrm_ls_cache within this module's TLS block. */
#ifdef _WIN64
	void ***tls_mem = ((void****)__readgsqword(0x58))[_tls_index];
#else
	void ***tls_mem = ((void****)__readfsdword(0x2c))[_tls_index];
#endif
	size_t size = (char*)&_tls_end - (char*)&_tls_start;
	size_t offset = (size_t)((char*)&_tsrm_ls_cache - (char*)tls_mem);

	if (offset >= size) {
		zend_accel_error_noreturn(ACCEL_LOG_FATAL, "Could not enable JIT: offset >= size");
		return FAILURE;
	}

	*module_index  = _tls_index * sizeof(void*);
	*module_offset = offset;
	return SUCCESS;
}
