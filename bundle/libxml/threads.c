/**
 * threads.c: set of generic threading related routines 
 *
 * See Copyright for the status of this software.
 *
 * Gary Pennington <Gary.Pennington@uk.sun.com>
 * daniel@veillard.com
 */

#define IN_LIBXML
#include "libxml.h"

#include <string.h>

#include <libxml/threads.h>
#include <libxml/globals.h>

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_PTHREAD_H
#include <pthread.h>
#endif

#ifdef HAVE_WIN32_THREADS
#include <windows.h>
#ifndef HAVE_COMPILER_TLS
#include <process.h>
#endif
#endif

#if defined(SOLARIS)
#include <note.h>
#endif

/* #define DEBUG_THREADS */

/*
 * TODO: this module still uses malloc/free and not xmlMalloc/xmlFree
 *       to avoid some crazyness since xmlMalloc/xmlFree may actually
 *       be hosted on allocated blocks needing them for the allocation ...
 */

/*
 * xmlMutex are a simple mutual exception locks
 */
struct _xmlMutex {
#ifdef HAVE_PTHREAD_H
    pthread_mutex_t lock;
#elif defined HAVE_WIN32_THREADS
    HANDLE mutex;
#else
    int empty;
#endif
};

/*
 * xmlRMutex are reentrant mutual exception locks
 */
struct _xmlRMutex {
#ifdef HAVE_PTHREAD_H
    pthread_mutex_t lock;
    unsigned int    held;
    unsigned int    waiters;
    pthread_t       tid;
    pthread_cond_t  cv;
#elif defined HAVE_WIN32_THREADS
    CRITICAL_SECTION cs;
    unsigned int count;
#else
    int empty;
#endif
};
/*
 * This module still has some internal static data.
 *   - xmlLibraryLock a global lock
 *   - globalkey used for per-thread data
 */

#ifdef HAVE_PTHREAD_H
static pthread_key_t	globalkey;
static pthread_t	mainthread;
static pthread_once_t once_control = PTHREAD_ONCE_INIT;
#elif defined HAVE_WIN32_THREADS
#if defined(HAVE_COMPILER_TLS)
static __declspec(thread) xmlGlobalState tlstate;
static __declspec(thread) int tlstate_inited = 0;
#else /* HAVE_COMPILER_TLS */
static DWORD globalkey;
#endif /* HAVE_COMPILER_TLS */
static DWORD mainthread;
static int run_once_init = 1;
#endif /* HAVE_WIN32_THREADS */

static xmlRMutexPtr	xmlLibraryLock = NULL;
static void xmlOnceInit(void);

/**
 * xmlNewMutex:
 *
 * xmlNewMutex() is used to allocate a libxml2 token struct for use in
 * synchronizing access to data.
 *
 * Returns a new simple mutex pointer or NULL in case of error
 */
xmlMutexPtr
xmlNewMutex(void)
{
    xmlMutexPtr tok;

    if ((tok = malloc(sizeof(xmlMutex))) == NULL)
        return (NULL);
#ifdef HAVE_PTHREAD_H
    pthread_mutex_init(&tok->lock, NULL);
#elif defined HAVE_WIN32_THREADS
    tok->mutex = CreateMutex(NULL, FALSE, NULL);
#endif
    return (tok);
}

/**
 * xmlFreeMutex:
 * @tok:  the simple mutex
 *
 * xmlFreeMutex() is used to reclaim resources associated with a libxml2 token
 * struct.
 */
void
xmlFreeMutex(xmlMutexPtr tok)
{
#ifdef HAVE_PTHREAD_H
    pthread_mutex_destroy(&tok->lock);
#elif defined HAVE_WIN32_THREADS
    CloseHandle(tok->mutex);
#endif
    free(tok);
}

/**
 * xmlMutexLock:
 * @tok:  the simple mutex
 *
 * xmlMutexLock() is used to lock a libxml2 token.
 */
void
xmlMutexLock(xmlMutexPtr tok ATTRIBUTE_UNUSED)
{
#ifdef HAVE_PTHREAD_H
    pthread_mutex_lock(&tok->lock);
#elif defined HAVE_WIN32_THREADS
    WaitForSingleObject(tok->mutex, INFINITE);
#endif

}

/**
 * xmlMutexUnlock:
 * @tok:  the simple mutex
 *
 * xmlMutexUnlock() is used to unlock a libxml2 token.
 */
void
xmlMutexUnlock(xmlMutexPtr tok ATTRIBUTE_UNUSED)
{
#ifdef HAVE_PTHREAD_H
    pthread_mutex_unlock(&tok->lock);
#elif defined HAVE_WIN32_THREADS
    ReleaseMutex(tok->mutex);
#endif
}

/**
 * xmlNewRMutex:
 *
 * xmlRNewMutex() is used to allocate a reentrant mutex for use in
 * synchronizing access to data. token_r is a re-entrant lock and thus useful
 * for synchronizing access to data structures that may be manipulated in a
 * recursive fashion.
 *
 * Returns the new reentrant mutex pointer or NULL in case of error
 */
xmlRMutexPtr
xmlNewRMutex(void)
{
    xmlRMutexPtr tok;

    if ((tok = malloc(sizeof(xmlRMutex))) == NULL)
        return (NULL);
#ifdef HAVE_PTHREAD_H
    pthread_mutex_init(&tok->lock, NULL);
    tok->held = 0;
    tok->waiters = 0;
#elif defined HAVE_WIN32_THREADS
    InitializeCriticalSection(&tok->cs);
    tok->count = 0;
#endif
    return (tok);
}

/**
 * xmlFreeRMutex:
 * @tok:  the reentrant mutex
 *
 * xmlRFreeMutex() is used to reclaim resources associated with a
 * reentrant mutex.
 */
void
xmlFreeRMutex(xmlRMutexPtr tok ATTRIBUTE_UNUSED)
{
#ifdef HAVE_PTHREAD_H
    pthread_mutex_destroy(&tok->lock);
#elif defined HAVE_WIN32_THREADS
    DeleteCriticalSection(&tok->cs);
#endif
    free(tok);
}

/**
 * xmlRMutexLock:
 * @tok:  the reentrant mutex
 *
 * xmlRMutexLock() is used to lock a libxml2 token_r.
 */
void
xmlRMutexLock(xmlRMutexPtr tok ATTRIBUTE_UNUSED)
{
#ifdef HAVE_PTHREAD_H
    pthread_mutex_lock(&tok->lock);
    if (tok->held) {
        if (pthread_equal(tok->tid, pthread_self())) {
            tok->held++;
            pthread_mutex_unlock(&tok->lock);
            return;
        } else {
            tok->waiters++;
            while (tok->held)
                pthread_cond_wait(&tok->cv, &tok->lock);
            tok->waiters--;
        }
    }
    tok->tid = pthread_self();
    tok->held = 1;
    pthread_mutex_unlock(&tok->lock);
#elif defined HAVE_WIN32_THREADS
    EnterCriticalSection(&tok->cs);
    ++tok->count;
#endif
}

/**
 * xmlRMutexUnlock:
 * @tok:  the reentrant mutex
 *
 * xmlRMutexUnlock() is used to unlock a libxml2 token_r.
 */
void
xmlRMutexUnlock(xmlRMutexPtr tok ATTRIBUTE_UNUSED)
{
#ifdef HAVE_PTHREAD_H
    pthread_mutex_lock(&tok->lock);
    tok->held--;
    if (tok->held == 0) {
        if (tok->waiters)
            pthread_cond_signal(&tok->cv);
        tok->tid = 0;
    }
    pthread_mutex_unlock(&tok->lock);
#elif defined HAVE_WIN32_THREADS
    if (!--tok->count) 
	LeaveCriticalSection(&tok->cs);
#endif
}

/************************************************************************
 *									*
 *			Per thread global state handling		*
 *									*
 ************************************************************************/

#ifdef LIBXML_THREAD_ENABLED
/**
 * xmlFreeGlobalState:
 * @state:  a thread global state
 *
 * xmlFreeGlobalState() is called when a thread terminates with a non-NULL
 * global state. It is is used here to reclaim memory resources.
 */
static void
xmlFreeGlobalState(void *state)
{
    free(state);
}

/**
 * xmlNewGlobalState:
 *
 * xmlNewGlobalState() allocates a global state. This structure is used to
 * hold all data for use by a thread when supporting backwards compatibility
 * of libxml2 to pre-thread-safe behaviour.
 *
 * Returns the newly allocated xmlGlobalStatePtr or NULL in case of error
 */
static xmlGlobalStatePtr
xmlNewGlobalState(void)
{
    xmlGlobalState *gs;
    
    gs = malloc(sizeof(xmlGlobalState));
    if (gs == NULL)
	return(NULL);

    memset(gs, 0, sizeof(xmlGlobalState));
    xmlInitializeGlobalState(gs);
    return (gs);
}
#endif /* LIBXML_THREAD_ENABLED */


#ifdef HAVE_WIN32_THREADS
#if !defined(HAVE_COMPILER_TLS)
typedef struct _xmlGlobalStateCleanupHelperParams
{
    HANDLE thread;
    void *memory;
} xmlGlobalStateCleanupHelperParams;

static void xmlGlobalStateCleanupHelper (void *p)
{
    xmlGlobalStateCleanupHelperParams *params = (xmlGlobalStateCleanupHelperParams *) p;
    WaitForSingleObject(params->thread, INFINITE);
    CloseHandle(params->thread);
    xmlFreeGlobalState(params->memory);
    free(params);
    _endthread();
}
#endif /* HAVE_COMPILER_TLS */
#endif /* HAVE_WIN32_THREADS */

/**
 * xmlGetGlobalState:
 *
 * xmlGetGlobalState() is called to retrieve the global state for a thread.
 *
 * Returns the thread global state or NULL in case of error
 */
xmlGlobalStatePtr
xmlGetGlobalState(void)
{
#ifdef HAVE_PTHREAD_H
    xmlGlobalState *globalval;

    pthread_once(&once_control, xmlOnceInit);

    if ((globalval = (xmlGlobalState *)
		pthread_getspecific(globalkey)) == NULL) {
        xmlGlobalState *tsd = xmlNewGlobalState();

        pthread_setspecific(globalkey, tsd);
        return (tsd);
    }
    return (globalval);
#elif defined HAVE_WIN32_THREADS
#if defined(HAVE_COMPILER_TLS)
    if (!tlstate_inited) {
	tlstate_inited = 1;
	xmlInitializeGlobalState(&tlstate);
    }
    return &tlstate;
#else /* HAVE_COMPILER_TLS */
    xmlGlobalState *globalval;

    if (run_once_init) { 
	run_once_init = 0; 
	xmlOnceInit(); 
    }
    if ((globalval = (xmlGlobalState *) TlsGetValue(globalkey)) == NULL) {
	xmlGlobalState *tsd = xmlNewGlobalState();
	xmlGlobalStateCleanupHelperParams *p = 
	    (xmlGlobalStateCleanupHelperParams *) malloc(sizeof(xmlGlobalStateCleanupHelperParams));
	p->memory = tsd;
	DuplicateHandle(GetCurrentProcess(), GetCurrentThread(), 
		GetCurrentProcess(), &p->thread, 0, TRUE, DUPLICATE_SAME_ACCESS);
	TlsSetValue(globalkey, tsd);
	_beginthread(xmlGlobalStateCleanupHelper, 0, p);

	return (tsd);
    }
    return (globalval);
#endif /* HAVE_COMPILER_TLS */
#else
    return(NULL);
#endif
}

/************************************************************************
 *									*
 *			Library wide thread interfaces			*
 *									*
 ************************************************************************/

/**
 * xmlGetThreadId:
 *
 * xmlGetThreadId() find the current thread ID number
 *
 * Returns the current thread ID number
 */
int
xmlGetThreadId(void)
{
#ifdef HAVE_PTHREAD_H
    return((int) pthread_self());
#elif defined HAVE_WIN32_THREADS
    return GetCurrentThreadId();
#else
    return((int) 0);
#endif
}

/**
 * xmlIsMainThread:
 *
 * xmlIsMainThread() check whether the current thread is the main thread.
 *
 * Returns 1 if the current thread is the main thread, 0 otherwise
 */
int
xmlIsMainThread(void)
{
#ifdef HAVE_PTHREAD_H
    pthread_once(&once_control, xmlOnceInit);
#elif defined HAVE_WIN32_THREADS
    if (run_once_init) { 
	run_once_init = 0; 
	xmlOnceInit (); 
    }
#endif
        
#ifdef DEBUG_THREADS
    xmlGenericError(xmlGenericErrorContext, "xmlIsMainThread()\n");
#endif
#ifdef HAVE_PTHREAD_H
    return(mainthread == pthread_self());
#elif defined HAVE_WIN32_THREADS
    return(mainthread == GetCurrentThreadId ());
#else
    return(1);
#endif
}

/**
 * xmlLockLibrary:
 *
 * xmlLockLibrary() is used to take out a re-entrant lock on the libxml2
 * library.
 */
void
xmlLockLibrary(void)
{
#ifdef DEBUG_THREADS
    xmlGenericError(xmlGenericErrorContext, "xmlLockLibrary()\n");
#endif
    xmlRMutexLock(xmlLibraryLock);
}

/**
 * xmlUnlockLibrary:
 *
 * xmlUnlockLibrary() is used to release a re-entrant lock on the libxml2
 * library.
 */
void
xmlUnlockLibrary(void)
{
#ifdef DEBUG_THREADS
    xmlGenericError(xmlGenericErrorContext, "xmlUnlockLibrary()\n");
#endif
    xmlRMutexUnlock(xmlLibraryLock);
}

/**
 * xmlInitThreads:
 *
 * xmlInitThreads() is used to to initialize all the thread related
 * data of the libxml2 library.
 */
void
xmlInitThreads(void)
{
#ifdef DEBUG_THREADS
    xmlGenericError(xmlGenericErrorContext, "xmlInitThreads()\n");
#endif
}

/**
 * xmlCleanupThreads:
 *
 * xmlCleanupThreads() is used to to cleanup all the thread related
 * data of the libxml2 library once processing has ended.
 */
void
xmlCleanupThreads(void)
{
#ifdef DEBUG_THREADS
    xmlGenericError(xmlGenericErrorContext, "xmlCleanupThreads()\n");
#endif
}

/**
 * xmlOnceInit
 *
 * xmlOnceInit() is used to initialize the value of mainthread for use
 * in other routines. This function should only be called using
 * pthread_once() in association with the once_control variable to ensure
 * that the function is only called once. See man pthread_once for more
 * details.
 */
static void
xmlOnceInit(void) {
#ifdef HAVE_PTHREAD_H
    (void) pthread_key_create(&globalkey, xmlFreeGlobalState);
    mainthread = pthread_self();
#endif

#if defined(HAVE_WIN32_THREADS)
#if !defined(HAVE_COMPILER_TLS)
    globalkey = TlsAlloc();
#endif
    mainthread = GetCurrentThreadId();
#endif
}
