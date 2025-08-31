--TEST--
void socket_clear_error ([ resource $socket ] ) ;
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br> - #phparty7 - @phpsp - novatec/2015 - sao paulo - br
--EXTENSIONS--
sockets
--SKIPIF--
<?php

if (substr(PHP_OS, 0, 3) == 'WIN' ) {
    die('skip non-windows only test');
}
?>
--FILE--
<?php
$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
socket_set_option($socket, -1, -1, -1);
var_dump(socket_last_error($socket));
socket_clear_error($socket);
var_dump(socket_last_error($socket));
?>
--EXPECTF--
Warning: socket_set_option(): Unable to set socket option [%s]: %s in %s on line %d
int(%d)
int(0)
