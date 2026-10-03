--TEST--
socket_export_stream(): the stream's read timeout applies to a blocking socket
--EXTENSIONS--
sockets
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') die('skip no AF_UNIX socket pair on Windows');
?>
--FILE--
<?php
socket_create_pair(AF_UNIX, SOCK_STREAM, 0, $pair);
$s = socket_export_stream($pair[0]);
stream_set_timeout($s, 0, 100000);
var_dump(fread($s, 10));
var_dump(stream_get_meta_data($s)['timed_out']);
socket_write($pair[1], "data");
var_dump(fread($s, 10));
?>
--EXPECT--
bool(false)
bool(true)
string(4) "data"
