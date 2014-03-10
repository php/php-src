--TEST--
Test if socket_set_option() returns 'unable to set socket option' failure for invalid options
--SKIPIF--
<?php
if (!extension_loaded('sockets')) {
    die('SKIP sockets extension not available.');
}
if (PHP_OS == 'Darwin') {
    die('skip Not for OSX');
}
$filename = dirname(__FILE__) . '/006_root_check.tmp';
$fp = fopen($filename, 'w');
fclose($fp);
if (fileowner($filename) == 0) {
    unlink ($filename);
    die('SKIP Test cannot be run as root.');
}
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
--CLEAN--
<?php
unlink(dirname(__FILE__) . '/006_root_check.tmp');
--EXPECTF--
Warning: socket_set_option(): unable to set socket option [%d]: Permission denied in %s on line %d
--CREDITS--
Moritz Neuhaeuser, info@xcompile.net
PHP Testfest Berlin 2009-05-10
