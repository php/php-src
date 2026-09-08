--TEST--
AF_VSOCK rejects a port beyond the 32 bit range
--EXTENSIONS--
sockets
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) {
    die('skip this test is for 64bit platform only');
}
if (!defined('AF_VSOCK')) {
    die('skip AF_VSOCK not available');
}
if (@socket_create(AF_VSOCK, SOCK_STREAM, 0) === false) {
    die('skip no vsock transport available');
}
?>
--FILE--
<?php
$socket = socket_create(AF_VSOCK, SOCK_STREAM, 0);

try {
    socket_bind($socket, (string) VMADDR_CID_LOCAL, 4294967296);
} catch (ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    socket_sendto($socket, 'x', 1, 0, (string) VMADDR_CID_LOCAL, 4294967296);
} catch (ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

socket_close($socket);
?>
--EXPECT--
ValueError: socket_bind(): Argument #3 ($port) must be between 0 and 4294967295
ValueError: socket_sendto(): Argument #6 ($port) must be between 0 and 4294967295
