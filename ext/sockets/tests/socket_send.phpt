--TEST--
int socket_send ( resource $socket , string $buf , int $len , int $flags );
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br> - #phparty7 - @phpsp - novatec/2015 - sao paulo - br
--SKIPIF--
<?php
if (getenv("SKIP_ONLINE_TESTS")) die("skip online test");
if (!extension_loaded('sockets')) {
  die('SKIP sockets extension not available.');
}
if(substr(PHP_OS, 0, 3) == 'WIN' ) {
	die('skip not for windows');
}
?>
--FILE--
<?php
$port = 80;
$host = "yahoo.com";
$stringSocket = "send_socket_to_connected_socket";
$stringSocketLenght = strlen($stringSocket);

$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
$socketConn = socket_connect($socket, $host, $port);

if(socket_send($socket, $stringSocket, $stringSocketLenght, MSG_OOB)===$stringSocketLenght){
  print("okey\n");
}

if(socket_send($socket, $stringSocket, $stringSocketLenght, MSG_EOR)===$stringSocketLenght){
  print("okey\n");
}

if(socket_send($socket, $stringSocket, $stringSocketLenght, MSG_EOF)===$stringSocketLenght){
  print("okey\n");
}

if(socket_send($socket, $stringSocket, $stringSocketLenght, MSG_DONTROUTE)===$stringSocketLenght){
  print("okey\n");
}
?>
<?php
socket_close($socket);
unset($port);
unset($host);
unset($stringSocket);
unset($stringSocketLenght);
unset($socket);
unset($socketConn);
?>
--EXPECTF--
okey
okey
okey
okey
