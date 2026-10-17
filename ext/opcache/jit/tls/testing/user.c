
/* _tsrm_ls_cache is used / inspected here */

#include "../zend_jit_tls.h"

int test(void)
{
	size_t tcb_offset = 0;
	size_t module_index = -1;
	size_t module_offset = -1;

	/* Ensure the slot is allocated */
	void *cache = &_tsrm_ls_cache;

	zend_result result = zend_jit_resolve_tsrm_ls_cache_offsets(
			&tcb_offset, &module_index, &module_offset);

	printf("tcb_offset: %zd; module_index: %zd; module_offset: %zd\n",
			tcb_offset, module_index, module_offset);

	if (result != SUCCESS) {
		return 0;
	}

	return zend_jit_tsrm_ls_cache_address(tcb_offset, module_index, module_offset) == cache;
}
