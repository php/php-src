/* Copyright Abandoned 1996 TCX DataKonsult AB & Monty Program KB & Detron HB 
This file is public domain and comes with NO WARRANTY of any kind */

#include <global.h>
#include <my_sys.h>

#ifdef USE_SYSTEM_WRAPPERS
#include "system_wrappers.h"
#endif

#ifdef THREAD
extern pthread_mutex_t THR_LOCK_malloc,THR_LOCK_open,THR_LOCK_keycache,
  THR_LOCK_lock,THR_LOCK_isam,THR_LOCK_net,THR_LOCK_charset;
extern pthread_mutex_t LOCK_bitmap;
#else /* THREAD */
#define pthread_mutex_lock(A)
#define pthread_mutex_unlock(A)
#endif
