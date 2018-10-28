--TEST--
Bug #74090 stream_get_contents maxlength>-1 returns empty string on windows
--SKIPIF--
<?php
if (getenv("SKIP_ONLINE_TESTS")) { die('skip: online test'); }
if (getenv("SKIP_SLOW_TESTS")) { die('skip: slow test'); }
?>
--FILE--
<?php
$data = base64_decode("1oIBAAABAAAAAAAAB2V4YW1wbGUDb3JnAAABAAE=");
$fd = stream_socket_client("udp://8.8.8.8:53", $errno, $errstr, 0, STREAM_CLIENT_CONNECT | STREAM_CLIENT_ASYNC_CONNECT);
stream_set_blocking($fd, 0);
stream_socket_sendto($fd,$data);
sleep(1);
$ret = stream_get_contents($fd,65565);
var_dump(strlen($ret) > 0);
stream_socket_shutdown($fd,STREAM_SHUT_RDWR);
?>
==DONE==
--EXPECT--
bool(true)
==DONE==
