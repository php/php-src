#ifndef _NEW_SAPI_H
#define _NEW_SAPI_H


#if WIN32||WINNT
#	ifdef SAPI_EXPORTS
#	define SAPI_API __declspec(dllexport) 
#	else
#	define SAPI_API __declspec(dllimport) 
#	endif
#else
#define SAPI_API
#endif


typedef struct {
	int (*ub_write)(const char *str, unsigned int str_length);
} sapi_functions_struct;

extern sapi_functions_struct sapi_functions;  /* true global */


typedef struct {
	void *server_context;
} sapi_globals_struct;


void sapi_startup(sapi_functions_struct *sf);

#ifdef ZTS
# define SLS_D	sapi_globals_struct *sapi_globals
# define SLS_DC	, SLS_D
# define SLS_C	sapi_globals
# define SLS_CC , SLS_C
# define SG(v) (sapi_globals->v)
# define SLS_FETCH()	sapi_globals_struct *sapi_globals = ts_resource(sapi_globals_id)
SAPI_API extern int sapi_globals_id;
#else
# define SLS_D
# define SLS_DC
# define SLS_C
# define SLS_CC
# define SG(v) (sapi_globals.v)
# define SLS_FETCH()
extern SAPI_API sapi_globals_struct sapi_globals;
#endif


#endif /* _NEW_SAPI_H */