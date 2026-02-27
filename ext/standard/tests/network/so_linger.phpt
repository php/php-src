--TEST--
stream_socket_server() and stream_socket_client() SO_LINGER context option test
--EXTENSIONS--
sockets
--SKIPIF--
<?php
if (!defined('SO_LINGER')) {
    die('skip SO_LINGER not available');
}
?>
--FILE--
<?php
// Test server with SO_LINGER enabled
$server_context = stream_context_create([
    'socket' => [
        'so_linger' => 10,
    ]
]);

$server = stream_socket_server("tcp://127.0.0.1:0", $errno, $errstr, 
    STREAM_SERVER_BIND | STREAM_SERVER_LISTEN, $server_context);

if (!$server) {
    die('Unable to create server');
}

$addr = stream_socket_get_name($server, false);
$port = (int)substr(strrchr($addr, ':'), 1);

// Test client with SO_LINGER enabled
$client_context = stream_context_create([
    'socket' => [
        'so_linger' => 8,
    ]
]);

$client = stream_socket_client("tcp://127.0.0.1:$port", $errno, $errstr, 30, 
    STREAM_CLIENT_CONNECT, $client_context);

if (!$client) {
    die('Unable to create client');
}

$accepted = stream_socket_accept($server, 1);

if (!$accepted) {
    die('Unable to accept connection');
}

// Verify server side (accepted connection)
$server_sock = socket_import_stream($accepted);
$server_linger = socket_get_option($server_sock, SOL_SOCKET, SO_LINGER);
echo "Server SO_LINGER\n";
var_dump($server_linger);

// Verify client side
$client_sock = socket_import_stream($client);
$client_linger = socket_get_option($client_sock, SOL_SOCKET, SO_LINGER);
echo "Client SO_LINGER\n";
var_dump($client_linger);

fclose($accepted);
fclose($client);
fclose($server);

?>
--EXPECT--
Server SO_LINGER
array(2) {
  ["l_onoff"]=>
  int(1)
  ["l_linger"]=>
  int(10)
}
Client SO_LINGER
array(2) {
  ["l_onoff"]=>
  int(1)
  ["l_linger"]=>
  int(8)
}
