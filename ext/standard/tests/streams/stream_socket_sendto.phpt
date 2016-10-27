--TEST--
int stream_socket_sendto ( resource $socket , string $data [, int $flags = 0 [, string $address ]] );
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br> - @phpsp - sao paulo - br
--SKIPIF--
<?php
if (getenv("SKIP_ONLINE_TESTS")) { die('skip: online test'); }
?>
--FILE--
<?php
$serverUri = "tcp://127.0.0.1:31854";
$serverUrl = "http://php.net";
$stringSocket = "out of band data.";
$stringFWrite = "normal data to transmit";
$sock = stream_socket_server($serverUri, $errno, $errstr);

if (is_resource($sock)) {
    fwrite($sock, $stringFWrite);
    var_dump(stream_socket_sendto());
    var_dump(stream_socket_sendto($sock));
    var_dump(stream_socket_sendto($sock, $stringSocket));
    var_dump(stream_socket_sendto($sock, $stringSocket, STREAM_OOB));
    var_dump(stream_socket_sendto($sock, $stringSocket, STREAM_OOB, $serverUri));
    var_dump(stream_socket_sendto($sock, $stringSocket, STREAM_OOB, $serverUrl));
} else {
    die("Test stream_socket_enable_crypto has failed; Unable to connect: {$errstr} ({$errno})");
}
?>
--EXPECTF--
Notice: fwrite(): send of %i bytes failed with errno=%i Broken pipe in %s on line %i

Warning: stream_socket_sendto() expects at least %i parameters, %i given in %s on line %i
bool(%s)

Warning: stream_socket_sendto() expects at least %i parameters, %i given in %s on line %i
bool(%s)

Warning: stream_socket_sendto(): Broken pipe
 in %s on line %i
int(%i)

Warning: stream_socket_sendto(): Broken pipe
 in %s on line %i
int(%i)

Warning: stream_socket_sendto(): php_network_getaddresses: getaddrinfo failed: Name or service not known in %s on line %i

Warning: stream_socket_sendto(): Failed to resolve %s: php_network_getaddresses: getaddrinfo failed: Name or service not known in %s on line %i

Warning: stream_socket_sendto(): Failed to parse %s into a valid network address in %s on line %i
bool(%s)

Warning: stream_socket_sendto(): php_network_getaddresses: getaddrinfo failed: Name or service not known in %s on line %i

Warning: stream_socket_sendto(): Failed to resolve %s: php_network_getaddresses: getaddrinfo failed: Name or service not known in %s on line %i

Warning: stream_socket_sendto(): Failed to parse %s into a valid network address in %s on line %i
bool(%s)
