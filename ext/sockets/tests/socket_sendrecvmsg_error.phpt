--TEST--
Error during socket_sendmsg() or socket_recvmsg()
--EXTENSIONS--
sockets
--FILE--
<?php

$socket = socket_create(AF_INET, SOCK_DGRAM, SOL_UDP);
socket_sendmsg($socket, [], -1);
$message = ['controllen' => 1];
socket_recvmsg($socket, $message, -1);

?>
--EXPECTF--
Warning: socket_sendmsg(): Error in sendmsg [%d]: %a in %s on line %d

Warning: socket_recvmsg(): Error in recvmsg [%d]: %a in %s on line %d
