<?php

/*
 * Shared --SKIPIF-- checks for the dtls:// stream transport tests.
 *
 * Include it from a test's --SKIPIF-- section:
 *
 *     --SKIPIF--
 *     <?php require __DIR__ . '/dtls_skipif.inc'; ?>
 *
 * Skips when the openssl extension does not expose a usable dtls:// transport,
 * for example an OpenSSL build with OPENSSL_NO_DTLS or a php-src that does not
 * register the transport yet. Tests that also spawn a peer process should add
 * their own proc_open() check on top of this one.
 */

if (!extension_loaded('openssl')) {
    die('skip openssl extension not available');
}

if (!in_array('dtls', stream_get_transports(), true)) {
    die('skip dtls:// stream transport not available');
}
