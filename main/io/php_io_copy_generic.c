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

#include "php_io_internal.h"

/* Generic implementations using standard read/write */

zend_result php_io_copy_generic_file_to_file(int src_fd, int dest_fd, size_t len, size_t *copied)
{
    return php_io_generic_copy_fallback(src_fd, dest_fd, len, copied);
}

zend_result php_io_copy_generic_file_to_socket(int src_fd, int dest_fd, size_t len, size_t *copied)
{
    return php_io_generic_copy_fallback(src_fd, dest_fd, len, copied);
}

zend_result php_io_copy_generic_socket_to_fd(int src_fd, int dest_fd, size_t len, size_t *copied)
{
    return php_io_generic_copy_fallback(src_fd, dest_fd, len, copied);
}

/* Generic copy operations vtable */
static php_io_copy_ops php_io_copy_ops_generic = {
    .file_to_file = php_io_copy_generic_file_to_file,
    .file_to_socket = php_io_copy_generic_file_to_socket,
    .socket_to_fd = php_io_copy_generic_socket_to_fd,
};

/* Generic ring operations vtable (no ring support) */
static php_io_ring_ops php_io_ring_ops_generic = {
    .create = NULL,
    .submit = NULL,
    .wait_cqe = NULL,
    .cqe_seen = NULL,
    .destroy = NULL,
    .capabilities = 0,
};

/* Generic php_io instance */
static php_io php_io_instance_generic = {
    .copy = {
        .file_to_file = php_io_copy_generic_file_to_file,
        .file_to_socket = php_io_copy_generic_file_to_socket,
        .socket_to_fd = php_io_copy_generic_socket_to_fd,
    },
    .ring = {
        .create = NULL,
        .submit = NULL,
        .wait_cqe = NULL,
        .cqe_seen = NULL,
        .destroy = NULL,
        .capabilities = 0,
    },
    .platform_name = "generic",
    .capabilities = 0,
};

/* Registration function */
php_io* php_io_register_generic(void)
{
    return &php_io_instance_generic;
}
