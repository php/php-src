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

/* This makes a wrapper for mutex handling to make it easier to debug mutex */

#include <global.h>
#if defined(HAVE_LINUXTHREADS) && !defined (__USE_UNIX98)
#define __USE_UNIX98			/* To get rw locks under Linux */
#endif
#include <m_string.h>
#if defined(THREAD) && defined(SAFE_MUTEX)
#undef SAFE_MUTEX			/* Avoid safe_mutex redefinitions */
#include <my_pthread.h>

#ifndef DO_NOT_REMOVE_THREAD_WRAPPERS
/* Remove wrappers */
#undef pthread_mutex_init
#undef pthread_mutex_lock
#undef pthread_mutex_unlock
#undef pthread_mutex_destroy
#undef pthread_cond_wait
#undef pthread_cond_timedwait
#ifdef HAVE_NONPOSIX_PTHREAD_MUTEX_INIT
#define pthread_mutex_init(a,b) my_pthread_mutex_init((a),(b))
#endif
#endif /* DO_NOT_REMOVE_THREAD_WRAPPERS */

int safe_mutex_init(safe_mutex_t *mp,
		    const pthread_mutexattr_t *attr __attribute__((unused)))
{
  bzero((char*) mp,sizeof(*mp));
#ifdef HAVE_LINUXTHREADS			/* Some extra safety */
  {
    pthread_mutexattr_t tmp;
    pthread_mutexattr_init(&tmp);
    pthread_mutexattr_setkind_np(&tmp,PTHREAD_MUTEX_ERRORCHECK_NP);
    pthread_mutex_init(&mp->global,&tmp);
    pthread_mutex_init(&mp->mutex, &tmp);
    pthread_mutexattr_destroy(&tmp);
  }
#else
  pthread_mutex_init(&mp->global,NULL);
  pthread_mutex_init(&mp->mutex,attr);
#endif
  return 0;
}

int safe_mutex_lock(safe_mutex_t *mp,const char *file, uint line)
{
  int error;
  pthread_mutex_lock(&mp->global);
  if (mp->count > 0 && pthread_equal(pthread_self(),mp->thread))
  {
    fprintf(stderr,"safe_mutex: Trying to lock mutex at %s, line %d, when the mutex was already locked at %s, line %d\n",
	    file,line,mp->file,mp->line);
    abort();
  }
  pthread_mutex_unlock(&mp->global);
  error=pthread_mutex_lock(&mp->mutex);
  if (error || (error=pthread_mutex_lock(&mp->global)))
  {
    fprintf(stderr,"Got error %d when trying to lock mutex at %s, line %d\n",
	    error, file, line);
    abort();
  }
  if (mp->count++)
  {
    fprintf(stderr,"safe_mutex: Error in thread libray: Got mutex at %s, line %d more than 1 time\n", file,line);
    abort();
  }
  mp->thread=pthread_self();
  mp->file= (char*) file;
  mp->line=line;
  pthread_mutex_unlock(&mp->global);
  return error;
}


int safe_mutex_unlock(safe_mutex_t *mp,const char *file, uint line)
{
  int error;
  pthread_mutex_lock(&mp->global);
  if (mp->count == 0)
  {
    fprintf(stderr,"safe_mutex: Trying to unlock mutex that wasn't locked at %s, line %d\n            Last used at %s, line: %d\n",
	    file,line,mp->file ? mp->file : "",mp->line);
    abort();
  }
  if (!pthread_equal(pthread_self(),mp->thread))
  {
    fprintf(stderr,"safe_mutex: Trying to unlock mutex at %s, line %d  that was locked by another thread at: %s, line: %d\n",
	    file,line,mp->file,mp->line);
    abort();
  }
  mp->count--;
  error=pthread_mutex_unlock(&mp->mutex);
  if (error)
  {
    fprintf(stderr,"safe_mutex: Got error: %d when trying to unlock mutex at %s, line %d\n", error, file, line);
    abort();
  }
  pthread_mutex_unlock(&mp->global);
  return error;
}


int safe_cond_wait(pthread_cond_t *cond, safe_mutex_t *mp, const char *file,
		   uint line)
{
  int error;
  pthread_mutex_lock(&mp->global);
  if (mp->count == 0)
  {
    fprintf(stderr,"safe_mutex: Trying to cond_wait on a unlocked mutex at %s, line %d\n",file,line);
    abort();
  }
  if (!pthread_equal(pthread_self(),mp->thread))
  {
    fprintf(stderr,"safe_mutex: Trying to cond_wait on a mutex at %s, line %d  that was locked by another thread at: %s, line: %d\n",
	    file,line,mp->file,mp->line);
    abort();
  }

  if (mp->count-- != 1)
  {
    fprintf(stderr,"safe_mutex:  Count was %d on locked mutex at %s, line %d\n",
	    mp->count+1, file, line);
    abort();
  }
  pthread_mutex_unlock(&mp->global);
  error=pthread_cond_wait(cond,&mp->mutex);
  pthread_mutex_lock(&mp->global);
  if (error)
  {
    fprintf(stderr,"safe_mutex: Got error: %d when doing a safe_mutex_wait at %s, line %d\n", error, file, line);
    abort();
  }
  if (mp->count++)
  {
    fprintf(stderr,
	    "safe_mutex:  Count was %d in thread %lx when locking mutex at %s, line %d\n",
	    mp->count-1, my_thread_id(), file, line);
    abort();
  }
  mp->thread=pthread_self();
  mp->file= (char*) file;
  mp->line=line;
  pthread_mutex_unlock(&mp->global);
  return error;
}


int safe_cond_timedwait(pthread_cond_t *cond, safe_mutex_t *mp,
			struct timespec *abstime,
			const char *file, uint line)
{
  int error;
  pthread_mutex_lock(&mp->global);
  if (mp->count != 1 || !pthread_equal(pthread_self(),mp->thread))
  {
    fprintf(stderr,"safe_mutex: Trying to cond_wait at %s, line %d on a not hold mutex\n",file,line);
    abort();
  }
  mp->count--;					/* Mutex will be released */
  pthread_mutex_unlock(&mp->global);
  error=pthread_cond_timedwait(cond,&mp->mutex,abstime);
#ifdef EXTRA_DEBUG
  if (error && (error != EINTR && error != ETIMEDOUT))
  {
    fprintf(stderr,"safe_mutex: Got error: %d when doing a safe_mutex_timedwait at %s, line %d\n", error, file, line);
  }
#endif
  pthread_mutex_lock(&mp->global);
  if (mp->count++)
  {
    fprintf(stderr,
	    "safe_mutex:  Count was %d in thread %lx when locking mutex at %s, line %d (error: %d)\n",
	    mp->count-1, my_thread_id(), file, line, error);
    abort();
  }
  mp->thread=pthread_self();
  mp->file= (char*) file;
  mp->line=line;
  pthread_mutex_unlock(&mp->global);
  return error;
}

int safe_mutex_destroy(safe_mutex_t *mp, const char *file, uint line)
{
  if (mp->count != 0)
  {
    fprintf(stderr,"safe_mutex: Trying to destroy a mutex that was locked at %s, line %d at %s, line %d\n",
	    mp->file,mp->line, file, line);
    abort();
  }
  pthread_mutex_destroy(&mp->global);
  return pthread_mutex_destroy(&mp->mutex);
}

#endif /* THREAD && SAFE_MUTEX */
