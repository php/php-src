/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2008 The PHP Group                                |
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


static int php_opt_error(int argc, char * const *argv, int oint, int optchr, int err, int show_err)
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

int php_getopt(int argc, char* const *argv, const opt_struct opts[], char **optarg, int *optind, int show_err)
{
	static int optchr = 0;
	static int dash = 0; /* have already seen the - */
	int arg_start = 2;

	int opts_idx = -1;

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
		/* '--' indicates end of args if not followed by a known long option name */
		while (1) {
			opts_idx++;
			if (opts[opts_idx].opt_char == '-') {
				(*optind)++;
				return(EOF);
			} else if (opts[opts_idx].opt_name && !strcmp(&argv[*optind][2], opts[opts_idx].opt_name)) {
				break;
			}
		}
		optchr = 0;
		dash = 1;
		arg_start = 2 + strlen(opts[opts_idx].opt_name);
	}
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
	if (opts_idx < 0) {
		while (1) {
			opts_idx++;
			if (opts[opts_idx].opt_char == '-') {
				int errind = *optind;
				int errchr = optchr;
		
				if (!argv[*optind][optchr+1]) {
					dash = 0;
					(*optind)++;
				} else {
					optchr++;
				}
				return(php_opt_error(argc, argv, errind, errchr, OPTERRNF, show_err));
			} else if (argv[*optind][optchr] == opts[opts_idx].opt_char) {
				break;
			}
		}
	}
	if (opts[opts_idx].need_param) {
		/* Check for cases where the value of the argument
		is in the form -<arg> <val> or in the form -<arg><val> */
		dash = 0;
		if(!argv[*optind][arg_start]) {
			(*optind)++;
			if (*optind == argc) {
				return(php_opt_error(argc, argv, *optind-1, optchr, OPTERRARG, show_err));
			}
			*optarg = argv[(*optind)++];
		} else {
			*optarg = &argv[*optind][arg_start];
			(*optind)++;
		}
		return opts[opts_idx].opt_char;
	} else {
		if (arg_start == 2) {
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
		return opts[opts_idx].opt_char;
	}
	assert(0);
	return(0);	/* never reached */
}
