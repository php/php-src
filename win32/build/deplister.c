/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2012 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Wez Furlong <wez@thebrainroom.com>                           |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

/* This little application will list the DLL dependencies for a PE
 * module to it's stdout for use by distro/installer building tools */

#include <windows.h>
#include <imagehlp.h>

BOOL CALLBACK StatusRoutine(IMAGEHLP_STATUS_REASON reason,
		PSTR image_name, PSTR dll_name,
		ULONG va, ULONG param)
{
	switch (reason) {
		case BindImportModuleFailed:
			printf("%s,NOTFOUND\n", dll_name);
			return TRUE;

		case BindImportModule:
			printf("%s,OK\n", dll_name);
			return TRUE;
	}
	return TRUE;
}

/* usage:
 * deplister.exe path\to\module.exe path\to\symbols\root
 * */

int main(int argc, char *argv[])
{
	return BindImageEx(BIND_NO_BOUND_IMPORTS | BIND_NO_UPDATE | BIND_ALL_IMAGES,
		argv[1], NULL, argv[2], StatusRoutine);	
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
