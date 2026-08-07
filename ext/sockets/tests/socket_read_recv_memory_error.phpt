--TEST--
socket_read(), socket_recv(), and socket_recvfrom() throw a catchable MemoryError
--EXTENSIONS--
sockets
--SKIPIF--
<?php
if (!function_exists('socket_create_pair')) die('skip socket_create_pair() not available');
?>
--INI--
memory_limit=64M
opcache.enable_cli=0
--FILE--
<?php

$domain = strtoupper(substr(PHP_OS, 0, 3)) === 'WIN' ? AF_INET : AF_UNIX;

socket_create_pair($domain, SOCK_STREAM, 0, $pair);
[$a, $b] = $pair;
socket_write($b, 'ping');

try {
    socket_read($a, PHP_INT_MAX - 1);
} catch (MemoryError $e) {
    echo 'socket_read: ' . $e::class . ': ' . $e->getMessage() . "\n";
}

try {
    socket_recv($a, $buf, PHP_INT_MAX - 1, 0);
} catch (MemoryError $e) {
    echo 'socket_recv: ' . $e::class . ': ' . $e->getMessage() . "\n";
}

try {
    socket_recvfrom($a, $buf, PHP_INT_MAX - 2, 0, $name);
} catch (MemoryError $e) {
    echo 'socket_recvfrom: ' . $e::class . ': ' . $e->getMessage() . "\n";
}

var_dump(socket_read($a, 4));
socket_close($a);
socket_close($b);

?>
--EXPECT--
socket_read: MemoryError: The resulting string is too large to fit in the configured memory limit
socket_recv: MemoryError: The resulting string is too large to fit in the configured memory limit
socket_recvfrom: MemoryError: The resulting string is too large to fit in the configured memory limit
string(4) "ping"
