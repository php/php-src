--TEST--
stream_socket_server() and stream_socket_client() keepalive option test with string values
--EXTENSIONS--
sockets
--SKIPIF--
<?php
if (!defined('TCP_KEEPIDLE') && !defined('TCP_KEEPALIVE')) {
    die('skip TCP_KEEPIDLE/TCP_KEEPALIVE not available');
}
if (!defined('TCP_KEEPINTVL')) {
    die('skip TCP_KEEPINTVL not available');
}
if (!defined('TCP_KEEPCNT')) {
    die('skip TCP_KEEPCNT not available');
}
?>
--FILE--
<?php
// Test server with SO_KEEPALIVE enabled
$server_context = stream_context_create([
    'socket' => [
        'so_keepalive' => true,
        'tcp_keepidle' => '80',
        'tcp_keepintvl' => '12',
        'tcp_keepcnt' => '6',
    ]
]);

$server = stream_socket_server("tcp://127.0.0.1:0", $errno, $errstr, 
    STREAM_SERVER_BIND | STREAM_SERVER_LISTEN, $server_context);

if (!$server) {
    die('Unable to create server');
}

$addr = stream_socket_get_name($server, false);
$port = (int)substr(strrchr($addr, ':'), 1);

// Test client with SO_KEEPALIVE enabled
$client_context = stream_context_create([
    'socket' => [
        'so_keepalive' => true,
        'tcp_keepidle' => '31',
        'tcp_keepintvl' => '6',
        'tcp_keepcnt' => '4',
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
$server_keepalive = socket_get_option($server_sock, SOL_SOCKET, SO_KEEPALIVE);
echo "Server SO_KEEPALIVE: " . ($server_keepalive ? "enabled" : "disabled") . "\n";

if (defined('TCP_KEEPIDLE')) {
    $server_idle = socket_get_option($server_sock, SOL_TCP, TCP_KEEPIDLE);
    echo "Server TCP_KEEPIDLE: $server_idle\n";
} else {
    $server_idle = socket_get_option($server_sock, SOL_TCP, TCP_KEEPALIVE);
    echo "Server TCP_KEEPIDLE: $server_idle\n";
}

$server_intvl = socket_get_option($server_sock, SOL_TCP, TCP_KEEPINTVL);
echo "Server TCP_KEEPINTVL: $server_intvl\n";

$server_cnt = socket_get_option($server_sock, SOL_TCP, TCP_KEEPCNT);
echo "Server TCP_KEEPCNT: $server_cnt\n";

// Verify client side
$client_sock = socket_import_stream($client);
$client_keepalive = socket_get_option($client_sock, SOL_SOCKET, SO_KEEPALIVE);
echo "Client SO_KEEPALIVE: " . ($client_keepalive ? "enabled" : "disabled") . "\n";

if (defined('TCP_KEEPIDLE')) {
    $client_idle = socket_get_option($client_sock, SOL_TCP, TCP_KEEPIDLE);
    echo "Client TCP_KEEPIDLE: $client_idle\n";
} else {
    $client_idle = socket_get_option($client_sock, SOL_TCP, TCP_KEEPALIVE);
    echo "Client TCP_KEEPIDLE: $client_idle\n";
}

$client_intvl = socket_get_option($client_sock, SOL_TCP, TCP_KEEPINTVL);
echo "Client TCP_KEEPINTVL: $client_intvl\n";

$client_cnt = socket_get_option($client_sock, SOL_TCP, TCP_KEEPCNT);
echo "Client TCP_KEEPCNT: $client_cnt\n";

fclose($accepted);
fclose($client);
fclose($server);

// Test server with SO_KEEPALIVE disabled
$server2_context = stream_context_create(['socket' => ['so_keepalive' => false]]);
$server2 = stream_socket_server("tcp://127.0.0.1:0", $errno, $errstr, 
    STREAM_SERVER_BIND | STREAM_SERVER_LISTEN, $server2_context);

$addr2 = stream_socket_get_name($server2, false);
$port2 = (int)substr(strrchr($addr2, ':'), 1);

$client2 = stream_socket_client("tcp://127.0.0.1:$port2");
$accepted2 = stream_socket_accept($server2, 1);

$server2_sock = socket_import_stream($accepted2);
$server2_keepalive = socket_get_option($server2_sock, SOL_SOCKET, SO_KEEPALIVE);
echo "Server disabled SO_KEEPALIVE: " . ($server2_keepalive ? "enabled" : "disabled") . "\n";

fclose($accepted2);
fclose($client2);
fclose($server2);

// Test client with SO_KEEPALIVE disabled
$server3 = stream_socket_server("tcp://127.0.0.1:0", $errno, $errstr, 
    STREAM_SERVER_BIND | STREAM_SERVER_LISTEN);

$addr3 = stream_socket_get_name($server3, false);
$port3 = (int)substr(strrchr($addr3, ':'), 1);

$client3_context = stream_context_create(['socket' => ['so_keepalive' => false]]);
$client3 = stream_socket_client("tcp://127.0.0.1:$port3", $errno, $errstr, 30, 
    STREAM_CLIENT_CONNECT, $client3_context);

$client3_sock = socket_import_stream($client3);
$client3_keepalive = socket_get_option($client3_sock, SOL_SOCKET, SO_KEEPALIVE);
echo "Client disabled SO_KEEPALIVE: " . ($client3_keepalive ? "enabled" : "disabled") . "\n";

fclose($client3);
fclose($server3);
?>
--EXPECT--
Server SO_KEEPALIVE: enabled
Server TCP_KEEPIDLE: 80
Server TCP_KEEPINTVL: 12
Server TCP_KEEPCNT: 6
Client SO_KEEPALIVE: enabled
Client TCP_KEEPIDLE: 31
Client TCP_KEEPINTVL: 6
Client TCP_KEEPCNT: 4
Server disabled SO_KEEPALIVE: disabled
Client disabled SO_KEEPALIVE: disabled
