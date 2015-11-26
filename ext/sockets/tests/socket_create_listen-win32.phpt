--TEST--
Test if socket binds on 31338
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
$sock = socket_create_listen(31338);
socket_getsockname($sock, $addr, $port); 
var_dump($addr, $port);
--EXPECT--
string(9) "127.0.0.1"
int(31338)
--CREDITS--
Till Klampaeckel, till@php.net
PHP Testfest Berlin 2009-05-09
