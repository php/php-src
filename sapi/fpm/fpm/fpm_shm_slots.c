
	/* $Id: fpm_shm_slots.c,v 1.2 2008/05/24 17:38:47 anight Exp $ */
	/* (c) 2007,2008 Andrei Nigmatulin */

#include "fpm_config.h"

#include "fpm_atomic.h"
#include "fpm_worker_pool.h"
#include "fpm_children.h"
#include "fpm_shm.h"
#include "fpm_shm_slots.h"
#include "zlog.h"

static void *shm_mem;
static struct fpm_shm_slot_s *shm_slot;

int fpm_shm_slots_prepare_slot(struct fpm_child_s *child) /* {{{ */
{
	struct fpm_worker_pool_s *wp = child->wp;
	struct fpm_shm_slot_ptr_s *shm_slot_ptr;

	child->shm_slot_i = wp->slots_used.used;
	shm_slot_ptr = fpm_array_push(&wp->slots_used);

	if (0 == shm_slot_ptr) {
		return -1;
	}

	if (0 == wp->slots_free.used) {
		shm_slot_ptr->shm_slot = fpm_shm_alloc_chunk(&wp->shm_list, sizeof(struct fpm_shm_slot_s), &shm_slot_ptr->mem);
		if (!shm_slot_ptr->shm_slot) {
			return -1;
		}
	} else {
		*shm_slot_ptr = *(struct fpm_shm_slot_ptr_s *) fpm_array_item_last(&wp->slots_free);
		--wp->slots_free.used;
	}

	memset(shm_slot_ptr->shm_slot, 0, sizeof(struct fpm_shm_slot_s));
	shm_slot_ptr->child = child;
	return 0;
}
/* }}} */

void fpm_shm_slots_discard_slot(struct fpm_child_s *child) /* {{{ */
{
	struct fpm_shm_slot_ptr_s *shm_slot_ptr;
	struct fpm_worker_pool_s *wp = child->wp;
	int n;

	shm_slot_ptr = fpm_array_push(&wp->slots_free);
	if (shm_slot_ptr) {
		struct fpm_shm_slot_ptr_s *shm_slot_ptr_used;

		shm_slot_ptr_used = fpm_array_item(&wp->slots_used, child->shm_slot_i);
		*shm_slot_ptr = *shm_slot_ptr_used;
		shm_slot_ptr->child = 0;
	}

	n = fpm_array_item_remove(&wp->slots_used, child->shm_slot_i);
	if (n > -1) {
		shm_slot_ptr = fpm_array_item(&wp->slots_used, n);
		shm_slot_ptr->child->shm_slot_i = n;
	}
}
/* }}} */

void fpm_shm_slots_child_use_slot(struct fpm_child_s *child) /* {{{ */
{
	struct fpm_shm_slot_ptr_s *shm_slot_ptr;
	struct fpm_worker_pool_s *wp = child->wp;

	shm_slot_ptr = fpm_array_item(&wp->slots_used, child->shm_slot_i);
	shm_slot = shm_slot_ptr->shm_slot;
	shm_mem = shm_slot_ptr->mem;
}
/* }}} */

void fpm_shm_slots_parent_use_slot(struct fpm_child_s *child) /* {{{ */
{
	/* nothing to do */
}
/* }}} */

void *fpm_shm_slots_mem() /* {{{ */
{
	return shm_mem;
}
/* }}} */

struct fpm_shm_slot_s *fpm_shm_slot(struct fpm_child_s *child) /* {{{ */
{
	struct fpm_shm_slot_ptr_s *shm_slot_ptr;
	struct fpm_worker_pool_s *wp = child->wp;

	shm_slot_ptr = fpm_array_item(&wp->slots_used, child->shm_slot_i);
	return shm_slot_ptr->shm_slot;
}
/* }}} */

struct fpm_shm_slot_s *fpm_shm_slots_acquire(struct fpm_shm_slot_s *s, int nohang) /* {{{ */
{
	if (s == 0) {
		s = shm_slot;
	}

	if (0 > fpm_spinlock(&s->lock, nohang)) {
		return 0;
	}
	return s;
}
/* }}} */

void fpm_shm_slots_release(struct fpm_shm_slot_s *s) /* {{{ */
{
	s->lock = 0;
}
/* }}} */

