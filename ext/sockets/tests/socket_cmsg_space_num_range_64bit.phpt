--TEST--
socket_cmsg_space() rejects a $num larger than INT_MAX on 64-bit platforms
--EXTENSIONS--
sockets
--SKIPIF--
<?php
if (PHP_INT_SIZE < 8) {
    die('skip 64-bit only');
}
?>
--FILE--
<?php
// $num is range checked before the level/type pair is looked up
try {
    socket_cmsg_space(SOL_SOCKET, 0, PHP_INT_MAX);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
ValueError: socket_cmsg_space(): Argument #3 ($num) must be between -2147483648 and 2147483647
