--TEST--
Bug#76136 stream_socket_get_name should enclose IPv6 in brackets
--FILE--
<?php
$server = stream_socket_server("tcp://[::1]:1337/");
var_dump(stream_socket_get_name($server, false));

--EXPECTF--
string(10) "[::1]:1337"
