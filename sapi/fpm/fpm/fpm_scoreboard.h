	/* (c) 2009 Jerome Loyet */

#ifndef FPM_SCOREBOARD_H
#define FPM_SCOREBOARD_H 1

#include <sys/time.h>
#ifdef HAVE_TIMES
#include <sys/times.h>
#endif

#include "fpm_request.h"
#include "fpm_worker_pool.h"
#include "fpm_atomic.h"

#define FPM_SCOREBOARD_ACTION_SET 0
#define FPM_SCOREBOARD_ACTION_INC 1

#define FPM_SCOREBOARD_LOCK_HANG 0
#define FPM_SCOREBOARD_LOCK_NOHANG 1

struct fpm_scoreboard_proc_s {
	union {
		atomic_t lock;
		char dummy[16];
	};
	int used;
	time_t start_epoch;
	pid_t pid;
	unsigned long requests;
	enum fpm_request_stage_e request_stage;
	struct timeval accepted;
	struct timeval duration;
	time_t accepted_epoch;
	struct timeval tv;
	char request_uri[128];
	char query_string[512];
	char request_method[16];
	size_t content_length; /* used with POST only */
	char script_filename[256];
	char auth_user[32];
#ifdef HAVE_TIMES
	struct tms cpu_accepted;
	struct timeval cpu_duration;
	struct tms last_request_cpu;
	struct timeval last_request_cpu_duration;
#endif
	size_t memory;
};

struct fpm_scoreboard_s {
	union {
		atomic_t lock;
		char dummy[16];
	};
	char pool[32];
	int pm;
	time_t start_epoch;
	int idle;
	int active;
	int active_max;
	unsigned long int requests;
	unsigned int max_children_reached;
	int lq;
	int lq_max;
	unsigned int lq_len;
	unsigned int nprocs;
	int free_proc;
	unsigned long int slow_rq;
	struct fpm_scoreboard_s *shared;
	struct fpm_scoreboard_proc_s procs[] ZEND_ELEMENT_COUNT(nprocs);
};

int fpm_scoreboard_init_main(void);
int fpm_scoreboard_init_child(struct fpm_worker_pool_s *wp);

void fpm_scoreboard_update_begin(struct fpm_scoreboard_s *scoreboard);
void fpm_scoreboard_update_commit(int idle, int active, int lq, int lq_len, int requests, int max_children_reached, int slow_rq, int action, struct fpm_scoreboard_s *scoreboard);
void fpm_scoreboard_update(int idle, int active, int lq, int lq_len, int requests, int max_children_reached, int slow_rq, int action, struct fpm_scoreboard_s *scoreboard);

struct fpm_scoreboard_s *fpm_scoreboard_get(void);
struct fpm_scoreboard_proc_s *fpm_scoreboard_proc_get(struct fpm_scoreboard_s *scoreboard, int child_index);
struct fpm_scoreboard_proc_s *fpm_scoreboard_proc_get_from_child(struct fpm_child_s *child);

struct fpm_scoreboard_s *fpm_scoreboard_acquire(struct fpm_scoreboard_s *scoreboard, int nohang);
void fpm_scoreboard_release(struct fpm_scoreboard_s *scoreboard);
struct fpm_scoreboard_proc_s *fpm_scoreboard_proc_acquire(struct fpm_scoreboard_s *scoreboard, int child_index, int nohang);
void fpm_scoreboard_proc_release(struct fpm_scoreboard_proc_s *proc);

void fpm_scoreboard_free(struct fpm_worker_pool_s *wp);

void fpm_scoreboard_child_use(struct fpm_child_s *child, pid_t pid);

void fpm_scoreboard_proc_free(struct fpm_child_s *child);
int fpm_scoreboard_proc_alloc(struct fpm_child_s *child);

struct fpm_scoreboard_s *fpm_scoreboard_copy(struct fpm_scoreboard_s *scoreboard, int copy_procs);
void fpm_scoreboard_free_copy(struct fpm_scoreboard_s *scoreboard);

#ifdef HAVE_TIMES
float fpm_scoreboard_get_tick(void);
#endif

#endif
