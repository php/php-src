#include "SAPI.h"
#ifdef ZTS
#include "TSRM.h"
#endif


#ifdef ZTS
SAPI_API int sapi_globals_id;
#else
sapi_globals_struct sapi_globals;
#endif

/* A true global (no need for thread safety) */
sapi_functions_struct sapi_functions;

void sapi_startup(sapi_functions_struct *sf)
{
	sapi_functions = *sf;
#ifdef ZTS
	sapi_globals_id = ts_allocate_id(sizeof(sapi_globals_struct), NULL, NULL);
#endif
}