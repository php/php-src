
#undef regexec
#undef regerror
#undef regfree
#undef regcomp

#if (defined(REGEX) && REGEX == 1) || (!defined(REGEX))

#define regexec php_regexec
#define regerror php_regerror
#define regfree php_regfree
#define regcomp php_regcomp

#endif
