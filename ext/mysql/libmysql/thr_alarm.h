/* Copyright Abandoned 1996 TCX DataKonsult AB & Monty Program KB & Detron HB 
This file is public domain and comes with NO WARRANTY of any kind */

/* Prototypes when using thr_alarm library functions */

#ifndef _thr_alarm_h
#define _thr_alarm_h
#ifdef	__cplusplus
extern "C" {
#endif

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

#if defined(DONT_USE_THR_ALARM)

#define USE_ALARM_THREAD
#undef USE_ONE_SIGNAL_HAND

typedef struct st_thr_alarm_entry
{
  uint crono;
} thr_alarm_entry;

#define thr_alarm_init(A)   (A)->crono=0
#define thr_alarm_in_use(A) (A)->crono
#define init_thr_alarm(A)
#define thr_alarm_kill(A)
#define end_thr_alarm()
#define thr_alarm(A,B) (((A)->crono=1)-1)
#define thr_got_alarm(A) (A)->crono
#define thr_end_alarm(A)

#else
#if defined(__WIN__)
typedef struct st_thr_alarm_entry
{
  rf_SetTimer crono;
} thr_alarm_entry;

#elif defined(__EMX__) || defined(OS2)

typedef struct st_thr_alarm_entry
{
  uint crono;
  uint event;
} thr_alarm_entry;

#else /* System with posix threads */

typedef int thr_alarm_entry;

#define thr_got_alarm(thr_alarm) (**(thr_alarm))

#endif /* __WIN__ */

typedef thr_alarm_entry* thr_alarm_t;

typedef struct st_alarm {
  ulong expire_time;
  thr_alarm_entry alarmed;		/* set when alarm is due */
  pthread_t thread;
  my_bool malloced;
} ALARM;

#define thr_alarm_init(A) (*(A))=0
#define thr_alarm_in_use(A) (*(A)!= 0)
void init_thr_alarm(uint max_alarm);
bool thr_alarm(thr_alarm_t *alarmed, uint sec, ALARM *buff);
void thr_alarm_kill(pthread_t thread_id);
void thr_end_alarm(thr_alarm_t *alarmed);
void end_thr_alarm(void);
sig_handler process_alarm(int);
#ifndef thr_got_alarm
bool thr_got_alarm(thr_alarm_t *alrm);
#endif


#endif /* DONT_USE_THR_ALARM */

#ifdef	__cplusplus
}
#endif /* __cplusplus */
#endif /* _thr_alarm_h */

