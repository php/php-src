--TEST--
Test parameter handling in socket_create_pair()
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') {
    die('skip.. Not valid for non Windows');
}
if (!extension_loaded('sockets')) {
    die('SKIP The sockets extension is not loaded.');
}
--FILE--
<?php

var_dump(socket_create_pair(AF_INET, null, null, $sockets));

try {
    var_dump(socket_create_pair(31337, null, null, $sockets));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

try {
    var_dump(socket_create_pair(AF_INET, 31337, 0, $sockets));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
?>
--EXPECT--
bool(true)
socket_create_pair(): Argument #1 ($domain) must be one of AF_UNIX, AF_INET6, or AF_INET
socket_create_pair(): Argument #2 ($type) must be one of SOCK_STREAM, SOCK_DGRAM, SOCK_SEQPACKET, SOCK_RAW, or SOCK_RDM
--CREDITS--
Till Klampaeckel, till@php.net
Berlin TestFest 2009
