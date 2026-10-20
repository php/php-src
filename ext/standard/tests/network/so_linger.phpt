--TEST--
stream_socket_server() and stream_socket_client() SO_LINGER context option test
--EXTENSIONS--
sockets
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

// macOS expresses SO_LINGER in ticks, SO_LINGER_SEC in seconds.
$so_linger = defined('SO_LINGER_SEC') ? SO_LINGER_SEC : SO_LINGER;

// Verify the listening socket
$listen_sock = socket_import_stream($server);
$listen_linger = socket_get_option($listen_sock, SOL_SOCKET, $so_linger);
echo "Listen SO_LINGER\n";
var_dump($listen_linger['l_onoff'] > 0);
var_dump($listen_linger['l_linger']);

// Verify server side (accepted connection, inherits from the listening socket)
$server_sock = socket_import_stream($accepted);
$server_linger = socket_get_option($server_sock, SOL_SOCKET, $so_linger);
echo "Server SO_LINGER\n";
var_dump($server_linger['l_onoff'] > 0);
var_dump($server_linger['l_linger']);

// Verify client side
$client_sock = socket_import_stream($client);
$client_linger = socket_get_option($client_sock, SOL_SOCKET, $so_linger);
echo "Client SO_LINGER\n";
var_dump($client_linger['l_onoff'] > 0);
var_dump($client_linger['l_linger']);

fclose($accepted);
fclose($client);
fclose($server);

?>
--EXPECT--
Listen SO_LINGER
bool(true)
int(10)
Server SO_LINGER
bool(true)
int(10)
Client SO_LINGER
bool(true)
int(8)
