/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright © Zend Technologies Ltd., a subsidiary company of          |
   |     Perforce Software, Inc., and Contributors.                       |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Authors: Ilija Tovilo <ilutov@php.net>                               |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_PERF_STAT_H
#define ZEND_PERF_STAT_H

#include "zend_portability.h"

# if !(HAVE_FCNTL_H && HAVE_SYS_SELECT_H && HAVE_SYS_STAT_H && HAVE_SYS_TIME_H && HAVE_UNISTD_H)
static void zend_perf_stat_enable(void) {}
static void zend_perf_stat_disable(void) {}
# else

# include <fcntl.h>
# include <sys/select.h>
# include <sys/stat.h>
# include <unistd.h>

# include "Zend/zend.h"

# define ZPS_CTL_FIFO_ENV "PERF_STAT_CTL_FIFO"
# define ZPS_ACK_FIFO_ENV "PERF_STAT_ACK_FIFO"

static int ctl_fd = -2;
static int ack_fd = -2;

static int zps_open_fifo(const char *env_name, int flags)
{
	const char *path = getenv(env_name);

	if (path == NULL || path[0] == '\0') {
		return -1;
	}

# ifdef O_CLOEXEC
	flags |= O_CLOEXEC;
# endif
	int fd = open(path, flags | O_NONBLOCK);
	if (fd < 0) {
		fprintf(stderr, "Failed to open fifo %s\n", path);
		zend_bailout();
	}

	struct stat st;
	if (fstat(fd, &st) != 0 || !S_ISFIFO(st.st_mode)) {
		close(fd);
		fprintf(stderr, "File %s is not a fifo\n", path);
		zend_bailout();
	}
	return fd;
}

static void zps_init(void)
{
	if (ctl_fd == -2) {
		ctl_fd = zps_open_fifo(ZPS_CTL_FIFO_ENV, O_WRONLY);
	}
	if (ack_fd == -2) {
		ack_fd = zps_open_fifo(ZPS_ACK_FIFO_ENV, O_RDONLY);
	}
}

static void zps_wait_ack(void)
{
	if (ack_fd < 0) {
		return;
	}

	struct timeval timeout;
	timeout.tv_sec = 1;
	timeout.tv_usec = 0;

	fd_set readfds;
	FD_ZERO(&readfds);
	FD_SET(ack_fd, &readfds);
	if (select(ack_fd + 1, &readfds, NULL, NULL, &timeout) <= 0) {
		return;
	}

	char ack[sizeof("ack\n") - 1];
	ssize_t bytes_read;
	while ((bytes_read = read(ack_fd, ack, sizeof(ack))) > 0) {
		for (ssize_t i = 0; i < bytes_read; i++) {
			if (ack[i] == '\n') {
				return;
			}
		}
	}
}

static void zps_control(const char *command, size_t command_len)
{
	zps_init();

	if (ctl_fd < 0) {
		return;
	}

	if (write(ctl_fd, command, command_len) != (ssize_t) command_len) {
		close(ctl_fd);
		ctl_fd = -1;
		return;
	}

	zps_wait_ack();
}

static void zend_perf_stat_enable(void)
{
	static const char command[] = "enable\n";

	zps_control(command, sizeof(command) - 1);
}

static void zend_perf_stat_disable(void)
{
	static const char command[] = "disable\n";

	zps_control(command, sizeof(command) - 1);
}

# endif
#endif
