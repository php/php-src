
	/* $Id: fpm_process_ctl.h,v 1.6 2008/07/20 21:33:10 anight Exp $ */
	/* (c) 2007,2008 Andrei Nigmatulin */

#ifndef FPM_PROCESS_CTL_H
#define FPM_PROCESS_CTL_H 1

struct fpm_child_s;

void fpm_pctl(int new_state, int action);
int fpm_pctl_can_spawn_children();
int fpm_pctl_kill(pid_t pid, int how);
void fpm_pctl_heartbeat(int fd, short which, void *arg);
int fpm_pctl_child_exited();
int fpm_pctl_init_main();


enum {
	FPM_PCTL_STATE_UNSPECIFIED,
	FPM_PCTL_STATE_NORMAL,
	FPM_PCTL_STATE_RELOADING,
	FPM_PCTL_STATE_TERMINATING,
	FPM_PCTL_STATE_FINISHING
};

enum {
	FPM_PCTL_ACTION_SET,
	FPM_PCTL_ACTION_TIMEOUT,
	FPM_PCTL_ACTION_LAST_CHILD_EXITED
};

enum {
	FPM_PCTL_TERM,
	FPM_PCTL_STOP,
	FPM_PCTL_CONT
};

#endif

