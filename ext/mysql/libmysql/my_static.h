/* Copyright Abandoned 1996 TCX DataKonsult AB & Monty Program KB & Detron HB 
This file is public domain and comes with NO WARRANTY of any kind */

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
