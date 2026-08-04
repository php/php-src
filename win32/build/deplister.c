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
  | Author: Wez Furlong <wez@thebrainroom.com>                           |
  +----------------------------------------------------------------------+
*/

/* This little application will list the DLL dependencies for a PE
 * module to it's stdout for use by distro/installer building tools */

#include <windows.h>
#include <stdio.h>
#include <imagehlp.h>

BOOL CALLBACK StatusRoutine(IMAGEHLP_STATUS_REASON reason,
		PCSTR image_name, PCSTR dll_name,
		ULONG_PTR va, ULONG_PTR param)
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
