#include "SAPI.h"
#include "TSRM.h"


#ifdef ZTS
int sapi_globals_id;
#endif


void sapi_startup()
{
#ifdef ZTS
	sapi_globals_id = ts_allocate_id(sizeof(sapi_globals_struct), NULL, NULL);
#endif
}

