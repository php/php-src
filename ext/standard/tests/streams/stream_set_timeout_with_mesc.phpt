--TEST--
Test stream_set_timeout() function : set timeout with microseconds
--FILE--
<?php
$sockets = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, 0);

stream_set_timeout($sockets[1], 1, 800);

fwrite($sockets[0], "foo");
var_dump(stream_get_contents($sockets[1], 3));
?>
--EXPECT--
string(3) "foo"
