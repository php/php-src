#ifndef _PHP_REGEX_H
#define _PHP_REGEX_H

#if REGEX
#include "regex/regex.h"
#define _REGEX_H 1				/* this should stop Apache from loading the system version of regex.h */
#define _RX_H 1				  	/* Try defining these for Linux to	*/
#define __REGEXP_LIBRARY_H__ 1 	/* avoid Apache including regex.h	*/
#define _H_REGEX 1              /* This one is for AIX */
#else
#include <regex.h>
#endif

#endif /* _PHP_REGEX_H */
