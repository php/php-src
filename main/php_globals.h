#ifndef _PHP_GLOBALS_H
#define _PHP_GLOBALS_H

#include "zend_globals.h"

typedef struct _php_core_globals php_core_globals;

#ifdef ZTS
# define PLS_D	php_core_globals *core_globals
# define PLS_DC	, PLS_D
# define PLS_C	core_globals
# define PLS_CC , PLS_C
# define PG(v) (core_globals->v)
# define PLS_FETCH()	php_core_globals *core_globals = ts_resource(core_globals_id)
extern int core_globals_id;
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

	long safe_mode;
	long sql_safe_mode;
	char *safe_mode_exec_dir;
	long enable_dl;

    long max_execution_time;
    long memory_limit;

    long track_errors;
    long display_errors;
    long log_errors;
    char *error_log;

    char *doc_root;
    char *user_dir;
    char *include_path;
	char *open_basedir;
    char *extension_dir;

    char *upload_tmp_dir;
	long upload_max_filesize;

    char *auto_prepend_file;
    char *auto_append_file;

    char *arg_separator;
    char *gpc_order;

    long track_vars;

    long y2k_compliance;
};


#endif /* _PHP_GLOBALS_H */