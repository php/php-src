#ifndef PHP_MILTER_H
#define PHP_MILTER_H

#include "libmilter/mfapi.h"

#define MLFI_NONE		0
#define MLFI_CONNECT	1
#define MLFI_HELO		2
#define MLFI_ENVFROM	3
#define MLFI_ENVRCPT	4
#define MLFI_HEADER		5
#define MLFI_EOH		6
#define MLFI_BODY		7
#define MLFI_EOM		8
#define MLFI_ABORT		9
#define MLFI_CLOSE		10
#define MLFI_INIT		11

#define MG(v)  TSRMG(milter_globals_id, zend_milter_globals *, v)

typedef struct {
    pthread_t thread;
	MUTEX_T receiver;
	MUTEX_T sender;
	SMFICTX *ctx;
	sfsistat retval;
	int message;
	void **args;
} worker_thread;

#endif
