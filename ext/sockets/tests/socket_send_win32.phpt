--TEST--
int socket_send ( resource $socket , string $buf , int $len , int $flags );
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
$port = 80;
$host = "yahoo.com";
$stringSocket = "send_socket_to_connected_socket";
$stringSocketLength = strlen($stringSocket);

$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
$socketConn = socket_connect($socket, $host, $port);

if(socket_send($socket, $stringSocket, $stringSocketLength, MSG_OOB)===$stringSocketLength){
  print("okey\n");
}

if(socket_send($socket, $stringSocket, $stringSocketLength, MSG_DONTROUTE)===$stringSocketLength){
  print("okey\n");
}
?>
<?php
socket_close($socket);
unset($port);
unset($host);
unset($stringSocket);
unset($stringSocketLength);
unset($socket);
unset($socketConn);
?>
--EXPECT--
okey
okey
