/*
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Authors: Jakub Zelenka <bukka@php.net>                               |
   +----------------------------------------------------------------------+
*/

#include "php.h"
#include "php_io.h"
#include "php_io_internal.h"
#include <sys/stat.h>

#ifdef PHP_WIN32
#include <io.h>
#include <winsock2.h>
#else
#include <unistd.h>
#endif

/* Global instance - initialized at compile time */
static php_io php_io_instance = {
	.copy = PHP_IO_PLATFORM_COPY_OPS,
	.platform_name = PHP_IO_PLATFORM_NAME,
};

/* Get global instance */
PHPAPI php_io *php_io_get(void)
{
	return &php_io_instance;
}

/* Detect file descriptor type */
PHPAPI php_io_fd_type php_io_detect_fd_type(int fd)
{
	struct stat st;

	if (fstat(fd, &st) != 0) {
		/* Can't stat - assume generic (safest fallback) */
		return PHP_IO_FD_GENERIC;
	}

	if (S_ISREG(st.st_mode)) {
		return PHP_IO_FD_FILE;
	}

	/* Everything else (socket, pipe, fifo, char device, etc.) is generic */
	return PHP_IO_FD_GENERIC;
}

/* High-level copy function with dispatch */
PHPAPI zend_result php_io_copy(int src_fd, php_io_fd_type src_type, int dest_fd,
		php_io_fd_type dest_type, size_t len, size_t *copied)
{
	php_io *io = php_io_get();

	/* Dispatch to appropriate copy function based on fd types */
	if (src_type == PHP_IO_FD_FILE && dest_type == PHP_IO_FD_FILE) {
		return io->copy.file_to_file(src_fd, dest_fd, len, copied);
	} else if (src_type == PHP_IO_FD_FILE && dest_type == PHP_IO_FD_GENERIC) {
		return io->copy.file_to_generic(src_fd, dest_fd, len, copied);
	} else if (src_type == PHP_IO_FD_GENERIC && dest_type == PHP_IO_FD_FILE) {
		return io->copy.generic_to_file(src_fd, dest_fd, len, copied);
	} else {
		/* generic to generic */
		return io->copy.generic_to_generic(src_fd, dest_fd, len, copied);
	}
}

/* Generic read/write fallback implementation */
zend_result php_io_generic_copy_fallback(int src_fd, int dest_fd, size_t len, size_t *copied)
{
	char buf[8192];
	size_t total_copied = 0;
	size_t remaining = len;

	while (remaining > 0 && total_copied < len) {
		size_t to_read = remaining < sizeof(buf) ? remaining : sizeof(buf);
		ssize_t bytes_read = read(src_fd, buf, to_read);

		if (bytes_read <= 0) {
			break; /* EOF or error */
		}

		ssize_t bytes_written = write(dest_fd, buf, bytes_read);
		if (bytes_written <= 0) {
			break; /* Error */
		}

		total_copied += bytes_written;
		remaining -= bytes_written;

		if (bytes_written != bytes_read) {
			break; /* Partial write */
		}
	}

	*copied = total_copied;
	return total_copied > 0 ? SUCCESS : FAILURE;
}
