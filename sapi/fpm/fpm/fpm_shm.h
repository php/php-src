	/* (c) 2007,2008 Andrei Nigmatulin */

#ifndef FPM_SHM_H
#define FPM_SHM_H 1

void *fpm_shm_alloc(size_t size);
int fpm_shm_free(void *mem, size_t size);
size_t fpm_shm_get_size_allocated();

#endif
