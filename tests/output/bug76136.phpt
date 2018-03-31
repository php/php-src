--TEST--
Bug #76136: stream_socket_get_name should enclose IPv6 in brackets
--FILE--
<?php
$server = stream_socket_server("tcp://[::1]:1337/");
echo stream_socket_get_name($server, false).PHP_EOL;
$server = stream_socket_server("tcp://127.0.0.1:1337/");
echo stream_socket_get_name($server, false);
--EXPECTF--
[::1]:1337
127.0.0.1:1337