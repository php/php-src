--TEST--
Test if socket_set_option() returns 'Unable to set socket option' failure for invalid options
--EXTENSIONS--
sockets
--SKIPIF--
<?php

$filename = __FILE__ . '.root_check.tmp';
$fp = fopen($filename, 'w');
fclose($fp);
if (fileowner($filename) == 0) {
    unlink($filename);
    die('SKIP Test cannot be run as root.');
}
unlink($filename);
?>
--FILE--
<?php
$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
if (!$socket) {
        die('Unable to create AF_INET socket [socket]');
}

socket_set_option( $socket, SOL_SOCKET, 1, 1);
socket_close($socket);
?>
--EXPECTF--
Warning: socket_set_option(): Unable to set socket option [%d]: Permission denied in %s on line %d
--CREDITS--
Moritz Neuhaeuser, info@xcompile.net
PHP Testfest Berlin 2009-05-10
