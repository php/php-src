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

/* Functions to get threads more portable */

#include "mysys_priv.h"
#ifdef THREAD
#include <signal.h>
#include <m_string.h>
#include <thr_alarm.h>
#include <assert.h>
#if !defined(MSDOS) && !defined(__WIN__)
#include <netdb.h>
#endif

#if (defined(__BSD__) || defined(_BSDI_VERSION)) && !defined(HAVE_mit_thread)
#define SCHED_POLICY SCHED_RR
#else
#define SCHED_POLICY SCHED_OTHER
#endif

#ifndef my_pthread_setprio
void my_pthread_setprio(pthread_t thread_id,int prior)
{
#ifdef HAVE_PTHREAD_SETSCHEDPARAM
  struct sched_param tmp_sched_param;
  bzero((char*) &tmp_sched_param,sizeof(tmp_sched_param));
  tmp_sched_param.sched_priority=prior;
  VOID(pthread_setschedparam(thread_id,SCHED_POLICY,&tmp_sched_param));
#endif
}
#endif

#ifndef my_pthread_getprio
int my_pthread_getprio(pthread_t thread_id)
{
#ifdef HAVE_PTHREAD_SETSCHEDPARAM
  struct sched_param tmp_sched_param;
  int policy;
  if (!pthread_getschedparam(thread_id,&policy,&tmp_sched_param))
  {
    DBUG_PRINT("thread",("policy: %d  priority: %d",
			 policy,tmp_sched_param.sched_priority));
    return tmp_sched_param.sched_priority;
  }
#endif
  return -1;
}
#endif

#ifndef my_pthread_attr_setprio
void my_pthread_attr_setprio(pthread_attr_t *attr, int priority)
{
#ifdef HAVE_PTHREAD_SETSCHEDPARAM
  struct sched_param tmp_sched_param;
  bzero((char*) &tmp_sched_param,sizeof(tmp_sched_param));
  tmp_sched_param.sched_priority=priority;
  VOID(pthread_attr_setschedparam(attr,&tmp_sched_param));
#endif
}
#endif


/* To allow use of pthread_getspecific with two arguments */

#ifdef HAVE_NONPOSIX_PTHREAD_GETSPECIFIC
#undef pthread_getspecific
#ifdef HAVE_UNIXWARE7_THREADS
#define pthread_getspecific thr_getspecific
#endif

void *my_pthread_getspecific_imp(pthread_key_t key)
{
  void *value;
  if (pthread_getspecific(key,(void *) &value))
    return 0;
  return value;
}
#endif


/* Some functions for RTS threads, AIX, Siemens Unix and UnixWare 7
   (and DEC OSF/1 3.2 too) */

int my_pthread_create_detached=1;

#if defined(HAVE_NONPOSIX_SIGWAIT) || defined(HAVE_DEC_3_2_THREADS)

int my_sigwait(const sigset_t *set,int *sig)
{
  int signal=sigwait(set);
  if (signal < 0)
    return errno;
  *sig=signal;
  return 0;
}
#endif

/* localtime_r for SCO 3.2V4.2 */

#ifndef HAVE_LOCALTIME_R

extern pthread_mutex_t LOCK_localtime_r;

struct tm *localtime_r(const time_t *clock, struct tm *res)
{
  struct tm *tmp;
  pthread_mutex_lock(&LOCK_localtime_r);
  tmp=localtime(clock);
  *res= *tmp;
  pthread_mutex_unlock(&LOCK_localtime_r);
  return res;
}
#endif


/****************************************************************************
** Replacement of sigwait if the system doesn't have one (like BSDI 3.0)
**
** Note:
** This version of sigwait() is assumed to called in a loop so the signalmask
** is permanently modified to reflect the signal set. This is done to get
** a much faster implementation.
**
** This implementation isn't thread safe: It assumes that only one
** thread is using sigwait.
**
** If one later supplies a different signal mask, all old signals that
** was used before are unblocked and set to SIGDFL.
**
** Author: Gary Wisniewski <garyw@spidereye.com.au>, much modified by Monty
****************************************************************************/

#if !defined(HAVE_SIGWAIT) && !defined(HAVE_mit_thread) && !defined(sigwait) && !defined(__WIN__) && !defined(HAVE_rts_threads) && !defined(HAVE_NONPOSIX_SIGWAIT) && !defined(HAVE_DEC_3_2_THREADS)

#if !defined(DONT_USE_SIGSUSPEND)

static sigset_t sigwait_set,rev_sigwait_set,px_recd;

void px_handle_sig(int sig)
{
  sigaddset(&px_recd, sig);
}


void sigwait_setup(sigset_t *set)
{
  int i;
  struct sigaction sact,sact1;
  sigset_t unblock_mask;

  sact.sa_flags = 0;
  sact.sa_handler = px_handle_sig;
  memcpy_fixed(&sact.sa_mask,set,sizeof(*set));	/* handler isn't thread_safe */
  sigemptyset(&unblock_mask);
  pthread_sigmask(SIG_UNBLOCK,(sigset_t*) 0,&rev_sigwait_set);

  for (i = 1; i <= sizeof(sigwait_set)*8; i++)
  {
    if (sigismember(set,i))
    {
      sigdelset(&rev_sigwait_set,i);
      if (!sigismember(&sigwait_set,i))
	sigaction(i, &sact, (struct sigaction*) 0);
    }
    else
    {
      sigdelset(&px_recd,i);			/* Don't handle this */
      if (sigismember(&sigwait_set,i))
      {						/* Remove the old handler */
	sigaddset(&unblock_mask,i);
	sigdelset(&rev_sigwait_set,i);
	sact1.sa_flags = 0;
	sact1.sa_handler = SIG_DFL;
	sigemptyset(&sact1.sa_mask);
	sigaction(i, &sact1, 0);
      }
    }
  }
  memcpy_fixed(&sigwait_set,set,sizeof(*set));
  pthread_sigmask(SIG_BLOCK,(sigset_t*) set,(sigset_t*) 0);
  pthread_sigmask(SIG_UNBLOCK,&unblock_mask,(sigset_t*) 0);
}


int sigwait(sigset_t *setp, int *sigp)
{
  if (memcmp(setp,&sigwait_set,sizeof(sigwait_set)))
    sigwait_setup(setp);			/* Init or change of set */

  for (;;)
  {
    /*
      This is a fast, not 100% portable implementation to find the signal.
      Because the handler is blocked there should be at most 1 bit set, but
      the specification on this is somewhat shady so we use a set instead a
      single variable.
      */

    ulong *ptr= (ulong*) &px_recd;
    ulong *end=ptr+sizeof(px_recd)/sizeof(ulong);

    for ( ; ptr != end ; ptr++)
    {
      if (*ptr)
      {
	ulong set= *ptr;
	int found= (int) ((char*) ptr - (char*) &px_recd)*8+1;
	while (!(set & 1))
	{
	  found++;
	  set>>=1;
	}
	*sigp=found;
	sigdelset(&px_recd,found);
	return 0;
      }
    }
    sigsuspend(&rev_sigwait_set);
  }
  return 0;
}
#else  /* !DONT_USE_SIGSUSPEND */

/****************************************************************************
** Replacement of sigwait if the system doesn't have one (like BSDI 3.0)
**
** Note:
** This version of sigwait() is assumed to called in a loop so the signalmask
** is permanently modified to reflect the signal set. This is done to get
** a much faster implementation.
**
** This implementation uses a extra thread to handle the signals and one
** must always call sigwait() with the same signal mask!
**
** BSDI 3.0 NOTE:
**
** pthread_kill() doesn't work on a thread in a select() or sleep() loop?
** After adding the sleep to sigwait_thread, all signals are checked and
** delivered every second. This isn't that terrible performance vice, but
** someone should report this to BSDI and ask for a fix!
** Another problem is that when the sleep() ends, every select() in other
** threads are interrupted!
****************************************************************************/

static sigset_t pending_set;
static bool inited=0;
static pthread_cond_t  COND_sigwait;
static pthread_mutex_t LOCK_sigwait;


void sigwait_handle_sig(int sig)
{
  pthread_mutex_lock(&LOCK_sigwait);
  sigaddset(&pending_set, sig);
  VOID(pthread_cond_signal(&COND_sigwait)); /* inform sigwait() about signal */
  pthread_mutex_unlock(&LOCK_sigwait);
}

extern pthread_t alarm_thread;

void *sigwait_thread(void *set_arg)
{
  sigset_t *set=(sigset_t*) set_arg;

  int i;
  struct sigaction sact;
  sact.sa_flags = 0;
  sact.sa_handler = sigwait_handle_sig;
  memcpy_fixed(&sact.sa_mask,set,sizeof(*set));	/* handler isn't thread_safe */
  sigemptyset(&pending_set);

  for (i = 1; i <= sizeof(pending_set)*8; i++)
  {
    if (sigismember(set,i))
    {
      sigaction(i, &sact, (struct sigaction*) 0);
    }
  }
  sigaddset(set,THR_CLIENT_ALARM);
  pthread_sigmask(SIG_UNBLOCK,(sigset_t*) set,(sigset_t*) 0);
  alarm_thread=pthread_self();			/* For thr_alarm */

  for (;;)
  {						/* Wait for signals */
#ifdef HAVE_NOT_BROKEN_SELECT
    fd_set fd;
    FD_ZERO(&fd);
    select(0,&fd,0,0,0);
#else
    sleep(1);					/* Because of broken BSDI */
#endif
  }
}


int sigwait(sigset_t *setp, int *sigp)
{
  if (!inited)
  {
    pthread_attr_t thr_attr;
    pthread_t sigwait_thread_id;
    inited=1;
    sigemptyset(&pending_set);
    pthread_mutex_init(&LOCK_sigwait,NULL);
    pthread_cond_init(&COND_sigwait,NULL);

    pthread_attr_init(&thr_attr);
    pthread_attr_setscope(&thr_attr,PTHREAD_SCOPE_PROCESS);
    pthread_attr_setdetachstate(&thr_attr,PTHREAD_CREATE_DETACHED);
    pthread_attr_setstacksize(&thr_attr,8196);
    my_pthread_attr_setprio(&thr_attr,100);	/* Very high priority */
    VOID(pthread_create(&sigwait_thread_id,&thr_attr,sigwait_thread,setp));
    VOID(pthread_attr_destroy(&thr_attr));
  }

  pthread_mutex_lock(&LOCK_sigwait);
  for (;;)
  {
    ulong *ptr= (ulong*) &pending_set;
    ulong *end=ptr+sizeof(pending_set)/sizeof(ulong);

    for ( ; ptr != end ; ptr++)
    {
      if (*ptr)
      {
	ulong set= *ptr;
	int found= (int) ((char*) ptr - (char*) &pending_set)*8+1;
	while (!(set & 1))
	{
	  found++;
	  set>>=1;
	}
	*sigp=found;
	sigdelset(&pending_set,found);
	pthread_mutex_unlock(&LOCK_sigwait);
	return 0;
      }
    }
    VOID(pthread_cond_wait(&COND_sigwait,&LOCK_sigwait));
  }
  return 0;
}

#endif /* DONT_USE_SIGSUSPEND */
#endif /* HAVE_SIGWAIT */

/*****************************************************************************
** Implement pthread_signal for systems that can't use signal() with threads
** Currently this is only used with BSDI 3.0
*****************************************************************************/

#ifdef USE_PTHREAD_SIGNAL

int pthread_signal(int sig, void (*func)())
{
  struct sigaction sact;
  sact.sa_flags= 0;
  sact.sa_handler= func;
  sigemptyset(&sact.sa_mask);
  sigaction(sig, &sact, (struct sigaction*) 0);
  return 0;
}

#endif

/*****************************************************************************
** Patches for AIX and DEC OSF/1 3.2
*****************************************************************************/

#if (defined(HAVE_NONPOSIX_PTHREAD_MUTEX_INIT) && !defined(HAVE_UNIXWARE7_THREADS)) || defined(HAVE_DEC_3_2_THREADS)
#undef pthread_mutex_init
#undef pthread_cond_init

#include <netdb.h>

int my_pthread_mutex_init(pthread_mutex_t *mp, const pthread_mutexattr_t *attr)
{
  int error;
  if (!attr)
    error=pthread_mutex_init(mp,pthread_mutexattr_default);
  else
    error=pthread_mutex_init(mp,*attr);
  return error;
}

int my_pthread_cond_init(pthread_cond_t *mp, const pthread_condattr_t *attr)
{
  int error;
  if (!attr)
    error=pthread_cond_init(mp,pthread_condattr_default);
  else
    error=pthread_cond_init(mp,*attr);
  return error;
}

#endif

/*
** Emulate SOLARIS style calls, not because it's better, but just to make the
** usage of getbostbyname_r simpler.
*/

#if !defined(my_gethostbyname_r) && defined(HAVE_GETHOSTBYNAME_R)

#if defined(HAVE_GLIBC2_STYLE_GETHOSTBYNAME_R)

struct hostent *my_gethostbyname_r(const char *name,
				   struct hostent *result, char *buffer,
				   int buflen, int *h_errnop)
{
  struct hostent *hp;
  assert((size_t) buflen >= sizeof(*result));
  if (gethostbyname_r(name,result, buffer, (size_t) buflen, &hp, h_errnop))
    return 0;
  return hp;
}

#elif defined(_HPUX_SOURCE) || (defined(_AIX) && !defined(_AIX32_THREADS))

struct hostent *my_gethostbyname_r(const char *name,
				   struct hostent *result, char *buffer,
				   int buflen, int *h_errnop)
{
  assert(buflen >= sizeof(struct hostent_data));
  if (gethostbyname_r(name,result,(struct hostent_data *) buffer) == -1)
  {
    *h_errnop= errno;
    return 0;
  }
  return result;
}

#else

struct hostent *my_gethostbyname_r(const char *name,
				   struct hostent *result, char *buffer,
				   int buflen, int *h_errnop)
{
  struct hostent *hp;
  assert(buflen >= sizeof(struct hostent_data));
  hp= gethostbyname_r(name,result,(struct hostent_data *) buffer);
  *h_errnop= errno;
  return hp;
}

#endif /* GLIBC2_STYLE_GETHOSTBYNAME_R */
#endif


/* Some help functions */

int pthread_no_free(void *not_used __attribute__((unused)))
{
  return 0;
}

int pthread_dummy(int ret)
{
  return ret;
}
#endif /* THREAD */
