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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif

#include "mbfilter.h"
#include "mbfilter_htmlent.h"
#include "html_entities.h"

static const unsigned char mblen_table_html[] = { /* 0x00, 0x80 - 0xFF, only valid for numeric entities */
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6
};

static const char *mbfl_encoding_html_ent_aliases[] = {"HTML", "html", NULL};

const mbfl_encoding mbfl_encoding_html_ent = {
	mbfl_no_encoding_html_ent,
	"HTML-ENTITIES",
	"HTML-ENTITIES",
	(const char *(*)[])&mbfl_encoding_html_ent_aliases,
	NULL, /* mblen_table_html, Do not use table instead calulate length based on entities actually used */
	MBFL_ENCTYPE_HTML_ENT
};

const struct mbfl_convert_vtbl vtbl_wchar_html = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_html_ent,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_html_enc,
	mbfl_filt_conv_html_enc_flush
};

const struct mbfl_convert_vtbl vtbl_html_wchar = {
	mbfl_no_encoding_html_ent,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_html_dec_ctor,
	mbfl_filt_conv_html_dec_dtor,
	mbfl_filt_conv_html_dec,
	mbfl_filt_conv_html_dec_flush };


#define CK(statement)	do { if ((statement) < 0) return (-1); } while (0)

/*
 * any => HTML
 */
int mbfl_filt_conv_html_enc(int c, mbfl_convert_filter *filter)
{
	int tmp[10];
	int i = 0, p = 0, e;
	unsigned int uc;

	if (c<256 && mblen_table_html[c]==1) {
		CK((*filter->output_function)(c, filter->data));
	} else {
		/*php_error_docref("ref.mbstring" TSRMLS_CC, E_NOTICE, "mbfl_filt_conv_html_enc(0x%08X = %d)", c, c);*/
 		CK((*filter->output_function)('&', filter->data));
		while (1) {
		    e = mbfl_html_entity_list[i].code;
			if (c < e || e == -1) {
				break;
			}
			if (c == e) {
				while(mbfl_html_entity_list[i].name[p]) {
					CK((*filter->output_function)((int)mbfl_html_entity_list[i].name[p++], filter->data));
				}
				break;
			}
			i++;
		}
		i=0;
		if (!p) {
			CK((*filter->output_function)('#', filter->data));
			uc = (unsigned int)c;
			do {
				tmp[i++] = '0'+uc%10;
				uc /= 10;
			} while (uc);
			do {
				CK((*filter->output_function)(tmp[--i], filter->data));
			} while (i);
		}
		CK((*filter->output_function)(';', filter->data));
	}
	return c;
}

int mbfl_filt_conv_html_enc_flush(mbfl_convert_filter *filter)
{
	filter->status = 0;
	filter->cache = 0;
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
	filter->cache = (int)mbfl_malloc(html_enc_buffer_size+1);
}
	
void mbfl_filt_conv_html_dec_dtor(mbfl_convert_filter *filter)
{
	filter->status = 0;
	if (filter->cache)
	{
		mbfl_free((void*)filter->cache);
	}
	filter->cache = 0;
}

int mbfl_filt_conv_html_dec(int c, mbfl_convert_filter *filter)
{
	int  pos, ent = 0;
	mbfl_html_entity *entity;
	char *buffer = (char*)filter->cache;

	if (!filter->status) {
		if (c == '&' ) {
			filter->status = 1;
			buffer[0] = '&';
		} else {
			CK((*filter->output_function)(c, filter->data));
		}
	} else {
		if (c == ';') {
			buffer[filter->status] = 0;
			if (buffer[1]=='#') {
				/* numeric entity */
				for (pos=2; pos<filter->status; pos++) {
					ent = ent*10 + (buffer[pos] - '0');
				}
				CK((*filter->output_function)(ent, filter->data));
				filter->status = 0;
				/*php_error_docref("ref.mbstring" TSRMLS_CC, E_NOTICE, "mbstring decoded '%s'=%d", buffer, ent);*/
			} else {
				/* named entity */
			        entity = (mbfl_html_entity *)mbfl_html_entity_list;
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
					/*php_error_docref("ref.mbstring" TSRMLS_CC, E_NOTICE,"mbstring decoded '%s'=%d", buffer, ent);*/
				} else { 
					/* failure */
					buffer[filter->status++] = ';';
					buffer[filter->status] = 0;
					/* php_error_docref("ref.mbstring" TSRMLS_CC, E_WARNING, "mbstring cannot decode '%s'", buffer); */
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
				/* php_error_docref("ref.mbstring" TSRMLS_CC, E_WARNING, "mbstring cannot decode '%s'", buffer)l */
				mbfl_filt_conv_html_dec_flush(filter);
				if (c=='&')
				{
					filter->status = 1;
					buffer[0] = '&';
				}
			}
		}
	}
	return c;
}

int mbfl_filt_conv_html_dec_flush(mbfl_convert_filter *filter)
{
	int status, pos = 0;
	char *buffer;

	buffer = (char*)filter->cache;
	status = filter->status;
	/* flush fragments */
	while (status--) {
		CK((*filter->output_function)(buffer[pos++], filter->data));
	}
	filter->status = 0;
	/*filter->buffer = 0; of cause NOT*/
	return 0;
}


