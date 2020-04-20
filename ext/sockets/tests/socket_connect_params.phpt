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
$s_w = socket_connect();
$s_w = socket_connect($s_c);
$s_w = socket_connect($s_c, '0.0.0.0');
$s_w = socket_connect($s_c, '0.0.0.0', $port);

socket_close($s_c);

?>
--EXPECTF--
Warning: socket_connect() expects at least 2 parameters, 0 given in %s on line %d

Warning: socket_connect() expects at least 2 parameters, 1 given in %s on line %d

Warning: socket_connect(): Socket of type AF_INET requires 3 arguments in %s on line %d

Warning: socket_connect(): unable to connect [%i]: %a in %s on line %d
