--TEST--
stream_copy_to_stream() with socket as $source
--SKIPIF--
<?php
$sockets = @stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, 0);
if (!$sockets) die("skip stream_socket_pair");
?>
--FILE--
<?php

$sockets = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, 0);
$tmp = tmpfile();

fwrite($sockets[0], "a");
stream_socket_shutdown($sockets[0], STREAM_SHUT_WR);
stream_copy_to_stream($sockets[1], $tmp);

fseek($tmp, 0, SEEK_SET);
var_dump(stream_get_contents($tmp));

stream_copy_to_stream($sockets[1], $tmp);

fseek($tmp, 0, SEEK_SET);
var_dump(stream_get_contents($tmp));


?>
--EXPECT--
string(1) "a"
string(1) "a"
