--TEST--
void socket_clear_error ([ resource $socket ] ) ;
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br> - #phparty7 - @phpsp - novatec/2015 - sao paulo - br
--SKIPIF--
<?php
if (!extension_loaded('sockets')) {
  die('SKIP sockets extension not available.');
}
if (!defined("SO_BINDTODEVICE")) {
  die('SKIP SO_BINDTODEVICE not supported on this platform.');
}
if (!function_exists("posix_getuid") || posix_getuid() != 0) {
  die('SKIP SO_BINDTODEVICE requires root permissions.');
}
?>
--FILE--
<?php
$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
$socketConn = socket_connect($socket, "127.0.0.1", 21);
var_dump(socket_last_error($socket));
socket_clear_error($socket);
var_dump(socket_last_error($socket));
?>
--CLEAN--
<?php
socket_close($socket);
unset($socket);
unset($socketConn);
?>
--EXPECTF--
Warning: socket_connect(): unable to connect [%d]: Connection refused in %s on line %d
int(%d)
int(%d)
