--TEST--
stream_get_contents() - Testing on socket with $maxlength
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') die("skip: non windows test");
?>
--FILE--
<?php
$sockets = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, 0);

stream_set_timeout($sockets[1], 6000);

fwrite($sockets[0], "foo");
var_dump(stream_get_contents($sockets[1], 3));

?>
--EXPECT--
string(3) "foo"
