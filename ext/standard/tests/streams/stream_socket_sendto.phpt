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
$serverUri = "tcp://127.0.0.1:31854";
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
Notice: fwrite(): send of %d bytes failed with errno=%d Broken pipe in %s on line %d
Warning: stream_socket_sendto(): Broken pipe

int(%i)

Warning: stream_socket_sendto(): Broken pipe

int(%i)

Warning: stream_socket_sendto(): php_network_getaddresses: getaddrinfo failed: Name or service not known in %s on line %d

Warning: stream_socket_sendto(): Failed to resolve `tcp': php_network_getaddresses: getaddrinfo failed: Name or service not known in %s on line %d

Warning: stream_socket_sendto(): Failed to parse `%s' into a valid network address in %s on line %d

bool(false)
