/* Copyright (C) 2000 MySQL AB & MySQL Finland AB & TCX DataKonsult AB
   
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
   
   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.
   
   You should have received a copy of the GNU Library General Public
   License along with this library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
   MA 02111-1307, USA */

#include <global.h>
#include <my_sys.h>

#ifdef USE_SYSTEM_WRAPPERS
#include "system_wrappers.h"
#endif

#ifdef THREAD
extern pthread_mutex_t THR_LOCK_malloc,THR_LOCK_open,THR_LOCK_keycache,
  THR_LOCK_lock,THR_LOCK_isam,THR_LOCK_net,THR_LOCK_charset;
#else /* THREAD */
#define pthread_mutex_lock(A)
#define pthread_mutex_unlock(A)
#endif
