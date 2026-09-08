--TEST--
AF_VSOCK address and port validation
--EXTENSIONS--
sockets
--SKIPIF--
<?php
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
    socket_bind($socket, 'host', 1024);
} catch (ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    socket_bind($socket, '1.5', 1024);
} catch (ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    socket_bind($socket, '4294967296', 1024);
} catch (ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    socket_connect($socket, (string) VMADDR_CID_LOCAL);
} catch (ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    socket_sendto($socket, 'x', 1, 0, 'host', 1024);
} catch (ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

var_dump(socket_bind($socket, (string) VMADDR_CID_LOCAL, VMADDR_PORT_ANY));

socket_close($socket);
?>
--EXPECT--
ValueError: socket_bind(): Argument #2 ($address) must be a numeric context ID between 0 and 4294967295
ValueError: socket_bind(): Argument #2 ($address) must be a numeric context ID between 0 and 4294967295
ValueError: socket_bind(): Argument #2 ($address) must be a numeric context ID between 0 and 4294967295
ValueError: socket_connect(): Argument #3 ($port) cannot be null when the socket type is AF_VSOCK
ValueError: socket_sendto(): Argument #5 ($address) must be a numeric context ID between 0 and 4294967295
bool(true)
