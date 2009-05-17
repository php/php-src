--TEST--
Test for socket_create_pair()
--SKIPIF--
<?php
if (!extension_loaded('sockets')) {
    die('SKIP The sockets extension is not loaded.');
}
--FILE--
<?php
$sockets = array();
if (strtolower(substr(PHP_OS, 0, 3)) == 'win') {
    $domain = AF_INET;
} else {
    $domain = AF_UNIX;
}
socket_create_pair($domain, SOCK_STREAM, 0, $sockets);
var_dump($sockets);
--EXPECT--
array(2) {
  [0]=>
  resource(4) of type (Socket)
  [1]=>
  resource(5) of type (Socket)
}
