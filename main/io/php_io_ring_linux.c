/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: PHP Development Team                                         |
   +----------------------------------------------------------------------+
*/

#ifdef __linux__

#include "php_io_internal.h"

#ifdef HAVE_IO_URING
#include <liburing.h>
#include <sys/utsname.h>

typedef struct {
    struct io_uring ring;
} php_io_ring_linux;

static php_io_ring* php_io_ring_linux_create(int queue_depth)
{
    php_io_ring_linux *lr = emalloc(sizeof(php_io_ring_linux));
    
    if (io_uring_queue_init(queue_depth, &lr->ring, 0) == 0) {
        return (php_io_ring*)lr;
    }
    
    efree(lr);
    return NULL;
}

static zend_result php_io_ring_linux_submit(php_io_ring *ring, php_io_sqe *sqe)
{
    php_io_ring_linux *lr = (php_io_ring_linux*)ring;
    struct io_uring_sqe *uring_sqe = io_uring_get_sqe(&lr->ring);
    
    if (!uring_sqe) {
        return FAILURE;
    }
    
    uring_sqe->user_data = sqe->user_data;
    
    switch (sqe->op) {
        case PHP_IO_OP_SPLICE:
            io_uring_prep_splice(uring_sqe, sqe->fd, sqe->params.splice.src_offset,
                                 sqe->params.splice.dest_fd, sqe->params.splice.dest_offset,
                                 sqe->params.splice.len, 0);
            break;
        case PHP_IO_OP_READ:
            io_uring_prep_read(uring_sqe, sqe->fd, sqe->params.rw.buf,
                               sqe->params.rw.len, sqe->params.rw.offset);
            break;
        case PHP_IO_OP_WRITE:
            io_uring_prep_write(uring_sqe, sqe->fd, sqe->params.rw.buf,
                                sqe->params.rw.len, sqe->params.rw.offset);
            break;
        default:
            return FAILURE;
    }
    
    return io_uring_submit(&lr->ring) > 0 ? SUCCESS : FAILURE;
}

static int php_io_ring_linux_wait_cqe(php_io_ring *ring, php_io_cqe **cqe)
{
    php_io_ring_linux *lr = (php_io_ring_linux*)ring;
    struct io_uring_cqe *uring_cqe;
    static php_io_cqe php_cqe;
    
    int ret = io_uring_wait_cqe(&lr->ring, &uring_cqe);
    if (ret == 0) {
        php_cqe.user_data = uring_cqe->user_data;
        php_cqe.result = uring_cqe->res;
        php_cqe.flags = uring_cqe->flags;
        *cqe = &php_cqe;
        return 1;
    }
    
    return 0;
}

static void php_io_ring_linux_cqe_seen(php_io_ring *ring, php_io_cqe *cqe)
{
    php_io_ring_linux *lr = (php_io_ring_linux*)ring;
    struct io_uring_cqe *uring_cqe;
    
    /* Find the corresponding io_uring cqe and mark it as seen */
    if (io_uring_peek_cqe(&lr->ring, &uring_cqe) == 0) {
        if (uring_cqe->user_data == cqe->user_data) {
            io_uring_cqe_seen(&lr->ring, uring_cqe);
        }
    }
}

static void php_io_ring_linux_destroy(php_io_ring *ring)
{
    php_io_ring_linux *lr = (php_io_ring_linux*)ring;
    io_uring_queue_exit(&lr->ring);
    efree(lr);
}

/* Check if io_uring is available and supports necessary features */
static zend_bool php_io_ring_linux_detect_support(void)
{
    struct utsname uts;
    if (uname(&uts) != 0) {
        return 0;
    }
    
    int major, minor, patch;
    if (sscanf(uts.release, "%d.%d.%d", &major, &minor, &patch) != 3) {
        return 0;
    }
    
    /* Require kernel 5.7+ for reliable splice support */
    int version = major * 1000000 + minor * 1000 + patch;
    if (version < 5007000) {
        return 0;
    }
    
    /* Test if io_uring actually works */
    struct io_uring test_ring;
    if (io_uring_queue_init(1, &test_ring, 0) == 0) {
        io_uring_queue_exit(&test_ring);
        return 1;
    }
    
    return 0;
}

#endif /* HAVE_IO_URING */

void php_io_register_ring(php_io_ring_ops *ops, uint32_t *capabilities)
{
#ifdef HAVE_IO_URING
    if (php_io_ring_linux_detect_support()) {
        ops->create = php_io_ring_linux_create;
        ops->submit = php_io_ring_linux_submit;
        ops->wait_cqe = php_io_ring_linux_wait_cqe;
        ops->cqe_seen = php_io_ring_linux_cqe_seen;
        ops->destroy = php_io_ring_linux_destroy;
        ops->capabilities = PHP_IO_RING_CAP_SPLICE | PHP_IO_RING_CAP_READ | 
                           PHP_IO_RING_CAP_WRITE | PHP_IO_RING_CAP_BATCH;
        
        *capabilities |= PHP_IO_CAP_RING_SUPPORT;
        return;
    }
#endif
    
    /* No ring support available */
    ops->create = NULL;
    ops->submit = NULL;
    ops->wait_cqe = NULL;
    ops->cqe_seen = NULL;
    ops->destroy = NULL;
    ops->capabilities = 0;
}

#endif /* __linux__ */
