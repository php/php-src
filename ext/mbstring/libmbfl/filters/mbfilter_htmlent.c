/*
 * "streamable kanji code filter and converter"
 * Copyright (c) 1998-2002 HappySize, Inc. All rights reserved.
 *
 * LICENSE NOTICES
 *
 * This file is part of "streamable kanji code filter and converter",
 * which is distributed under the terms of GNU Lesser General Public
 * License (version 2) as published by the Free Software Foundation.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with "streamable kanji code filter and converter";
 * if not, write to the Free Software Foundation, Inc., 59 Temple Place,
 * Suite 330, Boston, MA  02111-1307  USA
 *
 * The author of this part: Marcus Boerger <helly@php.net>
 *
 */
/*
 * The source code included in this files was separated from mbfilter.c
 * by moriyoshi koizumi <moriyoshi@php.net> on 4 dec 2002.
 *
 */

#include <string.h>
#include "mbfilter.h"
#include "mbfilter_htmlent.h"
#include "html_entities.h"

static size_t mb_htmlent_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state);
static void mb_wchar_to_htmlent(uint32_t *in, size_t len, mb_convert_buf *buf, bool end);

static const int htmlentitifieds[256] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 2, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

static const char *mbfl_encoding_html_ent_aliases[] = {"HTML", "html", NULL};

const mbfl_encoding mbfl_encoding_html_ent = {
	mbfl_no_encoding_html_ent,
	"HTML-ENTITIES",
	"HTML-ENTITIES",
	mbfl_encoding_html_ent_aliases,
	NULL,
	MBFL_ENCTYPE_GL_UNSAFE,
	&vtbl_html_wchar,
	&vtbl_wchar_html,
	mb_htmlent_to_wchar,
	mb_wchar_to_htmlent,
	NULL,
	NULL
};

const struct mbfl_convert_vtbl vtbl_wchar_html = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_html_ent,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_html_enc,
	mbfl_filt_conv_html_enc_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_html_wchar = {
	mbfl_no_encoding_html_ent,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_html_dec_ctor,
	mbfl_filt_conv_html_dec_dtor,
	mbfl_filt_conv_html_dec,
	mbfl_filt_conv_html_dec_flush,
	mbfl_filt_conv_html_dec_copy,
};


#define CK(statement)	do { if ((statement) < 0) return (-1); } while (0)

/*
 * any => HTML
 */
int mbfl_filt_conv_html_enc(int c, mbfl_convert_filter *filter)
{
	int tmp[64];
	int i;
	unsigned int uc;
	const mbfl_html_entity_entry *e;

	if (c < sizeof(htmlentitifieds) / sizeof(htmlentitifieds[0]) &&
				htmlentitifieds[c] != 1) {
		CK((*filter->output_function)(c, filter->data));
	} else {
 		CK((*filter->output_function)('&', filter->data));
		for (i = 0; (e = &mbfl_html_entity_list[i])->name != NULL; i++) {
			if (c == e->code) {
				char *p;

				for (p = e->name; *p != '\0'; p++) {
					CK((*filter->output_function)((int)*p, filter->data));
				}
				goto last;
			}
		}

		{
			int *p = tmp + sizeof(tmp) / sizeof(tmp[0]);

			CK((*filter->output_function)('#', filter->data));

			uc = (unsigned int)c;

			*(--p) = '\0';
			do {
				*(--p) = "0123456789"[uc % 10];
				uc /= 10;
			} while (uc);

			for (; *p != '\0'; p++) {
				CK((*filter->output_function)(*p, filter->data));
			}
		}
	last:
		CK((*filter->output_function)(';', filter->data));
	}
	return 0;
}

int mbfl_filt_conv_html_enc_flush(mbfl_convert_filter *filter)
{
	filter->status = 0;
	filter->opaque = NULL;

	if (filter->flush_function != NULL) {
		(*filter->flush_function)(filter->data);
	}

	return 0;
}

/*
 * HTML => any
 */
#define html_enc_buffer_size	16
static const char html_entity_chars[] = "#0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

void mbfl_filt_conv_html_dec_ctor(mbfl_convert_filter *filter)
{
	filter->status = 0;
	filter->opaque = emalloc(html_enc_buffer_size+1);
}

void mbfl_filt_conv_html_dec_dtor(mbfl_convert_filter *filter)
{
	filter->status = 0;
	if (filter->opaque)
	{
		efree((void*)filter->opaque);
	}
	filter->opaque = NULL;
}

int mbfl_filt_conv_html_dec(int c, mbfl_convert_filter *filter)
{
	int pos;
	unsigned int ent = 0;
	mbfl_html_entity_entry *entity;
	unsigned char *buffer = (unsigned char*)filter->opaque;

	if (!filter->status) {
		if (c == '&' ) {
			filter->status = 1;
			buffer[0] = '&';
		} else {
			CK((*filter->output_function)(c, filter->data));
		}
	} else {
		if (c == ';') {
			if (buffer[1]=='#') {
				if (filter->status > 2 && (buffer[2] == 'x' || buffer[2] == 'X')) {
					if (filter->status > 3) {
						/* numeric entity */
						for (pos=3; pos<filter->status; pos++) {
							int v = buffer[pos];
							if (v >= '0' && v <= '9') {
								v = v - '0';
							} else if (v >= 'A' && v <= 'F') {
								v = v - 'A' + 10;
							} else if (v >= 'a' && v <= 'f') {
								v = v - 'a' + 10;
							} else {
								ent = -1;
								break;
							}
							ent = ent * 16 + v;
						}
					} else {
						ent = -1;
					}
				} else {
					/* numeric entity */
					if (filter->status > 2) {
						for (pos=2; pos<filter->status; pos++) {
							if (ent > 0x19999999) {
								ent = -1;
								break;
							}
							int v = buffer[pos];
							if (v >= '0' && v <= '9') {
								v = v - '0';
							} else {
								ent = -1;
								break;
							}
							ent = ent*10 + v;
						}
					} else {
						ent = -1;
					}
				}
				if (ent < 0x110000) {
					CK((*filter->output_function)(ent, filter->data));
				} else {
					for (pos = 0; pos < filter->status; pos++) {
						CK((*filter->output_function)(buffer[pos], filter->data));
					}
					CK((*filter->output_function)(c, filter->data));
				}
				filter->status = 0;
			} else {
				/* named entity */
				buffer[filter->status] = 0;
				entity = (mbfl_html_entity_entry *)mbfl_html_entity_list;
				while (entity->name) {
					if (!strcmp((const char*)buffer+1, entity->name)) {
						ent = entity->code;
						break;
					}
					entity++;
				}
				if (ent) {
					/* decoded */
					CK((*filter->output_function)(ent, filter->data));
					filter->status = 0;

				} else {
					/* failure */
					buffer[filter->status++] = ';';
					buffer[filter->status] = 0;

					/* flush fragments */
					pos = 0;
					while (filter->status--) {
						int e = (*filter->output_function)(buffer[pos++], filter->data);
						if (e != 0)
							return e;
					}
					filter->status = 0;
				}
			}
		} else {
			/* add character */
			buffer[filter->status++] = c;
			/* add character and check */
			if (!strchr(html_entity_chars, c) || filter->status+1==html_enc_buffer_size || (c=='#' && filter->status>2))
			{
				/* illegal character or end of buffer */
				if (c=='&')
					filter->status--;
				buffer[filter->status] = 0;

				pos = 0;
				while (filter->status--) {
					int e = (*filter->output_function)(buffer[pos++], filter->data);
					if (e != 0)
						return e;
				}
				filter->status = 0;

				if (c=='&')
				{
					buffer[filter->status++] = '&';
				}
			}
		}
	}
	return 0;
}

int mbfl_filt_conv_html_dec_flush(mbfl_convert_filter *filter)
{
	int status, pos = 0;
	unsigned char *buffer;
	int err = 0;

	buffer = (unsigned char*)filter->opaque;
	status = filter->status;
	filter->status = 0;

	/* flush fragments */
	while (status--) {
		int e = (*filter->output_function)(buffer[pos++], filter->data);
		if (e != 0)
			err = e;
	}

	if (filter->flush_function != NULL) {
		(*filter->flush_function)(filter->data);
	}

	return err;
}

void mbfl_filt_conv_html_dec_copy(mbfl_convert_filter *src, mbfl_convert_filter *dest)
{
	*dest = *src;
	dest->opaque = emalloc(html_enc_buffer_size+1);
	memcpy(dest->opaque, src->opaque, html_enc_buffer_size+1);
}

static bool is_html_entity_char(unsigned char c)
{
	return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '#';
}

static size_t mb_htmlent_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
{
	unsigned char *p = *in, *e = p + *in_len;
	uint32_t *out = buf, *limit = buf + bufsize;

	while (p < e && out < limit) {
		unsigned char c = *p++;

		if (c == '&') {
			/* Find terminating ; for HTML entity */
			unsigned char *terminator = p;
			while (terminator < e && is_html_entity_char(*terminator))
				terminator++;
			if (terminator < e && *terminator == ';') {
				if (*p == '#' && (e - p) >= 2) {
					/* Numeric entity */
					unsigned int value = 0;
					unsigned char *digits = p + 1;
					if (*digits == 'x' || *digits == 'X') {
						/* Hexadecimal */
						digits++;
						if (digits == terminator) {
							goto bad_entity;
						}
						while (digits < terminator) {
							unsigned char digit = *digits++;
							if (digit >= '0' && digit <= '9') {
								value = (value * 16) + (digit - '0');
							} else if (digit >= 'A' && digit <= 'F') {
								value = (value * 16) + (digit - 'A' + 10);
							} else if (digit >= 'a' && digit <= 'f') {
								value = (value * 16) + (digit - 'a' + 10);
							} else {
								goto bad_entity;
							}
						}
					} else {
						/* Decimal */
						if (digits == terminator) {
							goto bad_entity;
						}
						while (digits < terminator) {
							unsigned char digit = *digits++;
							if (digit >= '0' && digit <= '9') {
								value = (value * 10) + (digit - '0');
							} else {
								goto bad_entity;
							}
						}
					}
					if (value > 0x10FFFF) {
						goto bad_entity;
					}
					*out++ = value;
					p = terminator + 1;
					goto next_iteration;
				} else if (terminator > p && terminator < e) {
					/* Named entity */
					mbfl_html_entity_entry *entity = (mbfl_html_entity_entry*)mbfl_html_entity_list;
					while (entity->name) {
						if (!strncmp((char*)p, entity->name, terminator - p) && strlen(entity->name) == terminator - p) {
							*out++ = entity->code;
							p = terminator + 1;
							goto next_iteration;
						}
						entity++;
					}
				}
			}
			/* Either we didn't find ;, or the name of the entity was not recognized */
bad_entity:
			*out++ = '&';
			while (p < terminator && out < limit) {
				*out++ = *p++;
			}
			if (terminator < e && *terminator == ';' && out < limit) {
				*out++ = *p++;
			}
		} else {
			*out++ = c;
		}

next_iteration: ;
	}

	*in_len = e - p;
	*in = p;
	return out - buf;
}

static void mb_wchar_to_htmlent(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
	unsigned char *out, *limit;
	MB_CONVERT_BUF_LOAD(buf, out, limit);
	MB_CONVERT_BUF_ENSURE(buf, out, limit, len);

	while (len--) {
		uint32_t w = *in++;

		if (w < sizeof(htmlentitifieds) / sizeof(htmlentitifieds[0]) && htmlentitifieds[w] != 1) {
			/* Fast path for most ASCII characters */
			out = mb_convert_buf_add(out, w);
		} else {
			out = mb_convert_buf_add(out, '&');

			/* See if there is a matching named entity */
			mbfl_html_entity_entry *entity = (mbfl_html_entity_entry*)mbfl_html_entity_list;
			while (entity->name) {
				if (w == entity->code) {
					MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 1 + strlen(entity->name));
					for (char *str = entity->name; *str; str++) {
						out = mb_convert_buf_add(out, *str);
					}
					out = mb_convert_buf_add(out, ';');
					goto next_iteration;
				}
				entity++;
			}

			/* There is no matching named entity; emit a numeric entity instead */
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 12);
			out = mb_convert_buf_add(out, '#');

			if (!w) {
				out = mb_convert_buf_add(out, '0');
			} else {
				unsigned char buf[12];
				unsigned char *converted = buf + sizeof(buf);
				while (w) {
					*(--converted) = "0123456789"[w % 10];
					w /= 10;
				}
				while (converted < buf + sizeof(buf)) {
					out = mb_convert_buf_add(out, *converted++);
				}
			}

			out = mb_convert_buf_add(out, ';');
		}

next_iteration: ;
	}

	MB_CONVERT_BUF_STORE(buf, out, limit);
}
