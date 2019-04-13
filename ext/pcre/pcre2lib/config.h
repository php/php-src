
#include <php_compat.h>

#ifdef PHP_WIN32
# include <config.w32.h>
#else
# include <php_config.h>
#endif

#define SUPPORT_UNICODE 1
#define SUPPORT_PCRE2_8 1

#if defined(__GNUC__) && __GNUC__ >= 4
# ifdef __cplusplus
#  define PCRE2_EXP_DECL		extern "C" __attribute__ ((visibility("default")))
# else
#  define PCRE2_EXP_DECL		extern __attribute__ ((visibility("default")))
# endif
# define PCRE2_EXP_DEFN		__attribute__ ((visibility("default")))
#endif

/* Define to any value for valgrind support to find invalid memory reads. */
#if HAVE_PCRE_VALGRIND_SUPPORT
#define SUPPORT_VALGRIND 1
#endif

/* Define to any value to enable support for Just-In-Time compiling. */
#if HAVE_PCRE_JIT_SUPPORT
#define SUPPORT_JIT
#endif

/* This limits the amount of memory that pcre2_match() may use while matching
   a pattern. The value is in kilobytes. */
#ifndef HEAP_LIMIT
#define HEAP_LIMIT 20000000
#endif

/* The value of PARENS_NEST_LIMIT specifies the maximum depth of nested
   parentheses (of any kind) in a pattern. This limits the amount of system
   stack that is used while compiling a pattern. */
#ifndef PARENS_NEST_LIMIT
#define PARENS_NEST_LIMIT 250
#endif

/* The value of MATCH_LIMIT determines the default number of times the
   pcre2_match() function can record a backtrack position during a single
   matching attempt. There is a runtime interface for setting a different
   limit. The limit exists in order to catch runaway regular expressions that
   take for ever to determine that they do not match. The default is set very
   large so that it does not accidentally catch legitimate cases. */
#ifndef MATCH_LIMIT
#define MATCH_LIMIT 10000000
#endif

/* The above limit applies to all backtracks, whether or not they are nested.
   In some environments it is desirable to limit the nesting of backtracking
   (that is, the depth of tree that is searched) more strictly, in order to
   restrict the maximum amount of heap memory that is used. The value of
   MATCH_LIMIT_DEPTH provides this facility. To have any useful effect, it
   must be less than the value of MATCH_LIMIT. The default is to use the same
   value as MATCH_LIMIT. There is a runtime method for setting a different
   limit. */
#ifndef MATCH_LIMIT_DEPTH
#define MATCH_LIMIT_DEPTH MATCH_LIMIT
#endif

/* This limit is parameterized just in case anybody ever wants to change it.
   Care must be taken if it is increased, because it guards against integer
   overflow caused by enormously large patterns. */
#ifndef MAX_NAME_COUNT
#define MAX_NAME_COUNT 10000
#endif

/* This limit is parameterized just in case anybody ever wants to change it.
   Care must be taken if it is increased, because it guards against integer
   overflow caused by enormously large patterns. */
#ifndef MAX_NAME_SIZE
#define MAX_NAME_SIZE 32
#endif

/* Defining NEVER_BACKSLASH_C locks out the use of \C in all patterns. */
/* #undef NEVER_BACKSLASH_C */

/* The value of NEWLINE_DEFAULT determines the default newline character
   sequence. PCRE2 client programs can override this by selecting other values
   at run time. The valid values are 1 (CR), 2 (LF), 3 (CRLF), 4 (ANY), 5
   (ANYCRLF), and 6 (NUL). */
#ifndef NEWLINE_DEFAULT
#define NEWLINE_DEFAULT 2
#endif

/* The value of LINK_SIZE determines the number of bytes used to store links
   as offsets within the compiled regex. The default is 2, which allows for
   compiled patterns up to 64K long. This covers the vast majority of cases.
   However, PCRE2 can also be compiled to use 3 or 4 bytes instead. This
   allows for longer patterns in extreme cases. */
#ifndef LINK_SIZE
#define LINK_SIZE 2
#endif

