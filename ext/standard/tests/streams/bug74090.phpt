--TEST--
Bug #74090 stream_get_contents maxlength>-1 returns empty string on windows
--SKIPIF--
<?php
if (getenv("SKIP_ONLINE_TESTS")) { die('skip: online test'); }
if (getenv("SKIP_SLOW_TESTS")) { die('skip: slow test'); }
?>
--FILE--
<?php
$port = 12327;
$server = false;
while(!$server && $port < 20000){
    $port++;
    $server = stream_socket_server("udp://localhost:$port", $errno, $errstr, STREAM_SERVER_BIND);
}
if(!$server){
    var_dump(false);
    die("Cannot create server socket");
}
$data = base64_decode("1oIBAAABAAAAAAAAB2V4YW1wbGUDb3JnAAABAAE=");
$fd = stream_socket_client("udp://localhost:$port", $errno, $errstr, 0, STREAM_CLIENT_CONNECT | STREAM_CLIENT_ASYNC_CONNECT);
stream_set_blocking($fd, 0);
stream_socket_sendto($fd, $data);
stream_socket_recvfrom($server, 1, 0, $peer);
stream_socket_sendto($server, $data, 0, $peer);
$ret = stream_get_contents($fd, 65565);
var_dump(strlen($ret) > 0);
stream_socket_shutdown($fd, STREAM_SHUT_RDWR);
stream_socket_shutdown($server, STREAM_SHUT_RDWR);
?>
==DONE==
--EXPECT--
bool(true)
==DONE==
