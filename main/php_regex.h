#ifndef _PHP_REGEX_H
#define _PHP_REGEX_H

#if REGEX
#include "regex/regex.h"
#ifndef _REGEX_H
#define _REGEX_H 1				/* this should stop Apache from loading the system version of regex.h */
#endif
#ifndef _REGEX_H_
#define _REGEX_H_ 1
#endif
#ifndef _RX_H
#define _RX_H 1				  	/* Try defining these for Linux to	*/
#endif
#ifndef __REGEXP_LIBRARY___H
#define __REGEXP_LIBRARY_H__ 1 	/* avoid Apache including regex.h	*/
#endif
#ifndef _H_REGEX
#define _H_REGEX 1              /* This one is for AIX */
#endif
#else
#include <regex.h>
#endif

#endif /* _PHP_REGEX_H */
