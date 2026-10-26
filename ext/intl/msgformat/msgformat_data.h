/*
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Authors: Stanislav Malyshev <stas@zend.com>                          |
   +----------------------------------------------------------------------+
 */

#ifndef MSG_FORMAT_DATA_H
#define MSG_FORMAT_DATA_H

#include <php.h>

#include "../intl_error.h"

#include <unicode/umsg.h>

typedef struct {
	// error hangling
	intl_error      error;

	// formatter handling
	UMessageFormat *umsgf;
	char*			orig_format;
	zend_ulong		orig_format_len;
	HashTable*		arg_types;
	int				tz_set; /* if we've already the time zone in sub-formats */
} msgformat_data;

void msgformat_data_init( msgformat_data* mf_data );
void msgformat_data_free( msgformat_data* mf_data );

#ifdef MSG_FORMAT_QUOTE_APOS
int msgformat_fix_quotes(UChar **spattern, uint32_t *spattern_len, UErrorCode *ec);
#endif

#endif // MSG_FORMAT_DATA_H
