--TEST--
int socket_send ( resource $socket , string $buf , int $len , int $flags );
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br> - #phparty7 - @phpsp - novatec/2015 - sao paulo - br
--EXTENSIONS--
sockets
--SKIPIF--
<?php
if (getenv("SKIP_ONLINE_TESTS")) die("skip online test");

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

if(!defined('MSG_EOR') || socket_send($socket, $stringSocket, $stringSocketLength, MSG_EOR)===$stringSocketLength){
  print("okey\n");
}

if(!defined('MSG_EOF') || socket_send($socket, $stringSocket, $stringSocketLength, MSG_EOF)===$stringSocketLength){
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
okey
okey
