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
} else {
  die("Test stream_socket_sendto has failed; Unable to connect: {$errstr} ({$errno})");
}
?>
--CLEAN--
unset($serverUri);
fclose($sock);
unset($sock);
--EXPECT--
Notice: fwrite(): send of %d bytes failed with errno=%d Broken pipe in %s on line %d

in %s on line %d
int(%i)
