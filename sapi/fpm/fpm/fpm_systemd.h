#ifndef FPM_SYSTEMD_H
#define FPM_SYSTEMD_H 1

#include "fpm_events.h"

/* 10s (in ms) heartbeat for systemd status */
#define FPM_SYSTEMD_DEFAULT_HEARTBEAT (10000)

void fpm_systemd_heartbeat(struct fpm_event_s *ev, short which, void *arg);
int fpm_systemd_conf(void);

#endif
