/*-------------------------------------------------------------------------
 *
 * libpq-fe.h
 *	  This file contains definitions for structures and
 *	  externs for functions used by frontend postgres applications.
 *
 * Portions Copyright (c) 1996-2001, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * $Id$
 *
 *-------------------------------------------------------------------------
 */

#ifndef LIBPQ_FE_H
#define LIBPQ_FE_H

#ifdef __cplusplus
extern		"C"
{
#endif

#include <stdio.h>

/*
 * postgres_ext.h defines the backend's externally visible types,
 * such as Oid.
 */
#include "postgres_ext.h"

/* SSL type is needed here only to declare PQgetssl() */
#ifdef USE_SSL
#include <openssl/ssl.h>
#endif

/* Application-visible enum types */

typedef enum
{
	/*
	 * Although you may decide to change this list in some way, values
	 * which become unused should never be removed, nor should constants
	 * be redefined - that would break compatibility with existing code.
	 */
	CONNECTION_OK,
	CONNECTION_BAD,
	/* Non-blocking mode only below here */

	/*
	 * The existence of these should never be relied upon - they should
	 * only be used for user feedback or similar purposes.
	 */
	CONNECTION_STARTED,			/* Waiting for connection to be made.  */
	CONNECTION_MADE,			/* Connection OK; waiting to send.	   */
	CONNECTION_AWAITING_RESPONSE,		/* Waiting for a response from the
										 * postmaster.		  */
	CONNECTION_AUTH_OK,			/* Received authentication; waiting for
								 * backend startup. */
	CONNECTION_SETENV			/* Negotiating environment.    */
} ConnStatusType;

typedef enum
{
	PGRES_POLLING_FAILED = 0,
	PGRES_POLLING_READING,		/* These two indicate that one may	  */
	PGRES_POLLING_WRITING,		/* use select before polling again.   */
	PGRES_POLLING_OK,
	PGRES_POLLING_ACTIVE		/* Can call poll function immediately. */
} PostgresPollingStatusType;

typedef enum
{
	PGRES_EMPTY_QUERY = 0,
	PGRES_COMMAND_OK,			/* a query command that doesn't return
								 * anything was executed properly by the
								 * backend */
	PGRES_TUPLES_OK,			/* a query command that returns tuples was
								 * executed properly by the backend,
								 * PGresult contains the result tuples */
	PGRES_COPY_OUT,				/* Copy Out data transfer in progress */
	PGRES_COPY_IN,				/* Copy In data transfer in progress */
	PGRES_BAD_RESPONSE,			/* an unexpected response was recv'd from
								 * the backend */
	PGRES_NONFATAL_ERROR,
	PGRES_FATAL_ERROR
} ExecStatusType;

/* PGconn encapsulates a connection to the backend.
 * The contents of this struct are not supposed to be known to applications.
 */
typedef struct pg_conn PGconn;

/* PGresult encapsulates the result of a query (or more precisely, of a single
 * SQL command --- a query string given to PQsendQuery can contain multiple
 * commands and thus return multiple PGresult objects).
 * The contents of this struct are not supposed to be known to applications.
 */
typedef struct pg_result PGresult;

/* PGnotify represents the occurrence of a NOTIFY message.
 * Ideally this would be an opaque typedef, but it's so simple that it's
 * unlikely to change.
 * NOTE: in Postgres 6.4 and later, the be_pid is the notifying backend's,
 * whereas in earlier versions it was always your own backend's PID.
 */
typedef struct pgNotify
{
	char		relname[NAMEDATALEN];	/* name of relation containing
										 * data */
	int			be_pid;			/* process id of backend */
} PGnotify;

/* PQnoticeProcessor is the function type for the notice-message callback.
 */
typedef void (*PQnoticeProcessor) (void *arg, const char *message);

/* Print options for PQprint() */
typedef char pqbool;

typedef struct _PQprintOpt
{
	pqbool		header;			/* print output field headings and row
								 * count */
	pqbool		align;			/* fill align the fields */
	pqbool		standard;		/* old brain dead format */
	pqbool		html3;			/* output html tables */
	pqbool		expanded;		/* expand tables */
	pqbool		pager;			/* use pager for output if needed */
	char	   *fieldSep;		/* field separator */
	char	   *tableOpt;		/* insert to HTML <table ...> */
	char	   *caption;		/* HTML <caption> */
	char	  **fieldName;		/* null terminated array of repalcement
								 * field names */
} PQprintOpt;

/* ----------------
 * Structure for the conninfo parameter definitions returned by PQconndefaults
 *
 * All fields except "val" point at static strings which must not be altered.
 * "val" is either NULL or a malloc'd current-value string.  PQconninfoFree()
 * will release both the val strings and the PQconninfoOption array itself.
 * ----------------
 */
typedef struct _PQconninfoOption
{
	char	   *keyword;		/* The keyword of the option			*/
	char	   *envvar;			/* Fallback environment variable name	*/
	char	   *compiled;		/* Fallback compiled in default value	*/
	char	   *val;			/* Option's current value, or NULL		 */
	char	   *label;			/* Label for field in connect dialog	*/
	char	   *dispchar;		/* Character to display for this field in
								 * a connect dialog. Values are: ""
								 * Display entered value as is "*"
								 * Password field - hide value "D"	Debug
								 * option - don't show by default */
	int			dispsize;		/* Field size in characters for dialog	*/
} PQconninfoOption;

/* ----------------
 * PQArgBlock -- structure for PQfn() arguments
 * ----------------
 */
typedef struct
{
	int			len;
	int			isint;
	union
	{
		int		   *ptr;		/* can't use void (dec compiler barfs)	 */
		int			integer;
	}			u;
} PQArgBlock;

/* ----------------
 * Exported functions of libpq
 * ----------------
 */

/* ===	in fe-connect.c === */

/* make a new client connection to the backend */
/* Asynchronous (non-blocking) */
extern PGconn *PQconnectStart(const char *conninfo);
extern PostgresPollingStatusType PQconnectPoll(PGconn *conn);

/* Synchronous (blocking) */
extern PGconn *PQconnectdb(const char *conninfo);
extern PGconn *PQsetdbLogin(const char *pghost, const char *pgport,
			 const char *pgoptions, const char *pgtty,
			 const char *dbName,
			 const char *login, const char *pwd);

#define PQsetdb(M_PGHOST,M_PGPORT,M_PGOPT,M_PGTTY,M_DBNAME)  \
	PQsetdbLogin(M_PGHOST, M_PGPORT, M_PGOPT, M_PGTTY, M_DBNAME, NULL, NULL)

/* close the current connection and free the PGconn data structure */
extern void PQfinish(PGconn *conn);

/* get info about connection options known to PQconnectdb */
extern PQconninfoOption *PQconndefaults(void);

/* free the data structure returned by PQconndefaults() */
extern void PQconninfoFree(PQconninfoOption *connOptions);

/*
 * close the current connection and restablish a new one with the same
 * parameters
 */
/* Asynchronous (non-blocking) */
extern int	PQresetStart(PGconn *conn);
extern PostgresPollingStatusType PQresetPoll(PGconn *conn);

/* Synchronous (blocking) */
extern void PQreset(PGconn *conn);

/* issue a cancel request */
extern int	PQrequestCancel(PGconn *conn);

/* Accessor functions for PGconn objects */
extern char *PQdb(const PGconn *conn);
extern char *PQuser(const PGconn *conn);
extern char *PQpass(const PGconn *conn);
extern char *PQhost(const PGconn *conn);
extern char *PQport(const PGconn *conn);
extern char *PQtty(const PGconn *conn);
extern char *PQoptions(const PGconn *conn);
extern ConnStatusType PQstatus(const PGconn *conn);
extern char *PQerrorMessage(const PGconn *conn);
extern int	PQsocket(const PGconn *conn);
extern int	PQbackendPID(const PGconn *conn);
extern int	PQclientEncoding(const PGconn *conn);
extern int	PQsetClientEncoding(PGconn *conn, const char *encoding);

#ifdef USE_SSL
/* Get the SSL structure associated with a connection */
extern SSL *PQgetssl(PGconn *conn);
#endif


/* Enable/disable tracing */
extern void PQtrace(PGconn *conn, FILE *debug_port);
extern void PQuntrace(PGconn *conn);

/* Override default notice processor */
extern PQnoticeProcessor PQsetNoticeProcessor(PGconn *conn,
					 PQnoticeProcessor proc,
					 void *arg);

/* === in fe-exec.c === */

/* Quoting strings before inclusion in queries. */
extern size_t PQescapeString(char *to, const char *from, size_t length);
extern unsigned char *PQescapeBytea(unsigned char *bintext, size_t binlen,
			  size_t *bytealen);

/* Simple synchronous query */
extern PGresult *PQexec(PGconn *conn, const char *query);
extern PGnotify *PQnotifies(PGconn *conn);
extern void PQfreeNotify(PGnotify *notify);

/* Interface for multiple-result or asynchronous queries */
extern int	PQsendQuery(PGconn *conn, const char *query);
extern PGresult *PQgetResult(PGconn *conn);

/* Routines for managing an asychronous query */
extern int	PQisBusy(PGconn *conn);
extern int	PQconsumeInput(PGconn *conn);

/* Routines for copy in/out */
extern int	PQgetline(PGconn *conn, char *string, int length);
extern int	PQputline(PGconn *conn, const char *string);
extern int	PQgetlineAsync(PGconn *conn, char *buffer, int bufsize);
extern int	PQputnbytes(PGconn *conn, const char *buffer, int nbytes);
extern int	PQendcopy(PGconn *conn);

/* Set blocking/nonblocking connection to the backend */
extern int	PQsetnonblocking(PGconn *conn, int arg);
extern int	PQisnonblocking(const PGconn *conn);

/* Force the write buffer to be written (or at least try) */
extern int	PQflush(PGconn *conn);

/*
 * "Fast path" interface --- not really recommended for application
 * use
 */
extern PGresult *PQfn(PGconn *conn,
	 int fnid,
	 int *result_buf,
	 int *result_len,
	 int result_is_int,
	 const PQArgBlock *args,
	 int nargs);

/* Accessor functions for PGresult objects */
extern ExecStatusType PQresultStatus(const PGresult *res);
extern char *PQresStatus(ExecStatusType status);
extern char *PQresultErrorMessage(const PGresult *res);
extern int	PQntuples(const PGresult *res);
extern int	PQnfields(const PGresult *res);
extern int	PQbinaryTuples(const PGresult *res);
extern char *PQfname(const PGresult *res, int field_num);
extern int	PQfnumber(const PGresult *res, const char *field_name);
extern Oid	PQftype(const PGresult *res, int field_num);
extern int	PQfsize(const PGresult *res, int field_num);
extern int	PQfmod(const PGresult *res, int field_num);
extern char *PQcmdStatus(PGresult *res);
extern char *PQoidStatus(const PGresult *res);	/* old and ugly */
extern Oid	PQoidValue(const PGresult *res);	/* new and improved */
extern char *PQcmdTuples(PGresult *res);
extern char *PQgetvalue(const PGresult *res, int tup_num, int field_num);
extern int	PQgetlength(const PGresult *res, int tup_num, int field_num);
extern int	PQgetisnull(const PGresult *res, int tup_num, int field_num);

/* Delete a PGresult */
extern void PQclear(PGresult *res);

/*
 * Make an empty PGresult with given status (some apps find this
 * useful). If conn is not NULL and status indicates an error, the
 * conn's errorMessage is copied.
 */
extern PGresult *PQmakeEmptyPGresult(PGconn *conn, ExecStatusType status);

/* === in fe-print.c === */

extern void
PQprint(FILE *fout,				/* output stream */
		const PGresult *res,
		const PQprintOpt *ps);	/* option structure */

/*
 * really old printing routines
 */
extern void
PQdisplayTuples(const PGresult *res,
				FILE *fp,		/* where to send the output */
				int fillAlign,	/* pad the fields with spaces */
				const char *fieldSep,	/* field separator */
				int printHeader,	/* display headers? */
				int quiet);

extern void
PQprintTuples(const PGresult *res,
			  FILE *fout,		/* output stream */
			  int printAttName, /* print attribute names */
			  int terseOutput,	/* delimiter bars */
			  int width);		/* width of column, if 0, use variable
								 * width */


/* === in fe-lobj.c === */

/* Large-object access routines */
extern int	lo_open(PGconn *conn, Oid lobjId, int mode);
extern int	lo_close(PGconn *conn, int fd);
extern int	lo_read(PGconn *conn, int fd, char *buf, size_t len);
extern int	lo_write(PGconn *conn, int fd, char *buf, size_t len);
extern int	lo_lseek(PGconn *conn, int fd, int offset, int whence);
extern Oid	lo_creat(PGconn *conn, int mode);
extern int	lo_tell(PGconn *conn, int fd);
extern int	lo_unlink(PGconn *conn, Oid lobjId);
extern Oid	lo_import(PGconn *conn, const char *filename);
extern int	lo_export(PGconn *conn, Oid lobjId, const char *filename);

/* === in fe-misc.c === */

/* Determine length of multibyte encoded char at *s */
extern int	PQmblen(const unsigned char *s, int encoding);

/* Get encoding id from environment variable PGCLIENTENCODING */
extern int	PQenv2encoding(void);

#ifdef __cplusplus
}
#endif

#endif   /* LIBPQ_FE_H */
