--TEST--
stream_copy_to_stream() 16k with file as $source and socket as $dest
--SKIPIF--
<?php
$sockets = @stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, 0);
if (!$sockets) die("skip stream_socket_pair");
?>
--FILE--
<?php

$sockets = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, 0);
$src = tmpfile();

$data = str_repeat('data', 4096);
fwrite($src, $data);
rewind($src);

$copied = stream_copy_to_stream($src, $sockets[0]);
var_dump($copied);

stream_socket_shutdown($sockets[0], STREAM_SHUT_WR);

$result = stream_get_contents($sockets[1]);
var_dump(strlen($result));
var_dump($result === $data);

fclose($src);
fclose($sockets[0]);
fclose($sockets[1]);

?>
--EXPECT--
int(16384)
int(16384)
bool(true)
