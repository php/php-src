#include "phpdbg_sigsafe.h"
#include "phpdbg.h"

ZEND_EXTERN_MODULE_GLOBALS(phpdbg);

static inline void zend_mm_init(zend_mm_heap *heap) {
	zend_mm_free_block *p;
	int i;

	heap->free_bitmap = 0;
	heap->large_free_bitmap = 0;
#if ZEND_MM_CACHE
	heap->cached = 0;
	memset(heap->cache, 0, sizeof(heap->cache));
#endif
#if ZEND_MM_CACHE_STAT
	for (i = 0; i < ZEND_MM_NUM_BUCKETS; i++) {
		heap->cache_stat[i].count = 0;
	}
#endif
	p = ZEND_MM_SMALL_FREE_BUCKET(heap, 0);
	for (i = 0; i < ZEND_MM_NUM_BUCKETS; i++) {
		p->next_free_block = p;
		p->prev_free_block = p;
		p = (zend_mm_free_block *)((char *)p + sizeof(zend_mm_free_block *) * 2);
		heap->large_free_buckets[i] = NULL;
	}
	heap->rest_buckets[0] = heap->rest_buckets[1] = ZEND_MM_REST_BUCKET(heap);
	heap->rest_count = 0;
}

static zend_mm_storage* zend_mm_mem_init(void *params) {
	TSRMLS_FETCH();

	return &PHPDBG_G(sigsafe_mem).storage;
}

static void zend_mm_mem_dummy(zend_mm_storage *storage) {
}

#define STR(x) #x
#define EXP_STR(x) STR(x)

static zend_mm_segment* zend_mm_mem_alloc(zend_mm_storage *storage, size_t size) {
	TSRMLS_FETCH();

	if (EXPECTED(size == PHPDBG_SIGSAFE_MEM_SIZE && !PHPDBG_G(sigsafe_mem).allocated)) {
		PHPDBG_G(sigsafe_mem).allocated = 1;
		return (zend_mm_segment *) PHPDBG_G(sigsafe_mem).mem;
	}

	write(PHPDBG_G(io)[PHPDBG_STDERR].fd, ZEND_STRL("Tried to allocate more than " EXP_STR(PHPDBG_SIGSAFE_MEM_SIZE) " bytes from stack memory in signal handler ... bailing out of signal handler\n"));

	if (*EG(bailout)) {
		LONGJMP(*EG(bailout), FAILURE);
	}

	write(PHPDBG_G(io)[PHPDBG_STDERR].fd, ZEND_STRL("Bailed out without a bailout address in signal handler!\n"));

	return NULL;
}

static zend_mm_segment* zend_mm_mem_realloc(zend_mm_storage *storage, zend_mm_segment *ptr, size_t size) {
	return zend_mm_mem_alloc(storage, size);
}

static void zend_mm_mem_free(zend_mm_storage *storage, zend_mm_segment *ptr) {
}

static const zend_mm_mem_handlers phpdbg_handlers_sigsafe_mem = { "stack", zend_mm_mem_init, zend_mm_mem_dummy, zend_mm_mem_dummy, zend_mm_mem_alloc, zend_mm_mem_realloc, zend_mm_mem_free };

void phpdbg_set_sigsafe_mem(char *buffer TSRMLS_DC) {
	phpdbg_signal_safe_mem *mem = &PHPDBG_G(sigsafe_mem);
	mem->old_heap = zend_mm_set_heap(&mem->heap TSRMLS_CC);
	mem->mem = buffer;
	mem->allocated = 0;

	mem->storage.handlers = &phpdbg_handlers_sigsafe_mem;
	mem->heap.storage = &mem->storage;
	mem->heap.block_size = PHPDBG_SIGSAFE_MEM_SIZE;
	mem->heap.compact_size = 0;
	mem->heap.segments_list = NULL;
	zend_mm_init(&mem->heap);
#if ZEND_MM_CACHE_STAT
	memset(mem->heap.cache_stat, 0, sizeof(mem->heap.cache_stat));
#endif
	mem->heap.use_zend_alloc = 1;
	mem->heap.real_size = 0;
	mem->heap.overflow = 0;
	mem->heap.real_peak = 0;
	mem->heap.limit = PHPDBG_SIGSAFE_MEM_SIZE;
	mem->heap.size = 0;
	mem->heap.peak = 0;
	mem->heap.internal = 0;
	mem->heap.reserve = NULL;
	mem->heap.reserve_size = 0;
}

zend_mm_heap *phpdbg_original_heap_sigsafe_mem(TSRMLS_D) {
	return PHPDBG_G(sigsafe_mem).old_heap;
}

void phpdbg_clear_sigsafe_mem(TSRMLS_D) {
	zend_mm_set_heap(phpdbg_original_heap_sigsafe_mem(TSRMLS_C) TSRMLS_CC);
	PHPDBG_G(sigsafe_mem).mem = NULL;
}

zend_bool phpdbg_active_sigsafe_mem(TSRMLS_D) {
	return !!PHPDBG_G(sigsafe_mem).mem;
}

