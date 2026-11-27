--TEST--
socket_addrinfo_bind(), socket_addrinfo_connect(), socket_create_listen(): failures return false cleanly
--EXTENSIONS--
sockets
--SKIPIF--
<?php
if (strtolower(substr(PHP_OS, 0, 3)) == 'win') die('skip not for Windows');
?>
--FILE--
<?php
$l = socket_create(AF_INET, SOCK_STREAM, 0);
socket_bind($l, '127.0.0.1', 0);
socket_getsockname($l, $addr, $port);
socket_listen($l);

$ai = socket_addrinfo_lookup('127.0.0.1', (string) $port, ['ai_socktype' => SOCK_STREAM, 'ai_family' => AF_INET]);
var_dump(@socket_addrinfo_bind($ai[0]));
$ai = socket_addrinfo_lookup('127.0.0.1', '1', ['ai_socktype' => SOCK_STREAM, 'ai_family' => AF_INET]);
var_dump(@socket_addrinfo_connect($ai[0]));
var_dump(@socket_create_listen($port));

// The descriptor numbers are not closed twice: a socket created afterwards still works
$s = socket_create(AF_INET, SOCK_STREAM, 0);
var_dump(socket_bind($s, '127.0.0.1', 0));
?>
--EXPECT--
bool(false)
bool(false)
bool(false)
bool(true)
