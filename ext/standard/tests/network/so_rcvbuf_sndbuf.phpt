--TEST--
stream_socket_server() and stream_socket_client() SO_RCVBUF and SO_SNDBUF context options test
--EXTENSIONS--
sockets
--FILE--
<?php
function buffers($stream): array {
    $sock = socket_import_stream($stream);
    return [
        socket_get_option($sock, SOL_SOCKET, SO_RCVBUF),
        socket_get_option($sock, SOL_SOCKET, SO_SNDBUF),
    ];
}

// Shrinking is always honoured, while growing may be capped by the system maximum.
function context(int $rcvbuf, int $sndbuf) {
    return stream_context_create(['socket' => [
        'so_rcvbuf' => intdiv($rcvbuf, 4),
        'so_sndbuf' => intdiv($sndbuf, 4),
    ]]);
}

$control = stream_socket_server("tcp://127.0.0.1:0", $errno, $errstr,
    STREAM_SERVER_BIND | STREAM_SERVER_LISTEN);

if (!$control) {
    die('Unable to create server');
}

[$rcvbuf, $sndbuf] = buffers($control);

$server = stream_socket_server("tcp://127.0.0.1:0", $errno, $errstr,
    STREAM_SERVER_BIND | STREAM_SERVER_LISTEN, context($rcvbuf, $sndbuf));

if (!$server) {
    die('Unable to create server');
}

$addr = stream_socket_get_name($server, false);
$port = (int)substr(strrchr($addr, ':'), 1);

$client = stream_socket_client("tcp://127.0.0.1:$port", $errno, $errstr, 30,
    STREAM_CLIENT_CONNECT, context($rcvbuf, $sndbuf));

if (!$client) {
    die('Unable to create client');
}

$accepted = stream_socket_accept($server, 1);

if (!$accepted) {
    die('Unable to accept connection');
}

// Verify the listening socket
[$listen_rcvbuf, $listen_sndbuf] = buffers($server);
echo "Listen buffers\n";
var_dump($listen_rcvbuf < $rcvbuf);
var_dump($listen_sndbuf < $sndbuf);

// Verify server side (accepted connection, inherits from the listening socket)
[$server_rcvbuf, $server_sndbuf] = buffers($accepted);
echo "Server buffers\n";
var_dump($server_rcvbuf < $rcvbuf);
var_dump($server_sndbuf < $sndbuf);

// Verify client side
[$client_rcvbuf, $client_sndbuf] = buffers($client);
echo "Client buffers\n";
var_dump($client_rcvbuf < $rcvbuf);
var_dump($client_sndbuf < $sndbuf);

fclose($accepted);
fclose($client);
fclose($server);
fclose($control);

?>
--EXPECT--
Listen buffers
bool(true)
bool(true)
Server buffers
bool(true)
bool(true)
Client buffers
bool(true)
bool(true)
