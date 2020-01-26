--TEST--
Bug #79000: Non-blocking socket stream reports EAGAIN as error
--SKIPIF--
<?php
if (PHP_OS_FAMILY == 'Windows') die('skip Not for Windows');
?>
--FILE--
<?php

[$sock1, $sock2] = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, STREAM_IPPROTO_IP);
$str = str_repeat('a', 1000000);
stream_set_blocking($sock1, false);
var_dump(fwrite($sock1, $str));
var_dump(fwrite($sock1, $str));

?>
--EXPECTF--
int(%d)
int(%d)
