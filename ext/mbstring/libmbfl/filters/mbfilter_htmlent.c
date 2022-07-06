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
	(const char *(*)[])&mbfl_encoding_html_ent_aliases,
	NULL,
	MBFL_ENCTYPE_GL_UNSAFE,
	&vtbl_html_wchar,
	&vtbl_wchar_html
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
	return c;
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
	int  pos, ent = 0;
	mbfl_html_entity_entry *entity;
	char *buffer = (char*)filter->opaque;

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
							int v =  buffer[pos];
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
				if (ent >= 0 && ent < 0x110000) {
					CK((*filter->output_function)(ent, filter->data));
				} else {
					for (pos = 0; pos < filter->status; pos++) {
						CK((*filter->output_function)(buffer[pos], filter->data));
					}
					CK((*filter->output_function)(c, filter->data));
				}
				filter->status = 0;
				/*php_error_docref("ref.mbstring", E_NOTICE, "mbstring decoded '%s'=%d", buffer, ent);*/
			} else {
				/* named entity */
				buffer[filter->status] = 0;
				entity = (mbfl_html_entity_entry *)mbfl_html_entity_list;
				while (entity->name) {
					if (!strcmp(buffer+1, entity->name))	{
						ent = entity->code;
						break;
					}
					entity++;
				}
				if (ent) {
					/* decoded */
					CK((*filter->output_function)(ent, filter->data));
					filter->status = 0;
					/*php_error_docref("ref.mbstring", E_NOTICE,"mbstring decoded '%s'=%d", buffer, ent);*/
				} else {
					/* failure */
					buffer[filter->status++] = ';';
					buffer[filter->status] = 0;
					/* php_error_docref("ref.mbstring", E_WARNING, "mbstring cannot decode '%s'", buffer); */
					mbfl_filt_conv_html_dec_flush(filter);
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
				/* php_error_docref("ref.mbstring", E_WARNING, "mbstring cannot decode '%s'", buffer)l */
				mbfl_filt_conv_html_dec_flush(filter);
				if (c=='&')
				{
					buffer[filter->status++] = '&';
				}
			}
		}
	}
	return c;
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
