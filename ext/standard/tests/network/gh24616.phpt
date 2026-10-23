--TEST--
GH-24616 (fsockopen()/stream_socket_client() accept TCP ports above 65535)
--FILE--
<?php
$srv = stream_socket_server("tcp://127.0.0.1:0");
$name = stream_socket_get_name($srv, false);
$port = (int) substr($name, strrpos($name, ':') + 1);
$bad = $port + 65536;

var_dump(@stream_socket_client("tcp://127.0.0.1:$bad", $errno, $errstr));
var_dump($errstr === "Failed to parse port of address \"127.0.0.1:$bad\"");
var_dump(@fsockopen("127.0.0.1", $bad));
var_dump(@stream_socket_client("tcp://[::1]:$bad"));
var_dump(@stream_socket_client("tcp://127.0.0.1:-1"));
var_dump(@stream_socket_client("tcp://127.0.0.1:99999999999999999999"));
var_dump(@stream_socket_server("tcp://127.0.0.1:65536"));

$udp = stream_socket_server("udp://127.0.0.1:0", $errno, $errstr, STREAM_SERVER_BIND);
$name = stream_socket_get_name($udp, false);
$bad = (int) substr($name, strrpos($name, ':') + 1) + 65536;
$client = stream_socket_client("udp://127.0.0.1:1");
var_dump(@stream_socket_sendto($client, "x", 0, "127.0.0.1:$bad"));

/* Valid ports still work */
$c = stream_socket_client("tcp://127.0.0.1:$port");
var_dump(stream_socket_get_name($c, true) === "127.0.0.1:$port");
?>
--EXPECT--
bool(false)
bool(true)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(true)
