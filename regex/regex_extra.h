/* do not frame this - we must be able to include this file multiple times */

#undef regexec
#undef regerror
#undef regfree
#undef regcomp

#if (defined(REGEX) && REGEX == 1) || (!defined(REGEX))

#if !(WIN32|WINNT)

#ifndef PHP_NO_ALIASES

#define regexec php_regexec
#define regerror php_regerror
#define regfree php_regfree
#define regcomp php_regcomp

#endif

#endif

#endif
