
#ifdef NETWARE

/* ------------------------------------------------------------------
 * These functions are to be called when the shared NLM starts and
 * stops.  By using these functions instead of defining a main()
 * and calling ExitThread(TSR_THREAD, 0), the load time of the
 * shared NLM is faster and memory size reduced.
 * 
 * You may also want to override these in your own Apache module
 * to do any cleanup other than the mechanism Apache modules provide.
 * ------------------------------------------------------------------
 */


#ifdef __GNUC__
#include <string.h>        /* memset */
extern char _edata, _end ; /* end of DATA (start of BSS), end of BSS */
#endif

int _lib_start()
{
/*	printf("Inside _lib_start\n");*/
#ifdef __GNUC__
    memset (&_edata, 0, &_end - &_edata);
#endif
    return 0;
}

int _lib_stop()
{
/*	printf("Inside _lib_stop\n");*/
    return 0;
}

#endif	/* NETWARE */
