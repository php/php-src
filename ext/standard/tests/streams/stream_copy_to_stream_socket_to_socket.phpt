--TEST--
stream_copy_to_stream() socket to socket (splice both directions)
--SKIPIF--
<?php
$sockets = @stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, 0);
if (!$sockets) die("skip stream_socket_pair not available");
?>
--FILE--
<?php

$sockets1 = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, 0);
$sockets2 = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, 0);

$data = str_repeat('test data ', 1000);
fwrite($sockets1[0], $data);
stream_socket_shutdown($sockets1[0], STREAM_SHUT_WR);

$copied = stream_copy_to_stream($sockets1[1], $sockets2[0]);
var_dump($copied);

stream_socket_shutdown($sockets2[0], STREAM_SHUT_WR);

$result = stream_get_contents($sockets2[1]);
var_dump(strlen($result));
var_dump($result === $data);

fclose($sockets1[0]);
fclose($sockets1[1]);
fclose($sockets2[0]);
fclose($sockets2[1]);

?>
--EXPECT--
int(10000)
int(10000)
bool(true)