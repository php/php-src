/*
 * Copyright (c) 1993 Brad Eacker,
 *              (Music, Intuition, Software, and Computers)
 * All Rights Reserved
 */

/*
 * dbf .ndx header structure on disk and in memory
 *
 *  Basic info taken from:
 *	"Clipper Programming Guide, 3rd Edition, Version 5.01"
 *	by Rick Spence
 */

#ifndef DBF_NDX_H_
#define	 DBF_NDX_H_

#include "dbf.h"

#define	NDX_PAGE_SZ	512

/* on disk ndx header */
struct dndx_header {
	char	dndx_st_pg[4];		/* starting page number */
	char	dndx_tot_pg[4];		/* total number of pages */
	char	dndx_filler1[4];	/* space */
	char	dndx_key_len[2];	/* key length */
	char	dndx_keys_ppg[2];	/* number of keys per page */
	char	dndx_key_type[2];	/* key type */
	char	dndx_size_key[4];	/* size of the key record */
	char	dndx_filler2;		/* space */
	char	dndx_unique;		/* whether or not done with unique */
	char	dndx_key_name[488];	/* string defining the key */
};
typedef struct dndx_header dndx_header_t;

/* in memory ndx header */
struct ndx_header {
	long	ndx_start_pg;
	long	ndx_total_pgs;
	unsigned short	ndx_key_len;
	unsigned short	ndx_keys_ppg;
	unsigned short	ndx_key_type;
	char	ndx_unique;
	long	ndx_key_size;
	char	*ndx_key_name;
	int	ndx_fd;
	struct ndx_page	*ndx_fp;
	dndx_header_t *ndx_hpage;
	struct ndx_record *ndx_cur_rec;
};
typedef struct ndx_header ndx_header_t;

/* these are the possible values in the key type field */
#define	NDX_CHAR_TYPE	00
#define	NDX_NUM_TYPE	01

/* on disk key record */
struct dndx_record {
	char	dndx_left_pg[4];	/* number of left page */
	char	dndx_dbf_rec[4];	/* dbf record number */
	char	dndx_key_data;		/* key data */
};
typedef struct dndx_record dndx_record_t;

struct ndx_record {
	long	ndxr_left;
	long	ndxr_rec;
	char	*ndxr_key_data;
	struct ndx_page *ndxr_page;	/* page pointer to where we are from*/
	int	ndxr_p_nrec;		/* number of the record within page */
};
typedef struct ndx_record ndx_record_t;

struct dndx_page {
	char	dndxp_num_keys[4];	/* number of keys on this page */
	struct dndx_record dndx_rp;
};
typedef struct dndx_page dndx_page_t;

struct ndx_page {
	long		ndxp_page_no;
	long		ndxp_num_keys;
	dndx_page_t	*ndxp_page_data;
	ndx_header_t	*ndxp_header_p;
	long		ndxp_last_key;
	struct ndx_page	*ndxp_parent;	/* parent page */
	int		ndxp_par_rno;	/* record number within parent */
	struct ndx_record *ndxp_records;
};
typedef struct ndx_page ndx_page_t;

extern ndx_header_t *ndx_get_header(int);

extern ndx_record_t *ndx_get_first_rec(ndx_header_t *);
extern ndx_record_t *ndx_get_next_rec(ndx_header_t *, ndx_record_t *);

#endif /* DBF_NDX_H_ */
