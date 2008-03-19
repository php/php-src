--TEST--
Testing fread() on a TCP server socket
--FILE--
<?php

$tcp_socket = stream_socket_server('tcp://127.0.0.1:31337');

socket_set_timeout($tcp_socket, 1);

var_dump(fread($tcp_socket, 1));

fclose($tcp_socket);

?>
--EXPECT--
string(0) ""
