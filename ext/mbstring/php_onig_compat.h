#ifndef _PHP_ONIG_COMPAT_H
#define _PHP_ONIG_COMPAT_H

#define re_pattern_buffer           php_mb_re_pattern_buffer
#define regex_t                     php_mb_regex_t
#define re_registers                php_mb_re_registers

/* Required for some distros that conditionally override PV_.
 * As we're in C99 this header is always available. */
#ifndef HAVE_STDARG_H
# define HAVE_STDARG_H
#endif

#endif /* _PHP_ONIG_COMPAT_H */
