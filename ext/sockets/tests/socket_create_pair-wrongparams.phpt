--TEST--
Test parameter handling in socket_create_pair()
--EXTENSIONS--
sockets
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip.. Not valid for Windows');
}
?>
--FILE--
<?php

var_dump(socket_create_pair(AF_INET, 0, 0, $sockets));

try {
    var_dump(socket_create_pair(31337, 0, 0, $sockets));
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    var_dump(socket_create_pair(AF_INET, 31337, 0, $sockets));
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECTF--
Warning: socket_create_pair(): Unable to create socket pair [%d]: %s %r(not supported|wrong type for socket)%r in %s on line %d
bool(false)
ValueError: socket_create_pair(): Argument #1 ($domain) must be one of AF_UNIX, AF_INET6, or AF_INET
ValueError: socket_create_pair(): Argument #2 ($type) must be one of SOCK_STREAM, SOCK_DGRAM, SOCK_SEQPACKET, SOCK_RAW, or SOCK_RDM%A
--CREDITS--
Till Klampaeckel, till@php.net
Berlin TestFest 2009
