--TEST--
Test socket_atmark() function
--EXTENSIONS--
sockets
--FILE--
<?php
$sockets = [];
if (!socket_create_pair(AF_UNIX, SOCK_STREAM, 0, $sockets)) {
    die("socket_create_pair failed");
}

socket_write($sockets[0], "a");
socket_send($sockets[0], "b", 1, MSG_OOB);
socket_write($sockets[0], "c");

$data = "";
var_dump(socket_atmark($sockets[1]));
socket_set_option($sockets[1], SOL_SOCKET, SO_OOBINLINE, 1);
var_dump(socket_atmark($sockets[1]));
socket_recv($sockets[1], $data, 1, 0);
var_dump($data);
var_dump(socket_atmark($sockets[1]));
socket_recv($sockets[1], $data, 1, 0);
var_dump($data);
var_dump(socket_atmark($sockets[1]));
socket_recv($sockets[1], $data, 1, 0);
var_dump($data);
var_dump(socket_atmark($sockets[1]));

socket_close($sockets[0]);
socket_close($sockets[1]);
?>
--EXPECT--
bool(false)
bool(false)
string(1) "a"
bool(true)
string(1) "b"
bool(false)
string(1) "c"
bool(false)
