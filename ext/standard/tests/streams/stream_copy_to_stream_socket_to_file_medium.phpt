--TEST--
stream_copy_to_stream() 2048 bytes with socket as $source and file as $dest
--SKIPIF--
<?php
$sockets = @stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, 0);
if (!$sockets) die("skip stream_socket_pair");
?>
--FILE--
<?php

$sockets = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, 0);
$tmp = tmpfile();

fwrite($sockets[0], str_repeat("a", 2048));
stream_socket_shutdown($sockets[0], STREAM_SHUT_WR);
stream_copy_to_stream($sockets[1], $tmp);

fseek($tmp, 0, SEEK_SET);
var_dump(stream_get_contents($tmp));


?>
--EXPECTF--
string(2048) "aaaaa%saaa"

