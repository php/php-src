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
  File to include when we want to use alarm or a loop_counter to display
  some information when a program is running
*/
#ifndef _my_alarm_h
#define _my_alarm_h
#ifdef	__cplusplus
extern "C" {
#endif

extern int volatile my_have_got_alarm;
extern ulong my_time_to_wait_for_lock;

#if defined(HAVE_ALARM) && !defined(NO_ALARM_LOOP)
#include <signal.h>
#define ALARM_VARIABLES uint alarm_old=0; \
			sig_return alarm_signal=0
#define ALARM_INIT	my_have_got_alarm=0 ; \
			alarm_old=(uint) alarm(MY_HOW_OFTEN_TO_ALARM); \
			alarm_signal=signal(SIGALRM,my_set_alarm_variable);
#define ALARM_END	VOID(signal(SIGALRM,alarm_signal)); \
			VOID(alarm(alarm_old));
#define ALARM_TEST	my_have_got_alarm
#ifdef DONT_REMEMBER_SIGNAL
#define ALARM_REINIT	VOID(alarm(MY_HOW_OFTEN_TO_ALARM)); \
			VOID(signal(SIGALRM,my_set_alarm_variable));\
			my_have_got_alarm=0;
#else
#define ALARM_REINIT	VOID(alarm((uint) MY_HOW_OFTEN_TO_ALARM)); \
			my_have_got_alarm=0;
#endif /* DONT_REMEMBER_SIGNAL */
#else
#define ALARM_VARIABLES long alarm_pos=0,alarm_end_pos=MY_HOW_OFTEN_TO_WRITE-1
#define ALARM_INIT
#define ALARM_END
#define ALARM_TEST (alarm_pos++ >= alarm_end_pos)
#define ALARM_REINIT alarm_end_pos+=MY_HOW_OFTEN_TO_WRITE
#endif /* HAVE_ALARM */

#ifdef	__cplusplus
}
#endif
#endif
