/**
 * threads.c: set of generic threading related routines 
 *
 * See Copyright for the status of this software.
 *
 * daniel@veillard.com
 */

#ifndef __XML_THREADS_H__
#define __XML_THREADS_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
 * xmlMutex are a simple mutual exception locks.
 */
typedef struct _xmlMutex xmlMutex;
typedef xmlMutex *xmlMutexPtr;

/*
 * xmlRMutex are reentrant mutual exception locks.
 */
typedef struct _xmlRMutex xmlRMutex;
typedef xmlRMutex *xmlRMutexPtr;

#ifdef __cplusplus
}
#endif
#include <libxml/globals.h>
#ifdef __cplusplus
extern "C" {
#endif

xmlMutexPtr		xmlNewMutex	(void);
void			xmlMutexLock	(xmlMutexPtr tok);
void			xmlMutexUnlock	(xmlMutexPtr tok);
void			xmlFreeMutex	(xmlMutexPtr tok);

xmlRMutexPtr		xmlNewRMutex	(void);
void			xmlRMutexLock	(xmlRMutexPtr tok);
void			xmlRMutexUnlock	(xmlRMutexPtr tok);
void			xmlFreeRMutex	(xmlRMutexPtr tok);

/*
 * Library wide APIs.
 */
void			xmlInitThreads	(void);
void			xmlLockLibrary	(void);
void			xmlUnlockLibrary(void);
int			xmlGetThreadId	(void);
int			xmlIsMainThread	(void);
void			xmlCleanupThreads(void);
xmlGlobalStatePtr	xmlGetGlobalState(void);

#ifdef __cplusplus
}
#endif


#endif /* __XML_THREADS_H__ */
