--TEST--
Testing linger `socket` option.
--FILE--
<?php
for ($i=0; $i<100; $i++) {
  $port = rand(10000, 65000);
  /* Setup socket server */
  $server = @stream_socket_server("tcp://127.0.0.1:$port");
  if ($server) {
    break;
  }
}
$client = stream_socket_client("tcp://127.0.0.1:$port");
$context = stream_context_create(['socket' => ['linger' => false]]);
$socket = stream_socket_client("tcp://127.0.0.1:$port", $errno, $errstr, 0, STREAM_CLIENT_CONNECT, $context);
var_dump($socket);
$context = stream_context_create(['socket' => ['linger' => PHP_INT_MAX + 1]]);
$socket = stream_socket_client("tcp://127.0.0.1:$port", $errno, $errstr, 0, STREAM_CLIENT_CONNECT, $context);
var_dump($socket);
$context = stream_context_create(['socket' => ['linger' => 5]]);
$socket = stream_socket_client("tcp://127.0.0.1:$port", $errno, $errstr, 1, STREAM_CLIENT_CONNECT, $context);
var_dump($socket);
stream_set_blocking($socket, true);
var_dump(stream_socket_sendto($socket, "data"));
$data = base64_decode("1oIBAAABAAAAAAAAB2V4YW1wbGUDb3JnAAABAAE=");
stream_set_blocking($socket, 0);
stream_socket_sendto($socket, $data);
stream_socket_shutdown($socket, STREAM_SHUT_RDWR);
stream_socket_shutdown($server, STREAM_SHUT_RDWR);
?>
--EXPECTF--
Warning: stream_socket_client(): Unable to connect to tcp://127.0.0.1:%d (Invalid `linger` value) in %s on line %d
bool(false)

Warning: stream_socket_client(): Unable to connect to tcp://127.0.0.1:%d (Invalid `linger` value) in %s on line %d
bool(false)
resource(%d) of type (stream)
int(4)
