--TEST--
bool socket_shutdown ( resource $socket [, int $how = 2 ] ) ;
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br> - #phparty7 - @phpsp - novatec/2015 - sao paulo - br
--SKIPIF--
<?php
if (!extension_loaded('sockets')) {
  die('SKIP sockets extension not available.');
}
if(substr(PHP_OS, 0, 3) != 'WIN' ) {
    die('skip windows only test');
}
?>
--FILE--
<?php
$host = "yahoo.com";
$port = 80;

$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
$socketConn = socket_connect($socket, $host, $port);
var_dump(socket_shutdown($socket,0));
socket_close($socket);

$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
$socketConn = socket_connect($socket, $host, $port);
var_dump(socket_shutdown($socket,1));
socket_close($socket);

$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
$socketConn = socket_connect($socket, $host, $port);
var_dump(socket_shutdown($socket,2));
socket_close($socket);

$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
var_dump(socket_shutdown($socket,0));

$socketConn = socket_connect($socket, $host, $port);
var_dump(socket_shutdown($socket,-1));
socket_close($socket);
?>
--CLEAN--
<?php
unset($host);
unset($port);
unset($socket);
unset($socketConn);
?>
--EXPECTF--
bool(true)
bool(true)
bool(true)

Warning: socket_shutdown(): Unable to shutdown socket [%d]: A request to send or receive data was disallowed because the socket is not connected and (when sending on a datagram socket using a sendto call) no address was supplied in %s on line %d
bool(false)

Warning: socket_shutdown(): Unable to shutdown socket [%d]: An invalid argument was supplied in %s on line %d
bool(false)
