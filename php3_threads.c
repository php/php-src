#include <stdio.h>
#include "alloc.h"
#include <errno.h>
#if (WIN32|WINNT)
#include <windows.h>
#else
#include <pthread.h>
#endif
#include "php3_threads.h"

/* Function for allocating memory for mutexes */
static void *php3_mutex_alloc( void )
{
   MUTEX_T *mutexp;
   /* Allocate memory for the mutex and initialize it */
   if ( (mutexp = emalloc( sizeof(MUTEX_T) )) != NULL ) {
      MUTEX_INIT( mutexp, NULL );
   }
   return( mutexp );
}

/* Function for freeing mutexes */
static void php3_mutex_free( void *mutexp )
{
   MUTEX_DESTROY( (MUTEX_T *) mutexp );
}

static void php3_mutex_lock( void *mutexp )
{
	return MUTEX_LOCK(mutexp);
}

static void php3_mutex_unlock( void *mutexp )
{
	return MUTEX_UNLOCK(mutexp);
}

/* Function for setting up thread-specific data */
/* should be called at thread start */
int php3_tls_init(void *key,void *tsd,int size)
{
	/* Check if thread-specific data already exists */
	tsd = TLS_GET_DATA( key );
	if ( tsd != NULL ) {
		/*FIXME what kind of output here? */
		fprintf( stderr, "php3_tls_setup tls var non-null!\n" );
		THREAD_EXIT( NULL );
	}
	/* Allocate memory for the LDAP error values */
	tsd = (void *) ecalloc( 1, size );
	/* Make the data specific to the calling thread */
	TLS_SET_DATA( key, tsd );
}

/*should be called before thread exit*/
int php3_tls_free(void *key, void *tsd)
{
	if (tsd)efree(tsd);
}

/*should be called at dl or process startup*/
int php3_tls_startup(void *key)
{
	if (key == NULL){
		if (TLS_ALLOC(key)==TLS_NOT_ALLOCED)
			return 0;
	}
	return 1;
}

/*should be called before dl or process exit*/
int php3_tls_shutdown(void *key)
{
	if (key != NULL){
		if (!TLS_FREE(key))
			return 0;
	}
	return 1;
}