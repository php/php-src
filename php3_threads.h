
#if WIN32|WINNT 
#define MUTEX_T void
#define MUTEX_INIT(a,b) (a=CreateMutex(b,FALSE,NULL))
#define MUTEX_DESTROY(a) CloseHandle(a)
#define MUTEX_LOCK(a) WaitForSingleObject(a,INFINITE)
#define MUTEX_UNLOCK(a) ReleaseMutex(a)
#define TLS_ALLOC(a) (a=TlsAlloc())
#define TLS_NOT_ALLOCED 0xFFFFFFFF
#define TLS_GET_DATA(a) TlsGetValue(a)
#define TLS_SET_DATA(a,b) TlsSetValue(a,b)
#define TLS_FREE(a) TlsFree(a)
#else /*PTHREADS*/
#define MUTEX_T pthread_mutex_t
#define MUTEX_INIT(a,b) pthread_mutex_init(a,b)
#define MUTEX_DESTROY(a) pthread_mutex_destroy(a)
#define MUTEX_LOCK(a) pthread_mutex_lock(a)
#define MUTEX_UNLOCK(a) pthread_mutex_unlock(a)
#define TLS_ALLOC(a)
#define TLS_NOT_ALLOCED NULL
#define TLS_GET_DATA(a) pthread_getspecific(a)
#define TLS_SET_DATA(a,b) pthread_setspecific(a,b)
#define TLS_FREE(a)
#endif

extern static void *php3_mutex_alloc( void );
extern static void php3_mutex_free( void *mutexp );
extern static void php3_mutex_lock( void *mutexp );
extern static void php3_mutex_unlock( void *mutexp );
extern int php3_tls_init(void *key,void *tsd,int size);
extern int php3_tls_free(void *key, void *tsd);
extern int php3_tls_startup(void *key);
extern int php3_tls_shutdown(void *key);
