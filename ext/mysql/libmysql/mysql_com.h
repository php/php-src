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
** Common definition between mysql server & client
*/

#ifndef _mysql_com_h
#define _mysql_com_h


#define NAME_LEN	64		/* Field/table name length */
#define HOSTNAME_LENGTH 60
#define USERNAME_LENGTH 16

#define LOCAL_HOST	"localhost"
#define LOCAL_HOST_NAMEDPIPE "."

#if defined(__EMX__) || defined(__OS2__)
#undef MYSQL_UNIX_ADDR
#define MYSQL_OS2_ADDR "\\socket\\MySQL"
#define MYSQL_UNIX_ADDR MYSQL_OS2_ADDR
#endif
#if defined(__WIN__) && !defined( _CUSTOMCONFIG_)
#define MYSQL_NAMEDPIPE "MySQL"
#define MYSQL_SERVICENAME "MySql"
#endif /* __WIN__ */

enum enum_server_command {COM_SLEEP,COM_QUIT,COM_INIT_DB,COM_QUERY,
			  COM_FIELD_LIST,COM_CREATE_DB,COM_DROP_DB,COM_REFRESH,
			  COM_SHUTDOWN,COM_STATISTICS,
			  COM_PROCESS_INFO,COM_CONNECT,COM_PROCESS_KILL,
			  COM_DEBUG,COM_PING,COM_TIME,COM_DELAYED_INSERT,
			  COM_CHANGE_USER, COM_BINLOG_DUMP,
                          COM_TABLE_DUMP};

#define NOT_NULL_FLAG	1		/* Field can't be NULL */
#define PRI_KEY_FLAG	2		/* Field is part of a primary key */
#define UNIQUE_KEY_FLAG 4		/* Field is part of a unique key */
#define MULTIPLE_KEY_FLAG 8		/* Field is part of a key */
#define BLOB_FLAG	16		/* Field is a blob */
#define UNSIGNED_FLAG	32		/* Field is unsigned */
#define ZEROFILL_FLAG	64		/* Field is zerofill */
#define BINARY_FLAG	128
/* The following are only sent to new clients */
#define ENUM_FLAG	256		/* field is an enum */
#define AUTO_INCREMENT_FLAG 512		/* field is a autoincrement field */
#define TIMESTAMP_FLAG	1024		/* Field is a timestamp */
#define SET_FLAG	2048		/* field is a set */
#define PART_KEY_FLAG	16384		/* Intern; Part of some key */
#define GROUP_FLAG	32768		/* Intern: Group field */
#define UNIQUE_FLAG	65536		/* Intern: Used by sql_yacc */

#define REFRESH_GRANT		1	/* Refresh grant tables */
#define REFRESH_LOG		2	/* Start on new log file */
#define REFRESH_TABLES		4	/* close all tables */
#define REFRESH_HOSTS		8	/* Flush host cache */
#define REFRESH_STATUS		16	/* Flush status variables */
#define REFRESH_THREADS		32	/* Flush status variables */
#define REFRESH_SLAVE           64      /* Reset master info and restart slave
					   thread */
#define REFRESH_MASTER          128     /* Remove all bin logs in the index
					   and truncate the index */

/* The following can't be set with mysql_refresh() */
#define REFRESH_READ_LOCK	16384	/* Lock tables for read */
#define REFRESH_FAST		32768	/* Intern flag */

#define CLIENT_LONG_PASSWORD	1	/* new more secure passwords */
#define CLIENT_FOUND_ROWS	2	/* Found instead of affected rows */
#define CLIENT_LONG_FLAG	4	/* Get all column flags */
#define CLIENT_CONNECT_WITH_DB	8	/* One can specify db on connect */
#define CLIENT_NO_SCHEMA	16	/* Don't allow database.table.column */
#define CLIENT_COMPRESS		32	/* Can use compression protocol */
#define CLIENT_ODBC		64	/* Odbc client */
#define CLIENT_LOCAL_FILES	128	/* Can use LOAD DATA LOCAL */
#define CLIENT_IGNORE_SPACE	256	/* Ignore spaces before '(' */
#define CLIENT_CHANGE_USER	512	/* Support the mysql_change_user() */
#define CLIENT_INTERACTIVE	1024	/* This is an interactive client */
#define CLIENT_SSL              2048     /* Switch to SSL after handshake */
#define CLIENT_IGNORE_SIGPIPE   4096     /* IGNORE sigpipes */
#define CLIENT_TRANSACTIONS	8196	/* Client knows about transactions */

#define SERVER_STATUS_IN_TRANS  1	/* Transaction has started */
#define SERVER_STATUS_AUTOCOMMIT 2	/* Server in auto_commit mode */

#define MYSQL_ERRMSG_SIZE	200
#define NET_READ_TIMEOUT	30		/* Timeout on read */
#define NET_WRITE_TIMEOUT	60		/* Timeout on write */
#define NET_WAIT_TIMEOUT	8*60*60		/* Wait for new query */

#ifndef Vio_defined
#define Vio_defined
#ifdef HAVE_VIO
class Vio;					/* Fill Vio class in C++ */
#else
struct st_vio;					/* Only C */
typedef struct st_vio Vio;
#endif
#endif

typedef struct st_net {
  Vio* vio;
  my_socket fd;					/* For Perl DBI/dbd */
  int fcntl;
  unsigned char *buff,*buff_end,*write_pos,*read_pos;
  char last_error[MYSQL_ERRMSG_SIZE];
  unsigned int last_errno,max_packet,timeout,pkt_nr;
  unsigned char error;
  my_bool return_errno,compress;
  my_bool no_send_ok; /* needed if we are doing several
   queries in one command ( as in LOAD TABLE ... FROM MASTER ),
   and do not want to confuse the client with OK at the wrong time
		      */
  unsigned long remain_in_buf,length, buf_length, where_b;
  unsigned int *return_status;
  unsigned char reading_or_writing;
  char save_char;
} NET;

#define packet_error ((unsigned int) -1)

enum enum_field_types { FIELD_TYPE_DECIMAL, FIELD_TYPE_TINY,
			FIELD_TYPE_SHORT,  FIELD_TYPE_LONG,
			FIELD_TYPE_FLOAT,  FIELD_TYPE_DOUBLE,
			FIELD_TYPE_NULL,   FIELD_TYPE_TIMESTAMP,
			FIELD_TYPE_LONGLONG,FIELD_TYPE_INT24,
			FIELD_TYPE_DATE,   FIELD_TYPE_TIME,
			FIELD_TYPE_DATETIME, FIELD_TYPE_YEAR,
			FIELD_TYPE_NEWDATE,
			FIELD_TYPE_ENUM=247,
			FIELD_TYPE_SET=248,
			FIELD_TYPE_TINY_BLOB=249,
			FIELD_TYPE_MEDIUM_BLOB=250,
			FIELD_TYPE_LONG_BLOB=251,
			FIELD_TYPE_BLOB=252,
			FIELD_TYPE_VAR_STRING=253,
			FIELD_TYPE_STRING=254
};

#define FIELD_TYPE_CHAR FIELD_TYPE_TINY		/* For compability */
#define FIELD_TYPE_INTERVAL FIELD_TYPE_ENUM	/* For compability */

extern unsigned long max_allowed_packet;
extern unsigned long net_buffer_length;

#define net_new_transaction(net) ((net)->pkt_nr=0)

int	my_net_init(NET *net, Vio* vio);
void	net_end(NET *net);
void	net_clear(NET *net);
int	net_flush(NET *net);
int	my_net_write(NET *net,const char *packet,unsigned long len);
int	net_write_command(NET *net,unsigned char command,const char *packet,
			  unsigned long len);
int	net_real_write(NET *net,const char *packet,unsigned long len);
unsigned int	my_net_read(NET *net);

struct rand_struct {
  unsigned long seed1,seed2,max_value;
  double max_value_dbl;
};

  /* The following is for user defined functions */

enum Item_result {STRING_RESULT,REAL_RESULT,INT_RESULT};

typedef struct st_udf_args
{
  unsigned int arg_count;		/* Number of arguments */
  enum Item_result *arg_type;		/* Pointer to item_results */
  char **args;				/* Pointer to argument */
  unsigned long *lengths;		/* Length of string arguments */
  char *maybe_null;			/* Set to 1 for all maybe_null args */
} UDF_ARGS;

  /* This holds information about the result */

typedef struct st_udf_init
{
  my_bool maybe_null;			/* 1 if function can return NULL */
  unsigned int decimals;		/* for real functions */
  unsigned int max_length;		/* For string functions */
  char	  *ptr;				/* free pointer for function data */
  my_bool const_item;			/* 0 if result is independent of arguments */
} UDF_INIT;

  /* Constants when using compression */
#define NET_HEADER_SIZE 4		/* standard header size */
#define COMP_HEADER_SIZE 3		/* compression header extra size */

  /* Prototypes to password functions */

#ifdef __cplusplus
extern "C" {
#endif
  
void randominit(struct rand_struct *,unsigned long seed1,
		unsigned long seed2);
double rnd(struct rand_struct *);
void make_scrambled_password(char *to,const char *password);
void get_salt_from_password(unsigned long *res,const char *password);
void make_password_from_salt(char *to, unsigned long *hash_res);
char *scramble(char *to,const char *message,const char *password,
	       my_bool old_ver);
my_bool check_scramble(const char *, const char *message,
		       unsigned long *salt,my_bool old_ver);
char *get_tty_password(char *opt_message);
void hash_password(unsigned long *result, const char *password);
#ifdef __cplusplus
}
#endif

/* Some other useful functions */

void my_init(void);
void load_defaults(const char *conf_file, const char **groups,
		   int *argc, char ***argv);

#define NULL_LENGTH ((unsigned long) ~0) /* For net_store_length */

#ifdef __WIN__
#define socket_errno WSAGetLastError()
#else
#define socket_errno errno
#endif

#endif
