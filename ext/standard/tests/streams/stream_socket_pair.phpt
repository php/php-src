--TEST--
stream_socket_pair()
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') die("skip: non windows test");
?>
--FILE--
<?php
$sockets = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, 0);
var_dump($sockets);
fwrite($sockets[0], b"foo");
var_dump(fread($sockets[1], strlen(b"foo")));
fclose($sockets[0]);
?>
--EXPECTF--
array(2) {
  [0]=>
  resource(%d) of type (stream)
  [1]=>
  resource(%d) of type (stream)
}
string(3) "foo"
