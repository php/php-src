/*
 * Copyright (c) 1991, 1992, 1993 Brad Eacker,
 *              (Music, Intuition, Software, and Computers)
 * All Rights Reserved
 */

#include <stdio.h>
#include <fcntl.h>

#include "dbf.h"
#include "dbf_ndx.h"

/*
 * get the ndx header for this file
 */
ndx_header_t *ndx_get_header(int fd)
{
	dndx_header_t	*dp;
	ndx_header_t	*np;

	if ((dp = (dndx_header_t *)malloc(NDX_PAGE_SZ)) == NULL)
		return NULL;
	if ((np = (ndx_header_t *)malloc(sizeof(ndx_header_t))) == NULL) {
		free(dp);
		return NULL;
	}
	if ((lseek(fd, 0, 0) < 0) || (read(fd, dp, NDX_PAGE_SZ) < 0)) {
		free(dp); free(np);
		return NULL;
	}
	np->ndx_hpage = dp;
	np->ndx_fd = fd;
	np->ndx_start_pg = get_long(dp->dndx_st_pg);
	np->ndx_total_pgs = get_long(dp->dndx_tot_pg);
	np->ndx_key_len = get_short(dp->dndx_key_len);
	np->ndx_keys_ppg = get_short(dp->dndx_keys_ppg);
	np->ndx_key_type = get_short(dp->dndx_key_type);
	np->ndx_key_size = get_long(dp->dndx_size_key);
	np->ndx_key_name = dp->dndx_key_name;
	np->ndx_unique = dp->dndx_unique;

	np->ndx_fp = NULL;

	return np;
}

static ndx_page_t *ndx_get_page(ndx_header_t *hp, int pageno)
{
	ndx_page_t	*fp;
	dndx_page_t	*dp;
	ndx_record_t	*rp;

#if PHP_DEBUG
	printf("getting page %d", pageno);
#endif
	if ((fp = (ndx_page_t *)malloc(sizeof(ndx_page_t))) == NULL)
		return NULL;
	if ((dp = (dndx_page_t *)malloc(NDX_PAGE_SZ)) == NULL) {
		free(fp);
		return NULL;
	}
	if ((rp = (ndx_record_t *)malloc(sizeof(ndx_record_t) * hp->ndx_keys_ppg)) == NULL) {
		free(dp); free(fp);
		return NULL;
	}
	fp->ndxp_page_data = dp;
	if ((lseek(hp->ndx_fd, pageno * NDX_PAGE_SZ, 0) < 0) ||
		(read(hp->ndx_fd, dp, NDX_PAGE_SZ) < 0)) {
		free(fp); free(dp);
		return NULL;
	}
	fp->ndxp_parent = NULL;
	fp->ndxp_page_no = pageno;
	fp->ndxp_num_keys = get_long(dp->dndxp_num_keys);
	memset(rp, 0, sizeof(ndx_record_t) * hp->ndx_keys_ppg);
	fp->ndxp_records = rp;
	fp->ndxp_header_p = hp;
#if PHP_DEBUG
	printf(", n_keys %ld\n", fp->ndxp_num_keys);
#endif
	return fp;
}

/*
 * get the first entry for this ndx
 */
static ndx_page_t *ndx_get_first_pg(ndx_header_t *hp)
{
	ndx_page_t	*fp;

	if (hp->ndx_fp)
		return hp->ndx_fp;
	if ((fp = ndx_get_page(hp, hp->ndx_start_pg))) {
		hp->ndx_fp = fp;
	}
	return fp;
}

static ndx_record_t *ndx_get_record(ndx_page_t *fp, int rec_no)
{
	ndx_record_t		*rp;
	ndx_header_t		*hp = fp->ndxp_header_p;
	struct dndx_record	*drp;

#if PHP_DEBUG
	printf("page %ld, rec %d: ", fp->ndxp_page_no, rec_no);
#endif
	if (rec_no >= fp->ndxp_num_keys)
		return NULL;
	rp = &(fp->ndxp_records[rec_no]);
	if (!rp->ndxr_page) {
		rp->ndxr_page = fp;
		drp = (dndx_record_t *)((char *)&fp->ndxp_page_data->dndx_rp
				+ rec_no * hp->ndx_key_size);
		rp->ndxr_left = get_long(drp->dndx_left_pg);
		rp->ndxr_rec = get_long(drp->dndx_dbf_rec);
		rp->ndxr_key_data = &drp->dndx_key_data;
		rp->ndxr_p_nrec = rec_no;
	}
#if PHP_DEBUG
	printf("left %ld, dbf_rec %ld, data '%s'\n", rp->ndxr_left,
		rp->ndxr_rec, rp->ndxr_key_data);
#endif
	return rp;
}

static ndx_record_t *ndx_scan_down(ndx_header_t *hp, ndx_page_t *fp, int recno)
{
	ndx_page_t	*np;
	ndx_record_t	*rp;

	while ((rp = ndx_get_record(fp, recno)) && (rp->ndxr_rec == 0)) {
		np = ndx_get_page(hp, rp->ndxr_left);
		np->ndxp_parent = fp;
		np->ndxp_par_rno = recno;
		fp = np;
		recno = 0;
	}
	return rp;
}

static ndx_record_t *ndx_scan_up(ndx_header_t *hp, ndx_page_t *fp, int recno)
{
	ndx_record_t	*rp;

	if (fp == NULL)
		rp = NULL;
	else if (recno < fp->ndxp_num_keys) {
		rp = ndx_scan_down(hp, fp, recno);
	} else {
		rp = ndx_scan_up(hp, fp->ndxp_parent, fp->ndxp_par_rno + 1);
	}
	return rp;
}

ndx_record_t *ndx_get_first_rec(ndx_header_t *hp)
{
	ndx_page_t	*fp;
	ndx_record_t	*rp = NULL;

	if ((fp = ndx_get_first_pg(hp))) {
		fp->ndxp_last_key = 0;
		rp = ndx_scan_down(hp, fp, 0);
	}
	hp->ndx_cur_rec = rp;
	return rp;
}

ndx_record_t *ndx_get_next_rec(ndx_header_t *hp, ndx_record_t *rp)
{
	ndx_page_t	*fp;
	int		rec_no;

	fp = rp->ndxr_page;
	rec_no = rp->ndxr_p_nrec + 1;
	if (rec_no < fp->ndxp_num_keys) {
		rp = ndx_scan_down(hp, fp, rec_no);
	} else {
		rp = ndx_scan_up(hp, fp->ndxp_parent, fp->ndxp_par_rno + 1);
	}
	return rp;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
