--TEST--
string stream_socket_recvfrom ( resource $socket , int $length [, int $flags = 0 [, string &$address ]] );
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br> - #phparty7 - @phpsp - novatec/2015 - sao paulo - br
--SKIPIF--
<?php
if (phpversion() < "5.3.0") {
  die('SKIP php version so lower.');
}
?>
--FILE--
<?php
$serverUri = "tcp://127.0.0.1:31854";
$sock = stream_socket_server($serverUri, $errno, $errstr);
$sockLen = 1500;

var_dump(stream_socket_recvfrom($sock, $sockLen));
var_dump(stream_socket_recvfrom($sock, $sockLen, STREAM_OOB));
var_dump(stream_socket_recvfrom($sock, $sockLen, STREAM_PEEK));
?>
--CLEAN--
<?php
fclose($sock);
unset($serverUri);
unset($clientFlags);
unset($sock);
unset($sockLen);
unset($errno);
unset($errstr);
?>
--EXPECT--
bool(false)
bool(false)
bool(false)
