/*
 * Copyright (c) 1991, 1992, 1993 Brad Eacker,
 *              (Music, Intuition, Software, and Computers)
 * All Rights Reserved
 */

/*
 * dbf header structure on disk (pc dbase III)
 *
 *  Basic info taken from:
 *	"File Formats for Popular PC Software"
 *	Jeff Walden
 *	(c) 1986 John Wiley & Sons, Inc.
 */

#ifndef	DBF_H_
#define	DBF_H_

#include <stdlib.h>
#include <string.h>
#ifdef WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

/* So we can use O_BINARY on non-Win32 systems. */
#if !defined(O_BINARY) && !defined(WIN32)
#define O_BINARY (0)
#endif

struct dbf_dhead {
	char	dbh_dbt;		/* memo (dbt) file present */
	char	dbh_date[3];		/* last update YY, MM, DD */
	char	dbh_records[4];		/* number of records LE */
	char	dbh_hlen[2];		/* header length LE */
	char	dbh_rlen[2];		/* record length LE */
	char	dbh_res[20];		/* padding */
};
#define	DBH_DATE_YEAR	0		/* byte offset for year in dbh_date */
#define	DBH_DATE_MONTH	1
#define	DBH_DATE_DAY	2

/*
 * field description on disk
 */

#define	DBF_NAMELEN	11

struct dbf_dfield {
	char	dbf_name[DBF_NAMELEN];	/* name of field */
	char	dbf_type;		/* type of field */
	char	dbf_fda[4];		/* something for dbase III */
	char	dbf_flen[2];		/* field length [and decimal if N] */
	char	dbf_res[14];		/* padding */
};

struct db_field {
	char	db_fname[DBF_NAMELEN+1];	/* 0 terminated */
	char	db_type;		/* type of field */
	int	db_flen;		/* length of field */
	int	db_fdc;			/* number of decimals in field */

	char	*db_format;		/* format for printing %s etc */
	int	db_foffset;		/* offset within record */
};
typedef struct db_field	dbfield_t;

struct db_head {
	int	db_fd;
	unsigned char	db_dbt;			/* dbt present */
	char	db_date[9];		/* date of last update in db format */
	long	db_records;		/* number of records */
	int	db_hlen;		/* header length */
	int	db_rlen;		/* record length */

	int	db_nfields;		/* number of fields */
	dbfield_t	*db_fields;	/* field info */
	char	*db_name;		/* name of dbf file */
	int	db_cur_rec;		/* current record */
};
typedef struct db_head	dbhead_t;

#define	DBH_TYPE_NORMAL	0x03
#define	DBH_TYPE_MEMO	0x83

#define	VALID_RECORD	' '
#define	DELETED_RECORD	'*'

#include "dbf_head.h"
#include "dbf_misc.h"
#include "dbf_rec.h"

#endif	/* DBF_H_ */
