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

#ifndef PHP_IO_H
#define PHP_IO_H

#include "php.h"

/* Forward declarations */
typedef struct php_io php_io;
typedef struct php_io_ring php_io_ring;

/* Operation types for ring API */
typedef enum {
    PHP_IO_OP_READ,
    PHP_IO_OP_WRITE,
    PHP_IO_OP_SPLICE,
} php_io_op_type;

/* File descriptor types */
typedef enum {
    PHP_IO_FD_FILE,
    PHP_IO_FD_SOCKET,
    PHP_IO_FD_PIPE,
    PHP_IO_FD_UNKNOWN,
} php_io_fd_type;

/* Ring-specific capabilities */
#define PHP_IO_RING_CAP_SPLICE     (1 << 0)
#define PHP_IO_RING_CAP_READ       (1 << 1)
#define PHP_IO_RING_CAP_WRITE      (1 << 2)
#define PHP_IO_RING_CAP_BATCH      (1 << 3)

/* General I/O capabilities */
#define PHP_IO_CAP_RING_SUPPORT    (1 << 0)
#define PHP_IO_CAP_ZERO_COPY       (1 << 1)
#define PHP_IO_CAP_SENDFILE        (1 << 2)

/* Ring API structures */
typedef struct php_io_sqe {
    php_io_op_type op;
    uint64_t user_data;
    int fd;
    union {
        struct {
            int dest_fd;
            size_t len;
            off_t src_offset;
            off_t dest_offset;
        } splice;
        struct {
            void *buf;
            size_t len;
            off_t offset;
        } rw;
    } params;
} php_io_sqe;

typedef struct php_io_cqe {
    uint64_t user_data;
    ssize_t result;
    uint32_t flags;
} php_io_cqe;

/* Ring operations vtable */
typedef struct php_io_ring_ops {
    php_io_ring* (*create)(int queue_depth);
    zend_result (*submit)(php_io_ring *ring, php_io_sqe *sqe);
    int (*wait_cqe)(php_io_ring *ring, php_io_cqe **cqe);
    void (*cqe_seen)(php_io_ring *ring, php_io_cqe *cqe);
    void (*destroy)(php_io_ring *ring);
    uint32_t capabilities;
} php_io_ring_ops;

/* Synchronous copy operations vtable */
typedef struct php_io_copy_ops {
    zend_result (*file_to_file)(int src_fd, int dest_fd, size_t len, size_t *copied);
    zend_result (*file_to_socket)(int src_fd, int dest_fd, size_t len, size_t *copied);
    zend_result (*socket_to_fd)(int src_fd, int dest_fd, size_t len, size_t *copied);
} php_io_copy_ops;

/* Main php_io structure */
typedef struct php_io {
    php_io_copy_ops copy;
    php_io_ring_ops ring;
    const char *platform_name;
    uint32_t capabilities;
} php_io;

/* Factory functions */
PHPAPI php_io* php_io_create(void);
PHPAPI php_io* php_io_get(void);

/* Utility functions */
PHPAPI php_io_fd_type php_io_detect_fd_type(int fd);
PHPAPI zend_result php_io_get_fd_and_type(php_stream *stream, int *fd, php_io_fd_type *type);

/* Ring API helper functions */
static inline void php_io_ring_prep_splice(php_io_sqe *sqe, uint64_t user_data,
                                           int src_fd, int dest_fd, size_t len)
{
    sqe->op = PHP_IO_OP_SPLICE;
    sqe->user_data = user_data;
    sqe->fd = src_fd;
    sqe->params.splice.dest_fd = dest_fd;
    sqe->params.splice.len = len;
    sqe->params.splice.src_offset = 0;
    sqe->params.splice.dest_offset = 0;
}

static inline void php_io_ring_prep_read(php_io_sqe *sqe, uint64_t user_data,
                                         int fd, void *buf, size_t len, off_t offset)
{
    sqe->op = PHP_IO_OP_READ;
    sqe->user_data = user_data;
    sqe->fd = fd;
    sqe->params.rw.buf = buf;
    sqe->params.rw.len = len;
    sqe->params.rw.offset = offset;
}

static inline void php_io_ring_prep_write(php_io_sqe *sqe, uint64_t user_data,
                                          int fd, void *buf, size_t len, off_t offset)
{
    sqe->op = PHP_IO_OP_WRITE;
    sqe->user_data = user_data;
    sqe->fd = fd;
    sqe->params.rw.buf = buf;
    sqe->params.rw.len = len;
    sqe->params.rw.offset = offset;
}

#endif /* PHP_IO_H */
