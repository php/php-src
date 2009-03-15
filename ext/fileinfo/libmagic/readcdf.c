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
FILE_RCSID("@(#)$File: readcdf.c,v 1.11 2009/02/03 20:27:51 christos Exp $")
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
					if (file_printf(ms, ", %s: %.*s", buf,
					    len, s) == -1)
						return -1;
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
				if (tp < 1000000000000000LL) {
					char tbuf[64];
					cdf_print_elapsed_time(tbuf,
					    sizeof(tbuf), tp);
					if (NOTMIME(ms) && file_printf(ms,
					    ", %s: %s", buf, tbuf) == -1)
						return -1;
				} else {
					char *c, *ec;
					cdf_timestamp_to_timespec(&ts, tp);
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
			file_error(ms, 0, "Internal parsing error");
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

	if (cdf_unpack_summary_info(sst, &si, &info, &count) == -1) {
		if (si.si_byte_order != 0xfffe)
			return 0;
		else
			return -1;
	}

	if (si.si_byte_order != 0xfffe)
		return 0;

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
	cdf_header_t h;
	cdf_sat_t sat, ssat;
	cdf_stream_t sst, scn;
	cdf_dir_t dir;
	int i;
	(void)&nbytes;
	(void)&buf;

	if (ms->flags & MAGIC_APPLE)
		return 0;
	if (cdf_read_header(fd, &h) == -1)
		return 0;
#ifdef CDF_DEBUG
	cdf_dump_header(&h);
#endif

	if (cdf_read_sat(fd, &h, &sat) == -1) {
		file_error(ms, errno, "Can't read SAT");
		return -1;
	}
#ifdef CDF_DEBUG
	cdf_dump_sat("SAT", &h, &sat);
#endif

	if ((i = cdf_read_ssat(fd, &h, &sat, &ssat)) == -1) {
		file_error(ms, errno, "Can't read SAT");
		goto out1;
	}
#ifdef CDF_DEBUG
	cdf_dump_sat("SSAT", &h, &ssat);
#endif

	if ((i = cdf_read_dir(fd, &h, &sat, &dir)) == -1) {
		file_error(ms, errno, "Can't read directory");
		goto out2;
	}

	if ((i = cdf_read_short_stream(fd, &h, &sat, &dir, &sst)) == -1) {
		file_error(ms, errno, "Cannot read short stream");
		goto out3;
	}

#ifdef CDF_DEBUG
	cdf_dump_dir(fd, &h, &sat, &ssat, &sst, &dir);
#endif
	if ((i = cdf_read_summary_info(fd, &h, &sat, &ssat, &sst, &dir, &scn))
	    == -1) {
		/* Some files don't have summary info! */
#ifdef notyet
		file_error(ms, errno, "Can't read summary_info");
#else
		i = 0;
#endif
		goto out4;
	}
#ifdef CDF_DEBUG
	cdf_dump_summary_info(&h, &scn);
#endif
	if ((i = cdf_file_summary_info(ms, &scn)) == -1)
		file_error(ms, errno, "Can't expand summary_info");
	free(scn.sst_tab);
out4:
	free(sst.sst_tab);
out3:
	free(dir.dir_tab);
out2:
	free(ssat.sat_tab);
out1:
	free(sat.sat_tab);
	return i;
}
