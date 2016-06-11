/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2016 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Anatol Belski <ab@php.net>                                   |
   +----------------------------------------------------------------------+
 */

#include "ipc.h"

#include <windows.h>
#include <sys/stat.h>


PHPAPI key_t
ftok(const char *pathname, int proj_id)
{
	HANDLE fh;
	struct stat st;
	BY_HANDLE_FILE_INFORMATION bhfi;
	key_t ret;

	if (stat(pathname, &st) < 0) {
		return (key_t)-1;
	}

	if ((fh = CreateFile(pathname, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0)) == INVALID_HANDLE_VALUE) {
		return (key_t)-1;
	}

	if (!GetFileInformationByHandle(fh, &bhfi)) {
		CloseHandle(fh);
		return (key_t)-1;
	}

	ret = (key_t) ((proj_id & 0xff) << 24 | (st.st_dev & 0xff) << 16 | ((bhfi.nFileIndexLow | (__int64)bhfi.nFileIndexHigh << 32) & 0xffff));

	CloseHandle(fh);

	return ret;
}

