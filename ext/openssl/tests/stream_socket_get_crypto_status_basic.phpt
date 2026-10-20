--TEST--
stream_socket_get_crypto_status(): constants and behavior on a non-crypto stream
--EXTENSIONS--
openssl
--FILE--
<?php
/* The status constants. */
var_dump(STREAM_CRYPTO_STATUS_NONE);
var_dump(STREAM_CRYPTO_STATUS_WANT_READ);
var_dump(STREAM_CRYPTO_STATUS_WANT_WRITE);

/* A plain (non-SSL) socket has no pending crypto operation, so the status
 * is STREAM_CRYPTO_STATUS_NONE. */
$server = stream_socket_server('tcp://127.0.0.1:0', $errno, $errstr);
var_dump(@stream_socket_get_crypto_status($server) === STREAM_CRYPTO_STATUS_NONE);
fclose($server);
?>
--EXPECT--
int(0)
int(1)
int(2)
bool(true)
