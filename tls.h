/*
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997,1998 PHP Development Team (See Credits file)      |
   +----------------------------------------------------------------------+
   | This program is free software; you can redistribute it and/or modify |
   | it under the terms of one of the following licenses:                 |
   |                                                                      |
   |  A) the GNU General Public License as published by the Free Software |
   |     Foundation; either version 2 of the License, or (at your option) |
   |     any later version.                                               |
   |                                                                      |
   |  B) the PHP License as published by the PHP Development Team and     |
   |     included in the distribution in the file: LICENSE                |
   |                                                                      |
   | This program is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |
   | GNU General Public License for more details.                         |
   |                                                                      |
   | You should have received a copy of both licenses referred to here.   |
   | If you did not, or have any questions about PHP licensing, please    |
   | contact core@php.net.                                                |
   +----------------------------------------------------------------------+
   | Authors:                                                             |
   |                                                                      |
   +----------------------------------------------------------------------+
 */
#ifndef _TLS_H_
#define _TLS_H_
#if WIN32|WINNT
#include "win32/pwd.h"
#include "win32/sendmail.h"
#include <winsock.h>
#endif

#include "alloc.h"
#include "functions/head.h"
#include "ext/bcmath/number.h"
#include "constants.h"
#include <sys/stat.h>
#if USE_SAPI
#include "serverapi/sapi.h"
#endif

typedef struct php3_global_struct{
	/*all globals must be here*/
	/*alloc.c*/
	void *cache[MAX_CACHED_MEMORY][MAX_CACHED_ENTRIES];
	unsigned char cache_count[MAX_CACHED_MEMORY];
	int saved_umask;
	mem_header *head;
	unsigned int allocated_memory;

	/*getopt.c*/
	char *optarg;
	int optind;
	int opterr;
	int optopt;
	/*file.c*/
	int pclose_ret;
	int wsa_fp;
	/*zend-parser.tab.c*/
	HashTable function_table;
	HashTable include_names;
	int current_lineno;
	int include_count;
	FunctionState function_state;
	FunctionState php3g_function_state_for_require;
	char *class_name;
	HashTable *class_symbol_table;
	pval return_value,globals;
	unsigned int param_index;
	pval *array_ptr;
	/*list.c*/
	int module_count;
	int current_module_being_cleaned;
	/*main.c*/
	int initialized;
	int module_initialized;
	char *php3_ini_path;
	unsigned char header_is_being_sent;
#if APACHE
	request_rec *php3_rqst;
#endif
#if USE_SAPI
	struct sapi_request_info *sapi_rqst;
#endif
#if WIN32|WINNT
	unsigned int wintimer_counter;
	unsigned int wintimer;
	unsigned int timerstart;
#endif
	FILE *phpin;
	/*request_info.c*/
	php3_request_info request_info;
	/*token_cache.c*/
	pval phplval;
	/*constants.c*/
	HashTable php3_constants;

	/*Functions*/
	/*bc math*/
	long bc_precision;
	bc_num _zero_;
	bc_num _one_;
	bc_num _two_;
	/*browscap.c*/
	HashTable browser_hash;
	char *lookup_browser_name;
	pval *found_browser_entry;
	/*dir.c*/
	int dirp_id;
	int le_dirp;
	/*file.c*/
	int fgetss_state;
	int le_fp;
	int le_pp;
	/*filestat.c*/
	char *CurrentStatFile;
#if MSVC5
	unsigned int CurrentStatLength;
#else
	int CurrentStatLength;
#endif
	struct stat sb;
#if HAVE_SYMLINK
	struct stat lsb;
#endif
	/*formated_print.c*/
	char cvt_buf[80]; /*STATIC VAR*/
	/*head.c*/
	int php3_PrintHeader;
	CookieList *top;
	char *cont_type;
	int header_called;
	/*info.c*/
#if APACHE
	module *top_module;
#endif
	/*pageinfo.c*/
	long page_uid;
	long page_inode;
	long page_mtime;
	/*post.c*/
	int le_uploads;
	/*strings.h*/
	char *strtok_string;
	char *strtok_pos1; /*STATIC VAR*/
	char *strtok_pos2; /*STATIC VAR*/
#ifndef HAVE_STRERROR
	char str_ebuf[40]; /*STATIC VAR*/
#endif	
#if WIN32|WINNT
	/*pwd.c*/
	struct passwd pw;	/* should we return a malloc()'d structure   */
	/*sendmail.c*/
	char Buffer[MAIL_BUFFER_SIZE]; 
	SOCKET sc;
	WSADATA Data;
	struct hostent *adr;
	SOCKADDR_IN sock_in;
	int WinsockStarted;
	char *AppName;
	char MailHost[HOST_NAME_LEN];
	char LocalHost[HOST_NAME_LEN];
	/*winsyslog.c*/
	char *loghdr;		/* log file header string */
	HANDLE loghdl;	/* handle of event source */
	/*time.c*/
	unsigned int proftimer,virttimer,realtimer;
	LPMSG phpmsg;
	/*winutil*/
	char Win_Error_msg[256];
#endif
	/*check for each module if it is compiled staticly
	we should include their globals here.*/
} php3_globals_struct;

#ifdef THREAD_SAFE
extern DWORD TlsIndex;

extern int tls_create(void);
extern int tls_destroy(void);

/* these are from the flex scanner */
#ifndef YY_TLS_VARS
#define phptext php_gbl->text
#define phpleng php_gbl->leng
extern DWORD phpLexTlsIndex;
#define YY_TLS_VARS flex_globals *php_gbl = TlsGetValue(phpLexTlsIndex)
#endif

/* Other needed defines */
#if !defined(COMPILE_DL)
extern int phplex(pval *phplval, struct php3_global_struct *php3_globals, flex_globals *php_gbl);
#endif
#else
extern php3_globals_struct *php3_globals;

#endif

extern int tls_startup(void);
extern int tls_shutdown(void);

#endif
