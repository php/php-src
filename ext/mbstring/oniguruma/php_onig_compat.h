#ifndef _PHP_MBREGEX_COMPAT_H
#define _PHP_MBREGEX_COMPAT_H

#define re_pattern_buffer           php_mb_re_pattern_buffer
#define regex_t                     php_mb_regex_t
#define re_registers                php_mb_re_registers

#ifdef HAVE_STDARG_H
#ifndef HAVE_STDARG_PROTOTYPES
#define HAVE_STDARG_PROTOTYPES 1
#endif
#endif

#endif /* _PHP_MBREGEX_COMPAT_H */
