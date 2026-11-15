--TEST--
stream_socket_sendto() returns false on failure
--FILE--
<?php

/* Unconnected datagram socket without a destination address */
$server = stream_socket_server('udp://127.0.0.1:0', $errno, $errstr, STREAM_SERVER_BIND);
var_dump(@stream_socket_sendto($server, 'x'));

/* Stream that is not a socket */
var_dump(stream_socket_sendto(fopen('php://memory', 'r+'), 'x'));

/* Address that cannot be parsed */
var_dump(@stream_socket_sendto($server, 'x', 0, 'not an address'));

/* A successful call still returns the number of bytes */
$client = stream_socket_client('udp://' . stream_socket_get_name($server, false));
var_dump(stream_socket_sendto($client, 'abc'));

?>
--EXPECT--
bool(false)
bool(false)
bool(false)
int(3)
