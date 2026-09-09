--TEST--
AF_VSOCK rejects a context ID or port beyond the 32 bit range
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
    socket_bind($socket, (string) VMADDR_CID_ANY, 4294967296);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    socket_sendto($socket, 'x', 1, 0, (string) VMADDR_CID_ANY, 4294967296);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    socket_bind($socket, (string) VMADDR_CID_ANY, -2147483649);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    socket_bind($socket, '-2147483649', 0);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    socket_bind($socket, '4294967296', 0);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

socket_close($socket);
?>
--EXPECT--
ValueError: socket_bind(): Argument #3 ($port) must be between -2147483648 and 4294967295
ValueError: socket_sendto(): Argument #6 ($port) must be between -2147483648 and 4294967295
ValueError: socket_bind(): Argument #3 ($port) must be between -2147483648 and 4294967295
ValueError: socket_bind(): Argument #2 ($address) must be a numeric context ID between -2147483648 and 4294967295
ValueError: socket_bind(): Argument #2 ($address) must be a numeric context ID between -2147483648 and 4294967295
