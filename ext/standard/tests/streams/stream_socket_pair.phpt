--TEST--
stream_socket_pair()
--FILE--
<?php
$domain = PHP_OS_FAMILY === 'Windows' ? STREAM_PF_INET : STREAM_PF_UNIX;
$sockets = stream_socket_pair($domain, STREAM_SOCK_STREAM, 0);
var_dump($sockets);
fwrite($sockets[0], "foo");
var_dump(fread($sockets[1], strlen("foo")));
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
