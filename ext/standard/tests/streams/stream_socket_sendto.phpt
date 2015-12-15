--TEST--
int stream_socket_sendto ( resource $socket , string $data [, int $flags = 0 [, string $address ]] );
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
$serverUri = "yahoo.com:80";
$sock = stream_socket_server($serverUri, $errno, $errstr);

if (is_resource($sock)) {
  fwrite($sock, "Normal data transmit.");
  var_dump(stream_socket_sendto($sock, "Out of Band data."));
  var_dump(stream_socket_sendto($sock, "Out of Band data.", STREAM_OOB));
  var_dump(stream_socket_sendto($sock, "Out of Band data.", STREAM_OOB, $serverUri));
} else {
  die("Test stream_socket_sendto has failed; Unable to connect: {$errstr} ({$errno})");
}
?>
--CLEAN--
fclose($sock);
--EXPECT--
