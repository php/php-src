#ifndef _STDBOOL_H
#define _STDBOOL_H
#if !defined(__BOOL_DEFINED)
#  define bool short

/* The other macros must be usable in preprocessor directives.  */
#  define false 0
#  define true 1
#  define __bool_true_false_are_defined 1
# endif
#endif /* _STDBOOL_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
