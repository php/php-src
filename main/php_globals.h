#ifndef _PHP_GLOBALS_H
#define _PHP_GLOBALS_H

typedef struct _php_core_globals php_core_globals;

#ifdef ZTS
# define PLS_D	php_core_globals *core_globals
# define PLS_DC	, PLS_D
# define PLS_C	core_globals
# define PLS_CC , PLS_C
# define PG(v) (core_globals->v)
# define PLS_FETCH()	php_core_globals *core_globals = ts_resource(core_globals_id)
#else
# define PLS_D
# define PLS_DC
# define PLS_C
# define PLS_CC
# define PG(v) (core_globals.v)
# define PLS_FETCH()
extern ZEND_API php_core_globals core_globals;
#endif


struct _php_core_globals {
    long magic_quotes_gpc;
    long magic_quotes_runtime;
    long magic_quotes_sybase;

	long asp_tags;
	long short_tags;
};


#endif /* _PHP_GLOBALS_H */