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

#include "mysys_priv.h"
#include <signal.h>

#define MAX_SIGNALS	10		/* Max signals under a dont-allow */
#define MIN_KEYBLOCK	(min(IO_SIZE,1024))
#define MAX_KEYBLOCK	8192		/* Max keyblocklength == 8*IO_SIZE */
#define MAX_BLOCK_TYPES MAX_KEYBLOCK/MIN_KEYBLOCK

struct st_remember {
  int number;
  sig_handler (*func)(int number);
};

typedef struct sec_link {
  struct sec_link *next_hash,**prev_hash;/* Blocks linked acc. to hash-value */
  struct sec_link *next_used,*prev_used;
  struct sec_link *next_changed,**prev_changed;
  File file;
  my_off_t diskpos;
  byte *buffer;
  my_bool changed;
} SEC_LINK;

struct irem {
    struct remember *_pNext;		/* Linked list of structures	   */
    struct remember *_pPrev;		/* Other link			   */
    my_string _sFileName;		/* File in which memory was new'ed */
    uint _uLineNum;			/* Line number in above file	   */
    uint _uDataSize;			/* Size requested		   */
    long _lSpecialValue;		/* Underrun marker value	   */
};

struct remember {
    struct irem tInt;
    char aData[1];
};

extern char	NEAR curr_dir[FN_REFLEN],NEAR home_dir_buff[FN_REFLEN];

extern volatile int _my_signals;
extern struct st_remember _my_sig_remember[MAX_SIGNALS];

extern my_bool	key_cache_inited;

extern const char *soundex_map;

extern USED_MEM* my_once_root_block;
extern uint	 my_once_extra;

#ifndef HAVE_TEMPNAM
extern int	_my_tempnam_used;
#endif

extern byte	*sf_min_adress,*sf_max_adress;
extern uint	cNewCount;
extern struct remember *pRememberRoot;

#if defined(THREAD) && !defined(__WIN__)
extern sigset_t my_signals;		/* signals blocked by mf_brkhant */
#endif
