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

#ifndef MSG_FORMAT_HELPERS_H
#define MSG_FORMAT_HELPERS_H

int32_t umsg_format_arg_count(UMessageFormat *fmt);
void umsg_format_helper(MessageFormatter_object *mfo, HashTable *args,
						UChar **formatted, int *formatted_len);
void umsg_parse_helper(UMessageFormat *fmt, int *count, zval **args,
					   UChar *source, int source_len, UErrorCode *status);
#endif // MSG_FORMAT_HELPERS_H
