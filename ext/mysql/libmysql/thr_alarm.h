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

/* Prototypes when using thr_alarm library functions */

#ifndef _thr_alarm_h
#define _thr_alarm_h
#ifdef	__cplusplus
extern "C" {
#endif

typedef struct st_alarm {
  ulong expire_time;
  int alarmed;					/* 1 when alarm is due */
  pthread_t thread;
  my_bool malloced;
} ALARM;

#ifndef USE_ALARM_THREAD
#define USE_ONE_SIGNAL_HAND		/* One must call process_alarm */
#endif
#ifdef HAVE_LINUXTHREADS
#define THR_CLIENT_ALARM SIGALRM
#else
#define THR_CLIENT_ALARM SIGUSR1
#endif
#ifdef HAVE_rts_threads
#undef USE_ONE_SIGNAL_HAND
#define USE_ALARM_THREAD
#define THR_SERVER_ALARM SIGUSR1
#else
#define THR_SERVER_ALARM SIGALRM
#endif

#ifdef DONT_USE_THR_ALARM

#define USE_ALARM_THREAD
#undef USE_ONE_SIGNAL_HAND

typedef struct st_win_timer
{
  uint crono;
} thr_alarm_t;

#define thr_alarm_init(A)   (A)->crono=0
#define thr_alarm_in_use(A) (A).crono
#define init_thr_alarm(A)
#define thr_alarm_kill(A)
#define end_thr_alarm()
#define thr_alarm(A,B) (((A)->crono=1)-1)
#define thr_got_alarm(A) (A).crono
#define thr_end_alarm(A)

#else

#ifdef __WIN__
typedef struct st_win_timer
{
  rf_SetTimer crono;
} thr_alarm_t;

bool thr_got_alarm(thr_alarm_t *alrm);
#define thr_alarm_init(A)   (A)->crono=0
#define thr_alarm_in_use(A) (A)->crono
#define init_thr_alarm(A)
#define thr_alarm_kill(A)
#else

typedef int* thr_alarm_t;
#define thr_got_alarm(thr_alarm) (*thr_alarm)
#define thr_alarm_init(A) (*A)=0
#define thr_alarm_in_use(A) ((A) != 0)
void init_thr_alarm(uint max_alarm);
void thr_alarm_kill(pthread_t thread_id);
sig_handler process_alarm(int);
#endif /* __WIN__ */

bool thr_alarm(thr_alarm_t *alarmed,uint sec, ALARM *buff);
void thr_end_alarm(thr_alarm_t *alarmed);
void end_thr_alarm(void);
#endif /* DONT_USE_THR_ALARM */

#ifdef	__cplusplus
}
#endif
#endif
