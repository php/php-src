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

/*
  Static variables for mysys library. All definied here for easy making of
  a shared library
*/

#ifndef stdin
#include "mysys_priv.h"
#include "my_static.h"
#include "my_alarm.h"
#endif

	/* from my_init */
my_string	home_dir=0,my_progname=0;
char		NEAR curr_dir[FN_REFLEN]= {0},
		NEAR home_dir_buff[FN_REFLEN]= {0};
uint		my_stream_opened=0,my_file_opened=0;
int		NEAR my_umask=0664, NEAR my_umask_dir=0777;
#ifndef THREAD
int		NEAR my_errno=0;
#endif
struct my_file_info my_file_info[MY_NFILE]= {{0,UNOPEN}};

	/* From mf_brkhant */
int			NEAR my_dont_interrupt=0;
volatile int		_my_signals=0;
struct st_remember _my_sig_remember[MAX_SIGNALS]={{0,0}};
#ifdef THREAD
sigset_t my_signals;			/* signals blocked by mf_brkhant */
#endif

	/* from mf_keycache.c */
my_bool key_cache_inited=0;

	/* from mf_reccache.c */
ulong my_default_record_cache_size=RECORD_CACHE_SIZE;

	/* from soundex.c */
				/* ABCDEFGHIJKLMNOPQRSTUVWXYZ */
				/* :::::::::::::::::::::::::: */
const char *soundex_map=	  "01230120022455012623010202";

	/* from my_malloc */
USED_MEM* my_once_root_block=0;			/* pointer to first block */
uint	  my_once_extra=ONCE_ALLOC_INIT;	/* Memory to alloc / block */

	/* from my_tempnam */
#ifndef HAVE_TEMPNAM
int _my_tempnam_used=0;
#endif

	/* from safe_malloc */
uint sf_malloc_prehunc=0,		/* If you have problem with core- */
     sf_malloc_endhunc=0,		/* dump when malloc-message.... */
					/* set theese to 64 or 128  */
     sf_malloc_quick=0;			/* set if no calls to sanity */
long lCurMemory = 0L;			/* Current memory usage */
long lMaxMemory = 0L;			/* Maximum memory usage */
uint cNewCount = 0;			/* Number of times NEW() was called */
byte *sf_min_adress= (byte*) ~(unsigned long) 0L,
     *sf_max_adress= (byte*) 0L;

/* Root of the linked list of remembers */
struct remember *pRememberRoot = NULL;

	/* from my_alarm */
int volatile my_have_got_alarm=0;	/* declare variable to reset */
ulong my_time_to_wait_for_lock=2;	/* In seconds */

	/* from errors.c */
#ifdef SHARED_LIBRARY
char * NEAR globerrs[GLOBERRS];		/* my_error_messages is here */
#endif
void (*my_abort_hook)(int) = (void(*)(int)) exit;
int (*error_handler_hook)(uint error,const char *str,myf MyFlags)=
    my_message_no_curses;
int (*fatal_error_handler_hook)(uint error,const char *str,myf MyFlags)=
  my_message_no_curses;

	/* How to disable options */
my_bool NEAR my_disable_locking=0;
my_bool NEAR my_disable_async_io=0;
my_bool NEAR my_disable_flush_key_blocks=0;
my_bool NEAR mysys_uses_curses=0;
