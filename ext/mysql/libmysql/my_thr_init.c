/* Copyright Abandoned 1996 TCX DataKonsult AB & Monty Program KB & Detron HB 
This file is public domain and comes with NO WARRANTY of any kind */

/*
** Functions to handle initializating and allocationg of all mysys & debug
** thread variables.
*/

#include "mysys_priv.h"
#include <m_string.h>

#ifdef THREAD
#ifdef USE_TLS
pthread_key(struct st_my_thread_var*, THR_KEY_mysys);
#else
pthread_key(struct st_my_thread_var, THR_KEY_mysys);
#endif /* USE_TLS */
pthread_mutex_t THR_LOCK_malloc,THR_LOCK_open,THR_LOCK_keycache,
	        THR_LOCK_lock,THR_LOCK_isam,THR_LOCK_myisam,THR_LOCK_heap,
	        THR_LOCK_net, THR_LOCK_charset; 
#ifndef HAVE_LOCALTIME_R
pthread_mutex_t LOCK_localtime_r;
#endif
#ifdef PTHREAD_ADAPTIVE_MUTEX_INITIALIZER_NP
pthread_mutexattr_t my_fast_mutexattr;
#endif
#ifdef PTHREAD_ERRORCHECK_MUTEX_INITIALIZER_NP
pthread_mutexattr_t my_errchk_mutexattr;
#endif

/* FIXME  Note.  TlsAlloc does not set an auto destructor, so
	the function my_thread_global_free must be called from
	somewhere before final exit of the library */

my_bool my_thread_global_init(void)
{
  if (pthread_key_create(&THR_KEY_mysys,free))
  {
    fprintf(stderr,"Can't initialize threads: error %d\n",errno);
    exit(1);
  }
#ifdef PTHREAD_ADAPTIVE_MUTEX_INITIALIZER_NP
  pthread_mutexattr_init(&my_fast_mutexattr);
  pthread_mutexattr_setkind_np(&my_fast_mutexattr,PTHREAD_MUTEX_ADAPTIVE_NP);
#endif
#ifdef PPTHREAD_ERRORCHECK_MUTEX_INITIALIZER_NP
  pthread_mutexattr_init(&my_errchk_mutexattr);
  pthread_mutexattr_setkind_np(&my_errchk_mutexattr,
			       PTHREAD_MUTEX_ERRORCHECK_NP);
#endif

  pthread_mutex_init(&THR_LOCK_malloc,MY_MUTEX_INIT_FAST);
  pthread_mutex_init(&THR_LOCK_open,MY_MUTEX_INIT_FAST);
  pthread_mutex_init(&THR_LOCK_keycache,MY_MUTEX_INIT_FAST);
  pthread_mutex_init(&THR_LOCK_lock,MY_MUTEX_INIT_FAST);
  pthread_mutex_init(&THR_LOCK_isam,MY_MUTEX_INIT_SLOW);
  pthread_mutex_init(&THR_LOCK_myisam,MY_MUTEX_INIT_SLOW);
  pthread_mutex_init(&THR_LOCK_heap,MY_MUTEX_INIT_FAST);
  pthread_mutex_init(&THR_LOCK_net,MY_MUTEX_INIT_FAST);
  pthread_mutex_init(&THR_LOCK_charset,MY_MUTEX_INIT_FAST);
#if defined( __WIN__) || defined(OS2)
  win_pthread_init();
#endif
#ifndef HAVE_LOCALTIME_R
  pthread_mutex_init(&LOCK_localtime_r,MY_MUTEX_INIT_SLOW);
#endif
  return my_thread_init();
}

void my_thread_global_end(void)
{
#if defined(USE_TLS)
  (void) TlsFree(THR_KEY_mysys);
#endif
#ifdef PTHREAD_ADAPTIVE_MUTEX_INITIALIZER_NP
  pthread_mutexattr_destroy(&my_fast_mutexattr);
#endif
#ifdef PPTHREAD_ERRORCHECK_MUTEX_INITIALIZER_NP
  pthread_mutexattr_destroy(&my_errchk_mutexattr);
#endif
}

static long thread_id=0;

/*
  We can't use mutex_locks here if we are using windows as
  we may have compiled the program with SAFE_MUTEX, in which
  case the checking of mutex_locks will not work until
  the pthread_self thread specific variable is initialized.
*/

my_bool my_thread_init(void)
{
  struct st_my_thread_var *tmp;
#if !defined(__WIN__) || defined(USE_TLS) || ! defined(SAFE_MUTEX)
  pthread_mutex_lock(&THR_LOCK_lock);
#endif
#if !defined(__WIN__) || defined(USE_TLS)
  if (my_pthread_getspecific(struct st_my_thread_var *,THR_KEY_mysys))
  {
    pthread_mutex_unlock(&THR_LOCK_lock);
    return 0;						/* Safequard */
  }
    /* We must have many calloc() here because these are freed on
       pthread_exit */
  if (!(tmp=(struct st_my_thread_var *)
	calloc(1,sizeof(struct st_my_thread_var))))
  {
    pthread_mutex_unlock(&THR_LOCK_lock);
    return 1;
  }
  pthread_setspecific(THR_KEY_mysys,tmp);

#else
  if (THR_KEY_mysys.id)   /* Already initialized */
  {
#if !defined(__WIN__) || defined(USE_TLS) || ! defined(SAFE_MUTEX)
    pthread_mutex_unlock(&THR_LOCK_lock);
#endif
    return 0;
  }
  tmp= &THR_KEY_mysys;
#endif
  tmp->id= ++thread_id;
  pthread_mutex_init(&tmp->mutex,MY_MUTEX_INIT_FAST);
  pthread_cond_init(&tmp->suspend, NULL);
#if !defined(__WIN__) || defined(USE_TLS) || ! defined(SAFE_MUTEX)
  pthread_mutex_unlock(&THR_LOCK_lock);
#endif
  return 0;
}

void my_thread_end(void)
{
  struct st_my_thread_var *tmp=my_thread_var;
  if (tmp)
  {
#if !defined(DBUG_OFF)
    if (tmp->dbug)
    {
      free(tmp->dbug);
      tmp->dbug=0;
    }
#endif
#if !defined(__bsdi__) || defined(HAVE_mit_thread) /* bsdi dumps core here */
    pthread_cond_destroy(&tmp->suspend);
#endif
    pthread_mutex_destroy(&tmp->mutex);
#if (!defined(__WIN__) && !defined(OS2)) || defined(USE_TLS)
    free(tmp);
#endif
  }
#if (!defined(__WIN__) && !defined(OS2)) || defined(USE_TLS)
  pthread_setspecific(THR_KEY_mysys,0);
#endif
}

struct st_my_thread_var *_my_thread_var(void)
{
  struct st_my_thread_var *tmp=
    my_pthread_getspecific(struct st_my_thread_var*,THR_KEY_mysys);
#if defined(USE_TLS)
  /* This can only happen in a .DLL */
  if (!tmp)
  {
    my_thread_init();
    tmp=my_pthread_getspecific(struct st_my_thread_var*,THR_KEY_mysys);
  }
#endif
  return tmp;
}

/****************************************************************************
** Get name of current thread.
****************************************************************************/

#define UNKNOWN_THREAD -1

long my_thread_id()
{
#if defined(HAVE_PTHREAD_GETSEQUENCE_NP)
  return pthread_getsequence_np(pthread_self());
#elif (defined(__sun) || defined(__sgi) || defined(__linux__)) && !defined(HAVE_mit_thread)
  return pthread_self();
#else
  return my_thread_var->id;
#endif
}

#ifdef DBUG_OFF
const char *my_thread_name(void)
{
  return "no_name";
}

#else

const char *my_thread_name(void)
{
  char name_buff[100];
  struct st_my_thread_var *tmp=my_thread_var;
  if (!tmp->name[0])
  {
    long id=my_thread_id();
    sprintf(name_buff,"T@%ld", id);
    strmake(tmp->name,name_buff,THREAD_NAME_SIZE);
  }
  return tmp->name;
}
#endif /* DBUG_OFF */

#endif /* THREAD */
