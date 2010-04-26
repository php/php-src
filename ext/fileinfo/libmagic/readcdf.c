/*-
 * Copyright (c) 2008 Christos Zoulas
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include "file.h"

#ifndef lint
FILE_RCSID("@(#)$File: readcdf.c,v 1.18 2009/05/06 20:48:22 christos Exp $")
#endif

#include <stdlib.h>
#ifdef PHP_WIN32
#include "win32/unistd.h"
#else
#include <unistd.h>
#endif
#include <string.h>
#include <time.h>
#include <ctype.h>

#include "cdf.h"
#include "magic.h"

#define NOTMIME(ms) (((ms)->flags & MAGIC_MIME) == 0)

private int
cdf_file_property_info(struct magic_set *ms, const cdf_property_info_t *info,
    size_t count)
{
	size_t i;
	cdf_timestamp_t tp;
	struct timeval ts;
	char buf[64];
	const char *str = "vnd.ms-office";
	const char *s;
	int len;

	for (i = 0; i < count; i++) {
		cdf_print_property_name(buf, sizeof(buf), info[i].pi_id);
		switch (info[i].pi_type) {
		case CDF_SIGNED16:
			if (NOTMIME(ms) && file_printf(ms, ", %s: %hd", buf,
			    info[i].pi_s16) == -1)
				return -1;
			break;
		case CDF_SIGNED32:
			if (NOTMIME(ms) && file_printf(ms, ", %s: %d", buf,
			    info[i].pi_s32) == -1)
				return -1;
			break;
		case CDF_UNSIGNED32:
			if (NOTMIME(ms) && file_printf(ms, ", %s: %u", buf,
			    info[i].pi_u32) == -1)
				return -1;
			break;
		case CDF_LENGTH32_STRING:
			len = info[i].pi_str.s_len;
			if (len > 1) {
				s = info[i].pi_str.s_buf;
				if (NOTMIME(ms)) {
					char vbuf[1024];
					size_t j;
					for (j = 0; j < sizeof(vbuf) && len--;
					    j++, s++) {
						if (*s == '\0')
							break;
						if (isprint((unsigned char)*s))
							vbuf[j] = *s;
					}
					if (j == sizeof(vbuf))
						--j;
					vbuf[j] = '\0';
					if (vbuf[0]) {
						if (file_printf(ms, ", %s: %s",
						    buf, vbuf) == -1)
							return -1;
					}
				} else if (info[i].pi_id == 
					CDF_PROPERTY_NAME_OF_APPLICATION) {
					if (strstr(s, "Word"))
						str = "msword";
					else if (strstr(s, "Excel"))
						str = "vnd.ms-excel";
					else if (strstr(s, "Powerpoint"))
						str = "vnd.ms-powerpoint";
				}
			}
			break;
		case CDF_FILETIME:
			tp = info[i].pi_tp;
			if (tp != 0) {
#if defined(PHP_WIN32) && _MSC_VER <= 1500
				if (tp < 1000000000000000i64) {
#else
				if (tp < 1000000000000000LL) {
#endif
					char tbuf[64];
					cdf_print_elapsed_time(tbuf,
					    sizeof(tbuf), tp);
					if (NOTMIME(ms) && file_printf(ms,
					    ", %s: %s", buf, tbuf) == -1)
						return -1;
				} else {
					char *c, *ec;

					if (cdf_timestamp_to_timespec(&ts, tp) == -1) {
						return -1;
					}
					c = ctime(&ts.tv_sec);
					if ((ec = strchr(c, '\n')) != NULL)
						*ec = '\0';

					if (NOTMIME(ms) && file_printf(ms,
					    ", %s: %s", buf, c) == -1)
						return -1;
				}
			}
			break;
		case CDF_CLIPBOARD:
			break;
		default:
			return -1;
		}
	}
	if (!NOTMIME(ms)) {
		if (file_printf(ms, "application/%s", str) == -1)
			return -1;
	}
	return 1;
}

private int
cdf_file_summary_info(struct magic_set *ms, const cdf_stream_t *sst)
{
	cdf_summary_info_header_t si;
	cdf_property_info_t *info;
	size_t count;
	int m;

	if (cdf_unpack_summary_info(sst, &si, &info, &count) == -1)
		return -1;

	if (NOTMIME(ms)) {
		if (file_printf(ms, "CDF V2 Document") == -1)
			return -1;

		if (file_printf(ms, ", %s Endian",
		    si.si_byte_order == 0xfffe ?  "Little" : "Big") == -1)
			return -1;
		switch (si.si_os) {
		case 2:
			if (file_printf(ms, ", Os: Windows, Version %d.%d",
			    si.si_os_version & 0xff, si.si_os_version >> 8)
			    == -1)
				return -1;
			break;
		case 1:
			if (file_printf(ms, ", Os: MacOS, Version %d.%d",
			    si.si_os_version >> 8, si.si_os_version & 0xff)
			    == -1)
				return -1;
			break;
		default:
			if (file_printf(ms, ", Os %d, Version: %d.%d", si.si_os,
			    si.si_os_version & 0xff, si.si_os_version >> 8)
			    == -1)
				return -1;
			break;
		}
	}

	m = cdf_file_property_info(ms, info, count);
	free(info);

	return m;
}

protected int
file_trycdf(struct magic_set *ms, int fd, const unsigned char *buf,
    size_t nbytes)
{
	cdf_info_t info;
	cdf_header_t h;
	cdf_sat_t sat, ssat;
	cdf_stream_t sst, scn;
	cdf_dir_t dir;
	int i;
	const char *expn = "";

	info.i_fd = fd;
	info.i_buf = buf;
	info.i_len = nbytes;
	if (ms->flags & MAGIC_APPLE)
		return 0;
	if (cdf_read_header(&info, &h) == -1)
		return 0;
#ifdef CDF_DEBUG
	cdf_dump_header(&h);
#endif

	if ((i = cdf_read_sat(&info, &h, &sat)) == -1) {
		expn = "Can't read SAT";
		goto out0;
	}
#ifdef CDF_DEBUG
	cdf_dump_sat("SAT", &sat, CDF_SEC_SIZE(&h));
#endif

	if ((i = cdf_read_ssat(&info, &h, &sat, &ssat)) == -1) {
		expn = "Can't read SSAT";
		goto out1;
	}
#ifdef CDF_DEBUG
	cdf_dump_sat("SSAT", &ssat, CDF_SHORT_SEC_SIZE(&h));
#endif

	if ((i = cdf_read_dir(&info, &h, &sat, &dir)) == -1) {
		expn = "Can't read directory";
		goto out2;
	}

	if ((i = cdf_read_short_stream(&info, &h, &sat, &dir, &sst)) == -1) {
		expn = "Cannot read short stream";
		goto out3;
	}
#ifdef CDF_DEBUG
	cdf_dump_dir(&info, &h, &sat, &ssat, &sst, &dir);
#endif

	if ((i = cdf_read_summary_info(&info, &h, &sat, &ssat, &sst, &dir,
	    &scn)) == -1) {
		expn = "Cannot read summary info";
		goto out4;
	}
#ifdef CDF_DEBUG
	cdf_dump_summary_info(&h, &scn);
#endif
	if ((i = cdf_file_summary_info(ms, &scn)) == -1)
		expn = "Can't expand summary_info";
	free(scn.sst_tab);
out4:
	free(sst.sst_tab);
out3:
	free(dir.dir_tab);
out2:
	free(ssat.sat_tab);
out1:
	free(sat.sat_tab);
out0:
	if (i != 1) {
		if (file_printf(ms, "CDF V2 Document") == -1)
			return -1;
		if (*expn)
			if (file_printf(ms, ", corrupt: %s", expn) == -1)
				return -1;
		i = 1;
	}
	return i;
}
