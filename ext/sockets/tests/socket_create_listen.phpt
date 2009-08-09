--TEST--
Test if socket binds on 31338
--SKIPIF--
<?php
if (!extension_loaded('sockets')) {
    die('SKIP The sockets extension is not loaded.');
}
--FILE--
<?php
$sock = socket_create_listen(31338);
socket_getsockname($sock, $addr, $port); 
var_dump($addr, $port);
--EXPECT--
string(7) "0.0.0.0"
int(31338)
--CREDITS--
Till Klampaeckel, till@php.net
PHP Testfest Berlin 2009-05-09
