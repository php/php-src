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

/* defines for the libmysql library */

#ifndef _mysql_h
#define _mysql_h

#ifndef MYSQL_SERVER
#ifdef	__cplusplus
extern "C" {
#endif
#endif
  
#ifndef _global_h				/* If not standard header */
#include <sys/types.h>
typedef char my_bool;
#if (defined(_WIN32) || defined(_WIN64)) && !defined(__WIN__)
#define __WIN__
#endif
#if !defined(__WIN__)
#define STDCALL
#else
#define STDCALL __stdcall
#endif
typedef char * gptr;

#ifndef ST_USED_MEM_DEFINED
#define ST_USED_MEM_DEFINED
typedef struct st_used_mem {			/* struct for once_alloc */
  struct st_used_mem *next;			/* Next block in use */
  unsigned int	left;				/* memory left in block  */
  unsigned int	size;				/* size of block */
} USED_MEM;
typedef struct st_mem_root {
  USED_MEM *free;
  USED_MEM *used;
  unsigned int	min_malloc;
  unsigned int	block_size;
  void (*error_handler)(void);
} MEM_ROOT;
#endif

#ifndef my_socket_defined
#ifdef __WIN__
#define my_socket SOCKET
#else
typedef int my_socket;
#endif
#endif
#endif
#include "mysql_com.h"
#include "mysql_version.h"

extern unsigned int mysql_port;
extern char *mysql_unix_port;

#define IS_PRI_KEY(n)	((n) & PRI_KEY_FLAG)
#define IS_NOT_NULL(n)	((n) & NOT_NULL_FLAG)
#define IS_BLOB(n)	((n) & BLOB_FLAG)
#define IS_NUM(t)	((t) <= FIELD_TYPE_INT24 || (t) == FIELD_TYPE_YEAR)

typedef struct st_mysql_field {
  char *name;			/* Name of column */
  char *table;			/* Table of column if column was a field */
  char *def;			/* Default value (set by mysql_list_fields) */
  enum enum_field_types type;	/* Type of field. Se mysql_com.h for types */
  unsigned int length;		/* Width of column */
  unsigned int max_length;	/* Max width of selected set */
  unsigned int flags;		/* Div flags */
  unsigned int decimals;	/* Number of decimals in field */
} MYSQL_FIELD;

typedef char **MYSQL_ROW;		/* return data as array of strings */
typedef unsigned int MYSQL_FIELD_OFFSET; /* offset to current field */

#if defined(NO_CLIENT_LONG_LONG)
typedef unsigned long my_ulonglong;
#elif defined (__WIN__)
typedef unsigned __int64 my_ulonglong;
#else
typedef unsigned long long my_ulonglong;
#endif

#define MYSQL_COUNT_ERROR (~(my_ulonglong) 0)

typedef struct st_mysql_rows {
  struct st_mysql_rows *next;		/* list of rows */
  MYSQL_ROW data;
} MYSQL_ROWS;

typedef MYSQL_ROWS *MYSQL_ROW_OFFSET;	/* offset to current row */

typedef struct st_mysql_data {
  my_ulonglong rows;
  unsigned int fields;
  MYSQL_ROWS *data;
  MEM_ROOT alloc;
} MYSQL_DATA;

struct st_mysql_options {
  unsigned int connect_timeout,client_flag;
  my_bool compress,named_pipe;
  unsigned int port;
  char *host,*init_command,*user,*password,*unix_socket,*db;
  char *my_cnf_file,*my_cnf_group, *charset_dir, *charset_name;
  my_bool use_ssl;				/* if to use SSL or not */
  char *ssl_key;				/* PEM key file */
  char *ssl_cert;				/* PEM cert file */
  char *ssl_ca;					/* PEM CA file */
  char *ssl_capath;				/* PEM directory of CA-s? */
};

enum mysql_option { MYSQL_OPT_CONNECT_TIMEOUT, MYSQL_OPT_COMPRESS,
		    MYSQL_OPT_NAMED_PIPE, MYSQL_INIT_COMMAND,
		    MYSQL_READ_DEFAULT_FILE, MYSQL_READ_DEFAULT_GROUP,
		    MYSQL_SET_CHARSET_DIR, MYSQL_SET_CHARSET_NAME};

enum mysql_status { MYSQL_STATUS_READY,MYSQL_STATUS_GET_RESULT,
		    MYSQL_STATUS_USE_RESULT};

typedef struct st_mysql {
  NET		net;			/* Communication parameters */
  gptr		connector_fd;		/* ConnectorFd for SSL */
  char		*host,*user,*passwd,*unix_socket,*server_version,*host_info,
		*info,*db;
  unsigned int	port,client_flag,server_capabilities;
  unsigned int	protocol_version;
  unsigned int	field_count;
  unsigned int 	server_status;
  unsigned long thread_id;		/* Id for connection in server */
  my_ulonglong affected_rows;
  my_ulonglong insert_id;		/* id if insert on table with NEXTNR */
  my_ulonglong extra_info;		/* Used by mysqlshow */
  unsigned long packet_length;
  enum mysql_status status;
  MYSQL_FIELD	*fields;
  MEM_ROOT	field_alloc;
  my_bool	free_me;		/* If free in mysql_close */
  my_bool	reconnect;		/* set to 1 if automatic reconnect */
  struct st_mysql_options options;
  char	        scramble_buff[9];
  struct charset_info_st *charset;
  unsigned int  server_language;
} MYSQL;


typedef struct st_mysql_res {
  my_ulonglong row_count;
  unsigned int	field_count, current_field;
  MYSQL_FIELD	*fields;
  MYSQL_DATA	*data;
  MYSQL_ROWS	*data_cursor;
  MEM_ROOT	field_alloc;
  MYSQL_ROW	row;			/* If unbuffered read */
  MYSQL_ROW	current_row;		/* buffer to current row */
  unsigned long *lengths;		/* column lengths of current row */
  MYSQL		*handle;		/* for unbuffered reads */
  my_bool	eof;			/* Used my mysql_fetch_row */
} MYSQL_RES;

/* Functions to get information from the MYSQL and MYSQL_RES structures */
/* Should definitely be used if one uses shared libraries */

my_ulonglong STDCALL mysql_num_rows(MYSQL_RES *res);
unsigned int STDCALL mysql_num_fields(MYSQL_RES *res);
my_bool STDCALL mysql_eof(MYSQL_RES *res);
MYSQL_FIELD *STDCALL mysql_fetch_field_direct(MYSQL_RES *res,
					      unsigned int fieldnr);
MYSQL_FIELD * STDCALL mysql_fetch_fields(MYSQL_RES *res);
MYSQL_ROWS * STDCALL mysql_row_tell(MYSQL_RES *res);
unsigned int STDCALL mysql_field_tell(MYSQL_RES *res);

unsigned int STDCALL mysql_field_count(MYSQL *mysql);
my_ulonglong STDCALL mysql_affected_rows(MYSQL *mysql);
my_ulonglong STDCALL mysql_insert_id(MYSQL *mysql);
unsigned int STDCALL mysql_errno(MYSQL *mysql);
char * STDCALL mysql_error(MYSQL *mysql);
char * STDCALL mysql_info(MYSQL *mysql);
unsigned long STDCALL mysql_thread_id(MYSQL *mysql);
const char * STDCALL mysql_character_set_name(MYSQL *mysql);

MYSQL *		STDCALL mysql_init(MYSQL *mysql);
#ifdef HAVE_OPENSSL
int		STDCALL mysql_ssl_set(MYSQL *mysql, const char *key,
				      const char *cert, const char *ca,
				      const char *capath);
char *		STDCALL mysql_ssl_cipher(MYSQL *mysql);
int		STDCALL mysql_ssl_clear(MYSQL *mysql);
#endif /* HAVE_OPENSSL */
MYSQL *		STDCALL mysql_connect(MYSQL *mysql, const char *host,
				      const char *user, const char *passwd);
my_bool		STDCALL mysql_change_user(MYSQL *mysql, const char *user, 
					  const char *passwd, const char *db);
#if MYSQL_VERSION_ID >= 32200
MYSQL *		STDCALL mysql_real_connect(MYSQL *mysql, const char *host,
					   const char *user,
					   const char *passwd,
					   const char *db,
					   unsigned int port,
					   const char *unix_socket,
					   unsigned int clientflag);
#else
MYSQL *		STDCALL mysql_real_connect(MYSQL *mysql, const char *host,
					   const char *user,
					   const char *passwd,
					   unsigned int port,
					   const char *unix_socket,
					   unsigned int clientflag);
#endif
void		STDCALL mysql_close(MYSQL *sock);
int		STDCALL mysql_select_db(MYSQL *mysql, const char *db);
int		STDCALL mysql_query(MYSQL *mysql, const char *q);
int		STDCALL mysql_real_query(MYSQL *mysql, const char *q,
					unsigned int length);
int		STDCALL mysql_create_db(MYSQL *mysql, const char *DB);
int		STDCALL mysql_drop_db(MYSQL *mysql, const char *DB);
int		STDCALL mysql_shutdown(MYSQL *mysql);
int		STDCALL mysql_dump_debug_info(MYSQL *mysql);
int		STDCALL mysql_refresh(MYSQL *mysql,
				     unsigned int refresh_options);
int		STDCALL mysql_kill(MYSQL *mysql,unsigned long pid);
int		STDCALL mysql_ping(MYSQL *mysql);
char *		STDCALL mysql_stat(MYSQL *mysql);
char *		STDCALL mysql_get_server_info(MYSQL *mysql);
char *		STDCALL mysql_get_client_info(void);
char *		STDCALL mysql_get_host_info(MYSQL *mysql);
unsigned int	STDCALL mysql_get_proto_info(MYSQL *mysql);
MYSQL_RES *	STDCALL mysql_list_dbs(MYSQL *mysql,const char *wild);
MYSQL_RES *	STDCALL mysql_list_tables(MYSQL *mysql,const char *wild);
MYSQL_RES *	STDCALL mysql_list_fields(MYSQL *mysql, const char *table,
					 const char *wild);
MYSQL_RES *	STDCALL mysql_list_processes(MYSQL *mysql);
MYSQL_RES *	STDCALL mysql_store_result(MYSQL *mysql);
MYSQL_RES *	STDCALL mysql_use_result(MYSQL *mysql);
int		STDCALL mysql_options(MYSQL *mysql,enum mysql_option option,
				      const char *arg);
void		STDCALL mysql_free_result(MYSQL_RES *result);
void		STDCALL mysql_data_seek(MYSQL_RES *result,
					my_ulonglong offset);
MYSQL_ROW_OFFSET STDCALL mysql_row_seek(MYSQL_RES *result, MYSQL_ROW_OFFSET);
MYSQL_FIELD_OFFSET STDCALL mysql_field_seek(MYSQL_RES *result,
					   MYSQL_FIELD_OFFSET offset);
MYSQL_ROW	STDCALL mysql_fetch_row(MYSQL_RES *result);
unsigned long * STDCALL mysql_fetch_lengths(MYSQL_RES *result);
MYSQL_FIELD *	STDCALL mysql_fetch_field(MYSQL_RES *result);
unsigned long	STDCALL mysql_escape_string(char *to,const char *from,
					    unsigned long from_length);
unsigned long STDCALL mysql_real_escape_string(MYSQL *mysql,
					       char *to,const char *from,
					       unsigned long length);
void		STDCALL mysql_debug(const char *debug);
char *		STDCALL mysql_odbc_escape_string(MYSQL *mysql,
						 char *to,
						 unsigned long to_length,
						 const char *from,
						 unsigned long from_length,
						 void *param,
						 char *
						 (*extend_buffer)
						 (void *, char *to,
						  unsigned long *length));
void 		STDCALL myodbc_remove_escape(MYSQL *mysql,char *name);
unsigned int	STDCALL mysql_thread_safe(void);

  
#define mysql_reload(mysql) mysql_refresh((mysql),REFRESH_GRANT)

/* new api functions */

#define HAVE_MYSQL_REAL_CONNECT

#ifndef MYSQL_SERVER  
#ifdef	__cplusplus
}
#endif
#endif

#endif
