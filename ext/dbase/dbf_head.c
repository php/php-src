/*
 * Copyright (c) 1991, 1992, 1993 Brad Eacker,
 *              (Music, Intuition, Software, and Computers)
 * All Rights Reserved
 */

#include <stdio.h>
#include <fcntl.h>

#include "php.h"
#include "dbf.h"

void free_dbf_head(dbhead_t *dbh);
int get_dbf_field(dbhead_t *dbh, dbfield_t *dbf);

/*
 * get the header info from the file
 *	basic header info & field descriptions
 */
dbhead_t *get_dbf_head(int fd)
{
	dbhead_t *dbh;
	struct dbf_dhead  dbhead;
	dbfield_t *dbf, *cur_f, *tdbf;
	int ret, nfields, offset, gf_retval;

	if ((dbh = (dbhead_t *)malloc(sizeof(dbhead_t))) == NULL)
		return NULL;
	if (lseek(fd, 0, 0) < 0)
		return NULL;
	if ((ret = read(fd, &dbhead, sizeof(dbhead))) < 0)
		return NULL;

	/* build in core info */
	dbh->db_fd = fd;
	dbh->db_dbt = dbhead.dbh_dbt;
	dbh->db_records = get_long(dbhead.dbh_records);
	dbh->db_hlen = get_short(dbhead.dbh_hlen);
	dbh->db_rlen = get_short(dbhead.dbh_rlen);

	db_set_date(dbh->db_date, dbhead.dbh_date[DBH_DATE_YEAR] + 1900,
		dbhead.dbh_date[DBH_DATE_MONTH],
		dbhead.dbh_date[DBH_DATE_DAY]);

	/* malloc enough memory for the maximum number of fields:
	   32 * 1024 = 32K dBase5 (for Win) seems to allow that many */
	tdbf = (dbfield_t *)malloc(sizeof(dbfield_t)*1024);
	
	offset = 1;
	nfields = 0;
	gf_retval = 0;
	for (cur_f = tdbf; gf_retval < 2 && nfields < 1024; cur_f++) {
		gf_retval = get_dbf_field(dbh, cur_f);

		if (gf_retval < 0) {
			free_dbf_head(dbh);
			return NULL;
		}
		if (gf_retval != 2 ) {
			cur_f->db_foffset = offset;
			offset += cur_f->db_flen;
			nfields++;
		}
	}
	dbh->db_nfields = nfields;
	
	/* malloc the right amount of space for records, copy and destroy old */
	dbf = (dbfield_t *)malloc(sizeof(dbfield_t)*nfields);
	memcpy(dbf, tdbf, sizeof(dbfield_t)*nfields);
	free(tdbf);

	dbh->db_fields = dbf;

	return dbh;
}

/*
 * free up the header info built above
 */
void free_dbf_head(dbhead_t *dbh)
{
	dbfield_t *dbf, *cur_f;
	int nfields;

	dbf = dbh->db_fields;
	nfields = dbh->db_nfields;
	for (cur_f = dbf; cur_f < &dbf[nfields]; cur_f++) {
		if (cur_f->db_format) {
			free(cur_f->db_format);
		}
	}

	free(dbf);
	free(dbh);
}

/*
 * put out the header info
 */
int put_dbf_head(dbhead_t *dbh)
{
	int fd = dbh->db_fd;
	struct dbf_dhead  dbhead;
	int	ret;

	memset (&dbhead, 0, sizeof(dbhead));

	/* build on disk info */
	dbhead.dbh_dbt = dbh->db_dbt;
	put_long(dbhead.dbh_records, dbh->db_records);
	put_short(dbhead.dbh_hlen, dbh->db_hlen);
	put_short(dbhead.dbh_rlen, dbh->db_rlen);

	/* put the date spec'd into the on disk header */
	dbhead.dbh_date[DBH_DATE_YEAR] =(char)(db_date_year(dbh->db_date) -
						1900);
	dbhead.dbh_date[DBH_DATE_MONTH]=(char)(db_date_month(dbh->db_date));
	dbhead.dbh_date[DBH_DATE_DAY] =(char)(db_date_day(dbh->db_date));

	if (lseek(fd, 0, 0) < 0)
		return -1;
	if ((ret = write(fd, &dbhead, sizeof(dbhead))) < 0)
		return -1;
	return ret;
}

/*
 * get a field off the disk from the current file offset
 */
int get_dbf_field(dbhead_t *dbh, dbfield_t *dbf)
{
	struct dbf_dfield	dbfield;
	int ret;

	if ((ret = read(dbh->db_fd, &dbfield, sizeof(dbfield))) < 0) {
		return ret;
	}

	/* Check for the '0Dh' field terminator , if found return '2'
	   which will tell the loop we are at the end of fields */
	if (dbfield.dbf_name[0]==0x0d) {
		return 2;
	}

	/* build the field name */
	copy_crimp(dbf->db_fname, dbfield.dbf_name, DBF_NAMELEN);

	dbf->db_type = dbfield.dbf_type;
	switch (dbf->db_type) {
	    case 'N':
		dbf->db_flen = dbfield.dbf_flen[0];
		dbf->db_fdc = dbfield.dbf_flen[1];
		break;
	    default:
	    	dbf->db_flen = get_short(dbfield.dbf_flen);
		break;
	}

	if ((dbf->db_format = get_dbf_f_fmt(dbf)) == NULL) {
		return 1;
	}

	return 0;
}

/*
 * put a field out on the disk at the current file offset
 */
int put_dbf_field(dbhead_t *dbh, dbfield_t *dbf)
{
	struct dbf_dfield	dbfield;
	char			*scp, *dcp;
	int			ret;

	memset (&dbfield, 0, sizeof(dbfield));

	/* build the on disk field info */
	scp = dbf->db_fname; dcp = dbfield.dbf_name;

	strncpy(dbfield.dbf_name, dbf->db_fname, DBF_NAMELEN);

	dbfield.dbf_type = dbf->db_type;
	switch (dbf->db_type) {
	    case 'N':		
		dbfield.dbf_flen[0] = dbf->db_flen;
		dbfield.dbf_flen[1] = dbf->db_fdc;
		break;
	    default:
	    	put_short(dbfield.dbf_flen, dbf->db_flen);
	}

	/* now write it out to disk */
	if ((ret = write(dbh->db_fd, &dbfield, sizeof(dbfield))) < 0) {
		return ret;
	}
	return 1;
}

/*
 * put out all the info at the top of the file...
 */
static char end_stuff[2] = {0x0d, 0};

void put_dbf_info(dbhead_t *dbh)
{
	dbfield_t	*dbf;
	char		*cp;
	int		fcnt;

	if ((cp = db_cur_date(NULL))) {
		strncpy(dbh->db_date, cp, 8);
		free(cp);
	}
	put_dbf_head(dbh);
	dbf = dbh->db_fields;
	for (fcnt = dbh->db_nfields; fcnt > 0; fcnt--, dbf++)
		put_dbf_field(dbh, dbf);
	write(dbh->db_fd, end_stuff, 1);
}

char *get_dbf_f_fmt(dbfield_t *dbf)
{
	char format[100];

	/* build the field format for printf */
	switch (dbf->db_type) {
	   case 'C':
		sprintf(format, "%%-%ds", dbf->db_flen);
		break;
	   case 'N':
	   case 'L':
	   case 'D':
		sprintf(format, "%%%ds", dbf->db_flen);
		break;
	   case 'M':
		strcpy(format, "%s");
		break;
	}
	return (char *)strdup(format);
}

dbhead_t *dbf_open(char *dp, int o_flags TSRMLS_DC)
{
	int fd;
	char *cp;
	dbhead_t *dbh;

	cp = dp;
	if ((fd = VCWD_OPEN(cp, o_flags|O_BINARY)) < 0) {
		cp = (char *)malloc(256);
		strcpy(cp, dp); strcat(cp, ".dbf");
		if ((fd = VCWD_OPEN(cp, o_flags)) < 0) {
			perror("open");
			return NULL;
		}
	}

	if ((dbh = get_dbf_head(fd)) ==	 0) {
		fprintf(stderr, "Unable to get header\n");
		return NULL;
	}
	dbh->db_name = cp;
	dbh->db_cur_rec = 0;
	
	return dbh;
}

void dbf_head_info(dbhead_t *dbh)
{
	int	nfields;
	dbfield_t *dbf, *cur_f;

	nfields = dbh->db_nfields;
	printf("# fields: %d, record len: %d, total records %ld\n",
		nfields, dbh->db_rlen, dbh->db_records);
	dbf = dbh->db_fields;
	for (cur_f = dbf; cur_f < &dbf[nfields] ; cur_f++) {
		printf("# %s, %c, %d, %d\n", cur_f->db_fname,
			cur_f->db_type, cur_f->db_flen, cur_f->db_fdc);
	}
}
 
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
