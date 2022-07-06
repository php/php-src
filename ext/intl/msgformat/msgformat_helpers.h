/*
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
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
