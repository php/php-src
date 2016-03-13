--TEST--
int stream_socket_sendto ( resource $socket , string $data [, int $flags = 0 [, string $address ]] );
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br> - @phpsp - sao paulo - br
--SKIPIF--
<?php
if (phpversion() < "5.3.0") {
    die('SKIP php version so lower.');
}
?>
--FILE--
<?php
$serverUri = "tcp://127.0.0.1:31854";
$serverUrl = "http://yahoo.com";
$string_socket = "Out of Band data.";
$string_fwrite = "Normal data transmit.";
$sock = stream_socket_server($serverUri, $errno, $errstr);

if (is_resource($sock)) {
    fwrite($sock, $string_fwrite);
    var_dump(stream_socket_sendto());
    var_dump(stream_socket_sendto($sock));
    var_dump(stream_socket_sendto($sock, $string_socket));
    var_dump(stream_socket_sendto($sock, $string_socket, STREAM_OOB));
    var_dump(stream_socket_sendto($sock, $string_socket, STREAM_OOB, $serverUri));
} else {
    die("Test stream_socket_enable_crypto has failed; Unable to connect: {$errstr} ({$errno})");
}
?>
--EXPECTF--
Warning: stream_socket_sendto() expects at least %d parameters, %i given in %s on line %d
bool(false)

Warning: stream_socket_sendto(): Broken pipe
 in %s on line %d
int(%i)
