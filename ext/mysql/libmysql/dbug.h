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

#ifndef _dbug_h
#define _dbug_h
#ifdef	__cplusplus
extern "C" {
#endif
#if !defined(DBUG_OFF) && !defined(_lint)
extern	int _db_on_,_no_db_;
extern	FILE *_db_fp_;
extern	char *_db_process_;
extern	int _db_keyword_(const char *keyword);
extern	void _db_setjmp_(void);
extern	void _db_longjmp_(void);
extern	void _db_push_(const char *control);
extern	void _db_pop_(void);
extern	void _db_enter_(const char *_func_,const char *_file_,uint _line_,
			const char **_sfunc_,const char **_sfile_,
			uint *_slevel_, char ***);
extern	void _db_return_(uint _line_,const char **_sfunc_,const char **_sfile_,
			 uint *_slevel_);
extern	void _db_pargs_(uint _line_,const char *keyword);
extern	void _db_doprnt_ _VARARGS((const char *format,...));
extern	void _db_dump_(uint _line_,const char *keyword,const char *memory,
		       uint length);
extern	void _db_lock_file();
extern	void _db_unlock_file();

#define DBUG_ENTER(a) const char *_db_func_, *_db_file_; uint _db_level_; \
	char **_db_framep_; \
	_db_enter_ (a,__FILE__,__LINE__,&_db_func_,&_db_file_,&_db_level_, \
		    &_db_framep_)
#define DBUG_LEAVE \
	(_db_return_ (__LINE__, &_db_func_, &_db_file_, &_db_level_))
#define DBUG_RETURN(a1) {DBUG_LEAVE; return(a1);}
#define DBUG_VOID_RETURN {DBUG_LEAVE; return;}
#define DBUG_EXECUTE(keyword,a1) \
	{if (_db_on_) {if (_db_keyword_ (keyword)) { a1 }}}
#define DBUG_PRINT(keyword,arglist) \
	{if (_db_on_) {_db_pargs_(__LINE__,keyword); _db_doprnt_ arglist;}}
#define DBUG_PUSH(a1) _db_push_ (a1)
#define DBUG_POP() _db_pop_ ()
#define DBUG_PROCESS(a1) (_db_process_ = a1)
#define DBUG_FILE (_db_fp_)
#define DBUG_SETJMP(a1) (_db_setjmp_ (), setjmp (a1))
#define DBUG_LONGJMP(a1,a2) (_db_longjmp_ (), longjmp (a1, a2))
#define DBUG_DUMP(keyword,a1,a2)\
	{if (_db_on_) {_db_dump_(__LINE__,keyword,a1,a2);}}
#define DBUG_IN_USE (_db_fp_ && _db_fp_ != stderr)
#define DEBUGGER_OFF _no_db_=1;_db_on_=0;
#define DEBUGGER_ON  _no_db_=0
#define DBUG_LOCK_FILE { _db_lock_file(); }
#define DBUG_UNLOCK_FILE { _db_unlock_file(); }
#else						/* No debugger */

#define DBUG_ENTER(a1)
#define DBUG_RETURN(a1) return(a1)
#define DBUG_VOID_RETURN return
#define DBUG_EXECUTE(keyword,a1) {}
#define DBUG_PRINT(keyword,arglist) {}
#define DBUG_PUSH(a1) {}
#define DBUG_POP() {}
#define DBUG_PROCESS(a1) {}
#define DBUG_FILE (stderr)
#define DBUG_SETJMP setjmp
#define DBUG_LONGJMP longjmp
#define DBUG_DUMP(keyword,a1,a2) {}
#define DBUG_IN_USE 0
#define DEBUGGER_OFF
#define DEBUGGER_ON
#define DBUG_LOCK_FILE
#define DBUG_UNLOCK_FILE
#endif
#ifdef	__cplusplus
}
#endif
#endif
