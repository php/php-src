
	/* $Id: fpm_shm_slots.h,v 1.2 2008/05/24 17:38:47 anight Exp $ */
	/* (c) 2007,2008 Andrei Nigmatulin */

#ifndef FPM_SHM_SLOTS_H
#define FPM_SHM_SLOTS_H 1

#include "fpm_atomic.h"
#include "fpm_worker_pool.h"
#include "fpm_request.h"

struct fpm_child_s;

struct fpm_shm_slot_s {
	union {
		atomic_t lock;
		char dummy[16];
	};
	enum fpm_request_stage_e request_stage;
	struct timeval accepted;
	struct timeval tv;
	char request_method[16];
	size_t content_length; /* used with POST only */
	char script_filename[256];
};

struct fpm_shm_slot_ptr_s {
	void *mem;
	struct fpm_shm_slot_s *shm_slot;
	struct fpm_child_s *child;
};

int fpm_shm_slots_prepare_slot(struct fpm_child_s *child);
void fpm_shm_slots_discard_slot(struct fpm_child_s *child);
void fpm_shm_slots_child_use_slot(struct fpm_child_s *child);
void fpm_shm_slots_parent_use_slot(struct fpm_child_s *child);
void *fpm_shm_slots_mem();
struct fpm_shm_slot_s *fpm_shm_slot(struct fpm_child_s *child);
struct fpm_shm_slot_s *fpm_shm_slots_acquire(struct fpm_shm_slot_s *, int nohang);
void fpm_shm_slots_release(struct fpm_shm_slot_s *);

#endif

