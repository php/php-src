--TEST--
string stream_socket_recvfrom ( resource $socket , int $length [, int $flags = 0 [, string &$address ]] );
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br> - #phparty7 - @phpsp - novatec/2015 - sao paulo - br
--FILE--
<?php
$serverUri = "tcp://127.0.0.1:31856";
$sock = stream_socket_server($serverUri, $errno, $errstr);
$sockLen = 1500;

var_dump(stream_socket_recvfrom($sock, $sockLen));
var_dump(stream_socket_recvfrom($sock, $sockLen, STREAM_OOB));
var_dump(stream_socket_recvfrom($sock, $sockLen, STREAM_PEEK));
?>
--EXPECT--
bool(false)
bool(false)
bool(false)
