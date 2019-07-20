--TEST--
EAGAIN/EWOULDBLOCK on a non-blocking socket should not result in an error return value
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') die('skip not for Windows');
?>
--FILE--
<?php

$sockets = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, STREAM_IPPROTO_IP);
stream_set_blocking($sockets[0], false);
var_dump(fread($sockets[0], 100));

?>
--EXPECT--
string(0) ""
