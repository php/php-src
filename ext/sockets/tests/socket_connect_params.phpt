--TEST--
ext/sockets - socket_connect - test with empty parameters
--CREDITS--
Florian Anderiasch
fa@php.net
--SKIPIF--
<?php
    if (!extension_loaded('sockets')) {
        die('skip - sockets extension not available.');
    }
?>
--FILE--
<?php

$s_c = socket_create_listen(0);
socket_getsockname($s_c, $addr, $port);

// wrong parameter count
try {
    socket_connect($s_c);
} catch (\ArgumentCountError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    socket_connect($s_c, '0.0.0.0');
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
$s_w = socket_connect($s_c, '0.0.0.0', $port);

socket_close($s_c);
?>
--EXPECTF--
socket_connect() expects at least 2 arguments, 1 given
socket_connect(): Argument #3 ($port) cannot be null when the socket type is AF_INET

Warning: socket_connect(): unable to connect [%i]: %a in %s on line %d
