--TEST--
string stream_socket_get_name ( resource $handle , bool $want_peer ) ;
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br> - #phparty7 - @phpsp - novatec/2015 - sao paulo - br
--FILE--
<?php
$serverUri = "tcp://127.0.0.1:31854";
$sock = stream_socket_server($serverUri, $errno, $errstr);

var_dump(stream_socket_get_name($sock, false));
var_dump(stream_socket_get_name($sock, true));
?>
--CLEAN--
<?php
unset($serverUri);
unset($sock);
?>
--EXPECT--
string(15) "127.0.0.1:31854"
bool(false)
