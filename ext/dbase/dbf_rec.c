/*
 * Copyright (c) 1993 Brad Eacker,
 *              (Music, Intuition, Software, and Computers)
 * All Rights Reserved
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"

#include "ext/standard/flock_compat.h" 

#include <stdio.h>
#include <fcntl.h>

#include "dbf.h"
 
int get_piece(dbhead_t *dbh, long offset, char *cp, int len);
int put_piece(dbhead_t *dbh, long offset, char *cp, int len);

/*
 * get a record off the database
 */
char *get_dbf_record(dbhead_t *dbh, long rec_num)
{
	long	offset;
	char	*cp;

	if (rec_num > dbh->db_records) {
		return NULL;
	}
	if ((cp = (char *)malloc(dbh->db_rlen)) == NULL) {
		return NULL;
	}

	/* go to the correct spot on the file */
	offset = dbh->db_hlen + (rec_num - 1) * dbh->db_rlen;
	if (get_piece(dbh, offset, cp, dbh->db_rlen) != dbh->db_rlen) {
		free(cp);
		cp = NULL;
	}
	if (cp)
		dbh->db_cur_rec = rec_num;
	return cp;
}

int
get_piece(dbhead_t *dbh, long offset, char *cp, int len)
{
	/* go to the correct spot on the file */
	if ( lseek(dbh->db_fd, offset, 0) < 0 ) {
		return -1;
	}

	/* read the record into the allocated space */
	return read(dbh->db_fd, cp, len);
}

/*
 * put a record to the database
 */
long put_dbf_record(dbhead_t *dbh, long rec_num, char *cp)
{
	long	offset;

	if (rec_num == 0) {
		rec_num = dbh->db_records;
	}
	if (rec_num > dbh->db_records) {
		return 0L;
	}
	/* go to the correct spot on the file */
	offset = dbh->db_hlen + (rec_num - 1) * dbh->db_rlen;
	if (put_piece(dbh, offset, cp, dbh->db_rlen) != dbh->db_rlen) {
		rec_num = -1;
	}
	return rec_num;
}

int put_piece(dbhead_t *dbh, long offset, char *cp, int len)
{
	/* go to the correct spot on the file */
	if ( lseek(dbh->db_fd, offset, 0) < 0 ) {
		return -1;
	}

	/* write the record into the file */
	return write(dbh->db_fd, cp, len);
}

int del_dbf_record(dbhead_t *dbh, long rec_num)
{
	int ret = 0;
	char *cp;

	if (rec_num > dbh->db_records)
		return -1;
	if ((cp = get_dbf_record(dbh, rec_num))) {
		*cp = DELETED_RECORD;
		ret = put_dbf_record(dbh, rec_num, cp);
		free(cp);
	}
	return ret;
}

void pack_dbf(dbhead_t *dbh)
{
	long	out_off, in_off;
	int	rec_cnt, new_cnt;
	char	*cp;

	if ((cp = (char *)malloc(dbh->db_rlen)) == NULL) {
		return;
	}
	in_off = out_off = dbh->db_hlen;

	new_cnt = 0;
	rec_cnt = dbh->db_records;
	while (rec_cnt > 0) {
		if (get_piece(dbh, in_off, cp, dbh->db_rlen) < 0)
			break;

		if (*cp != DELETED_RECORD) {
			/* write the record into the file */
			if (put_piece(dbh, out_off, cp, dbh->db_rlen) < 0)
				break;
			out_off += dbh->db_rlen;
			new_cnt++;
		}
		in_off += dbh->db_rlen;
		rec_cnt--;
	}
	free(cp);

	/* Try to truncate the file to the right size. */
	if (ftruncate(dbh->db_fd, out_off) != 0) {
	    php_error(E_WARNING, "dbase_pack() couldn't truncate the file to the right size. Some deleted records may still be left in there.");
	}

	if (rec_cnt == 0)
		dbh->db_records = new_cnt;
}

/* routine to get a field from a record */
char *get_field_val(char *rp, dbfield_t *fldp, char *cp)
{
	int flen = fldp->db_flen;

	if ( !cp )
		cp = (char *)malloc(flen + 1);
	if ( cp ) {
		strncpy(cp, &rp[fldp->db_foffset], flen);
		cp[flen] = 0;
	}
	return cp;
}

void put_field_val(char *rp, dbfield_t *fldp, char *cp)
{
	strncpy(&rp[fldp->db_foffset], cp, fldp->db_flen);
}

/*
 * output a record
 */
void out_rec(dbhead_t *dbh, dbfield_t *dbf, char *cp)
{
        dbfield_t       *cur_f;
        int     nfields = dbh->db_nfields;
        char    *fnp = (char *)malloc(dbh->db_rlen);

        printf("%c", *cp);
        for (cur_f = dbf; cur_f < &dbf[nfields] ; cur_f++) {
                printf(" ");
		printf(cur_f->db_format, get_field_val(cp, cur_f, fnp));
        }
        printf("\n");
	free(fnp);
}

/* check for record validity */
int is_valid_rec(char *cp)
{
	if (cp && (*cp == VALID_RECORD))
		return 1;
	else
		return 0;
}

/* get the next record */
char *dbf_get_next(dbhead_t *dbh)
{
	return get_dbf_record(dbh, dbh->db_cur_rec + 1);
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
