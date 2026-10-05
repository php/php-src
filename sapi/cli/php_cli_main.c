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
*/

#include "php.h"
#include "cli.h"
#ifdef PHP_CLI_WITH_FPM
#include "sapi/fpm/fpm/fpm.h"
#endif

int main(int argc, char *argv[])
{
#ifdef PHP_CLI_WITH_FPM
	/* "php --fpm [args]" runs the FPM SAPI. argv is passed as is because FPM
	 * re-executes it on reload; FPM options start after "--fpm". */
	if (argc > 1 && strcmp(argv[1], "--fpm") == 0) {
		return fpm_main(argc, argv, 2);
	}
#endif
	return do_php_cli(argc, argv);
}
