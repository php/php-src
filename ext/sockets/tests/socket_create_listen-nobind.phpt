--TEST--
Test if socket_create_listen() returns false, when it cannot bind to the port.
--SKIPIF--
<?php
if (!extension_loaded('sockets')) {
    die('SKIP The sockets extension is not loaded.');
}
$filename = dirname(__FILE__) . '/006_root_check.tmp';
$fp = fopen($filename, 'w');
fclose($fp);
if (fileowner($filename) == 0) {
    unlink ($filename);
    die('SKIP Test cannot be run as root.');
}
--FILE--
<?php
$sock = socket_create_listen(80);
--EXPECTF--
Warning: socket_create_listen(): unable to bind to given address [13]: Permission denied in %s on line %d
--CLEAN--
<?php
unlink(dirname(__FILE__) . '/006_root_check.tmp');
--CREDITS--
Till Klampaeckel, till@php.net
PHP Testfest Berlin 2009-05-09