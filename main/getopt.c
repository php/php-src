/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2018 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Marcus Boerger <helly@php.net>                               |
   +----------------------------------------------------------------------+
*/

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include "php_getopt.h"

#define OPTERRCOLON (1)
#define OPTERRNF (2)
#define OPTERRARG (3)

static int php_opt_error(int argc, char * const *argv, int oint, int optchr, int err, int show_err) /* {{{ */
{
	if (show_err)
	{
		fprintf(stderr, "Error in argument %d, char %d: ", oint, optchr+1);
		switch(err)
		{
		case OPTERRCOLON:
			fprintf(stderr, ": in flags\n");
			break;
		case OPTERRNF:
			fprintf(stderr, "option not found %c\n", argv[oint][optchr]);
			break;
		case OPTERRARG:
			fprintf(stderr, "no argument for option %c\n", argv[oint][optchr]);
			break;
		default:
			fprintf(stderr, "unknown\n");
			break;
		}
	}
	return('?');
}
/* }}} */

PHPAPI int php_optidx = -1;

PHPAPI int php_getopt(int argc, char* const *argv, const opt_struct opts[], char **optarg, int *optind, int show_err, int arg_start) /* {{{ */
{
	static int optchr = 0;
	static int dash = 0; /* have already seen the - */
	static char **prev_optarg = NULL;

	php_optidx = -1;

	if(prev_optarg && prev_optarg != optarg) {
		/* reset the state */
		optchr = 0;
		dash = 0;
	}
	prev_optarg = optarg;

	if (*optind >= argc) {
		return(EOF);
	}
	if (!dash) {
		if ((argv[*optind][0] !=  '-')) {
			return(EOF);
		} else {
			if (!argv[*optind][1])
			{
				/*
				* use to specify stdin. Need to let pgm process this and
				* the following args
				*/
				return(EOF);
			}
		}
	}
	if ((argv[*optind][0] == '-') && (argv[*optind][1] == '-')) {
		const char *pos;
		size_t arg_end = strlen(argv[*optind])-1;

		/* '--' indicates end of args if not followed by a known long option name */
		if (argv[*optind][2] == '\0') {
			(*optind)++;
			return(EOF);
		}

		arg_start = 2;

		/* Check for <arg>=<val> */
		if ((pos = php_memnstr(&argv[*optind][arg_start], "=", 1, argv[*optind]+arg_end)) != NULL) {
			arg_end = pos-&argv[*optind][arg_start];
			arg_start++;
		} else {
			arg_end--;
		}

		while (1) {
			php_optidx++;
			if (opts[php_optidx].opt_char == '-') {
				(*optind)++;
				return(php_opt_error(argc, argv, *optind-1, optchr, OPTERRARG, show_err));
			} else if (opts[php_optidx].opt_name && !strncmp(&argv[*optind][2], opts[php_optidx].opt_name, arg_end) && arg_end == strlen(opts[php_optidx].opt_name)) {
				break;
			}
		}

		optchr = 0;
		dash = 0;
		arg_start += (int)strlen(opts[php_optidx].opt_name);
	} else {
		if (!dash) {
			dash = 1;
			optchr = 1;
		}
		/* Check if the guy tries to do a -: kind of flag */
		if (argv[*optind][optchr] == ':') {
			dash = 0;
			(*optind)++;
			return (php_opt_error(argc, argv, *optind-1, optchr, OPTERRCOLON, show_err));
		}
		arg_start = 1 + optchr;
	}
	if (php_optidx < 0) {
		while (1) {
			php_optidx++;
			if (opts[php_optidx].opt_char == '-') {
				int errind = *optind;
				int errchr = optchr;

				if (!argv[*optind][optchr+1]) {
					dash = 0;
					(*optind)++;
				} else {
					optchr++;
					arg_start++;
				}
				return(php_opt_error(argc, argv, errind, errchr, OPTERRNF, show_err));
			} else if (argv[*optind][optchr] == opts[php_optidx].opt_char) {
				break;
			}
		}
	}
	if (opts[php_optidx].need_param) {
		/* Check for cases where the value of the argument
		is in the form -<arg> <val>, -<arg>=<varl> or -<arg><val> */
		dash = 0;
		if (!argv[*optind][arg_start]) {
			(*optind)++;
			if (*optind == argc) {
				/* Was the value required or is it optional? */
				if (opts[php_optidx].need_param == 1) {
					return(php_opt_error(argc, argv, *optind-1, optchr, OPTERRARG, show_err));
				}
			/* Optional value is not supported with -<arg> <val> style */
			} else if (opts[php_optidx].need_param == 1) {
				*optarg = argv[(*optind)++];
 			}
		} else if (argv[*optind][arg_start] == '=') {
			arg_start++;
			*optarg = &argv[*optind][arg_start];
			(*optind)++;
		} else {
			*optarg = &argv[*optind][arg_start];
			(*optind)++;
		}
		return opts[php_optidx].opt_char;
	} else {
		/* multiple options specified as one (exclude long opts) */
		if (arg_start >= 2 && !((argv[*optind][0] == '-') && (argv[*optind][1] == '-'))) {
			if (!argv[*optind][optchr+1])
			{
				dash = 0;
				(*optind)++;
			} else {
				optchr++;
			}
		} else {
			(*optind)++;
		}
		return opts[php_optidx].opt_char;
	}
	assert(0);
	return(0);	/* never reached */
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
