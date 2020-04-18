	/* (c) 2007,2008 Andrei Nigmatulin */

#ifndef FPM_ARRAYS_H
#define FPM_ARRAYS_H 1

#include <stdlib.h>
#include <string.h>

struct fpm_array_s {
	void *data;
	size_t sz;
	size_t used;
	size_t allocated;
};

static inline struct fpm_array_s *fpm_array_init(struct fpm_array_s *a, unsigned int sz, unsigned int initial_num) /* {{{ */
{
	void *allocated = 0;

	if (!a) {
		a = malloc(sizeof(struct fpm_array_s));

		if (!a) {
			return 0;
		}

		allocated = a;
	}

	a->sz = sz;

	a->data = calloc(sz, initial_num);

	if (!a->data) {
		free(allocated);
		return 0;
	}

	a->allocated = initial_num;
	a->used = 0;

	return a;
}
/* }}} */

static inline void *fpm_array_item(struct fpm_array_s *a, unsigned int n) /* {{{ */
{
	char *ret;

	ret = (char *) a->data + a->sz * n;

	return ret;
}
/* }}} */

static inline void *fpm_array_item_last(struct fpm_array_s *a) /* {{{ */
{
	return fpm_array_item(a, a->used - 1);
}
/* }}} */

static inline int fpm_array_item_remove(struct fpm_array_s *a, unsigned int n) /* {{{ */
{
	int ret = -1;

	if (n < a->used - 1) {
		void *last = fpm_array_item(a, a->used - 1);
		void *to_remove = fpm_array_item(a, n);

		memcpy(to_remove, last, a->sz);

		ret = n;
	}

	--a->used;

	return ret;
}
/* }}} */

static inline void *fpm_array_push(struct fpm_array_s *a) /* {{{ */
{
	void *ret;

	if (a->used == a->allocated) {
		size_t new_allocated = a->allocated ? a->allocated * 2 : 20;
		void *new_ptr = realloc(a->data, a->sz * new_allocated);

		if (!new_ptr) {
			return 0;
		}

		a->data = new_ptr;
		a->allocated = new_allocated;
	}

	ret = fpm_array_item(a, a->used);

	++a->used;

	return ret;
}
/* }}} */

static inline void fpm_array_free(struct fpm_array_s *a) /* {{{ */
{
	free(a->data);
	a->data = 0;
	a->sz = 0;
	a->used = a->allocated = 0;
}
/* }}} */

#endif
