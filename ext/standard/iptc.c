/*
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997,1998 PHP Development Team (See Credits file)      |
   +----------------------------------------------------------------------+
   | This program is free software; you can redistribute it and/or modify |
   | it under the terms of one of the following licenses:                 |
   |                                                                      |
   |  A) the GNU General Public License as published by the Free Software |
   |     Foundation; either version 2 of the License, or (at your option) |
   |     any later version.                                               |
   |                                                                      |
   |  B) the PHP License as published by the PHP Development Team and     |
   |     included in the distribution in the file: LICENSE                |
   |                                                                      |
   | This program is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |
   | GNU General Public License for more details.                         |
   |                                                                      |
   | You should have received a copy of both licenses referred to here.   |
   | If you did not, or have any questions about PHP licensing, please    |
   | contact core@php.net.                                                |
   +----------------------------------------------------------------------+
   | Authors: Thies C. Arntzen (thies@digicol.de)                         |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

/*
 * Functions to parse & compse IPTC data.
 * PhotoShop >= 3.0 can read and write textual data to JPEG files.
 * ... more to come .....
 */

/*
 * TODO:
 *  - add IPTC translation table
 *  - implement a call to embed iptc into a JPEG file
 */
 
#include "php.h"
#include "php3_iptc.h"

void php3_iptcparse(INTERNAL_FUNCTION_PARAMETERS)
{
	unsigned int length, inx, len, inheader, tagsfound;
	unsigned char *buffer;
	unsigned char recnum, dataset;
	unsigned char key[ 16 ];
	pval values, *str, *element;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &str) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(str);


	inx = 0;
	length = str->value.str.len;
	buffer = str->value.str.val;

	inheader = 0; /* have we already found the IPTC-Header??? */
	tagsfound = 0; /* number of tags already found */

	while (inx < length) {
		if (buffer[ inx++ ] != 0x1c) { /* skip all junk */
			if (inheader) {
				break;   /* we ran against some data which does not conform to IPTC - stop parsing! */
			} else {
				continue;
			}
		} else {
			inheader = 1;
		}
		
		if ((inx + 4) >= length)
			break;

		dataset = buffer[ inx++ ];
		recnum = buffer[ inx++ ];

		if (buffer[ inx ] & (unsigned char) 0x80) {
			len = (((long) buffer[ inx + 2 ]) << 24) + (((long) buffer[ inx + 3 ]) << 16) + 
				  (((long) buffer[ inx + 4 ]) <<  8) + (((long) buffer[ inx + 5 ]));
			inx += 6;
		} else {
			len = (((unsigned short) buffer[ inx ])<<8) | (unsigned short)buffer[ inx+1 ];
			inx += 2;
		}

		sprintf(key,"%d#%03d",(unsigned int) dataset,(unsigned int) recnum);

		if ((inx + len) > length)
			break;

		if (tagsfound == 0) { /* found the 1st tag - initialize the return array */
			if (array_init(return_value) == FAILURE) {
				php3_error(E_ERROR, "Unable to initialize array");
				RETURN_FALSE;
	  		}
		}

		if (_php3_hash_find(return_value->value.ht,key,strlen(key) + 1,(void **) &element) == FAILURE) {
			if (array_init(&values) == FAILURE) {
				php3_error(E_ERROR, "Unable to initialize array");
				RETURN_FALSE;
			}
			
			_php3_hash_update(return_value->value.ht, key, strlen(key)+1, (void *) &values, sizeof(pval), (void **) &element);
		} 
			
		add_next_index_stringl(element,buffer+inx,len,1);

		inx += len;

		tagsfound++;
	}

	if (! tagsfound) {
		RETURN_FALSE;
	}
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
