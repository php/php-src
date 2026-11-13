--TEST--
GH-24173 (Sockets from synchronous stream_socket_client() are left in non-blocking mode on Windows)
--EXTENSIONS--
sockets
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== 'Windows') die('skip Windows only');
?>
--FILE--
<?php
$srv = stream_socket_server('tcp://127.0.0.1:0', $errno, $errstr);
$client = stream_socket_client('tcp://' . stream_socket_get_name($srv, false), $errno, $errstr, 5);
$conn = stream_socket_accept($srv);

$sock = socket_import_stream($client);
socket_set_option($sock, SOL_SOCKET, SO_RCVTIMEO, ['sec' => 0, 'usec' => 100000]);

// No data is sent: a blocking socket waits and times out, while a
// non-blocking one fails at once with WSAEWOULDBLOCK.
var_dump(@socket_read($sock, 10));
var_dump(socket_last_error($sock) === SOCKET_ETIMEDOUT);
?>
--EXPECT--
bool(false)
bool(true)
