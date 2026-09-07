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

function port($server): int {
    return (int)substr(strrchr(stream_socket_get_name($server, false), ':'), 1);
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

echo "Listen buffers\n";
[$listen_rcvbuf, $listen_sndbuf] = buffers($server);
var_dump($listen_rcvbuf < $rcvbuf);
var_dump($listen_sndbuf < $sndbuf);

// A connection is compared against another connection: some systems size the
// receive buffer of a connected socket on their own.
$control_client = stream_socket_client("tcp://127.0.0.1:" . port($server), $errno, $errstr, 30);

if (!$control_client) {
    die('Unable to create client');
}

$control_accepted = stream_socket_accept($server, 1);

if (!$control_accepted) {
    die('Unable to accept connection');
}

[, $client_sndbuf] = buffers($control_client);

$client = stream_socket_client("tcp://127.0.0.1:" . port($server), $errno, $errstr, 30,
    STREAM_CLIENT_CONNECT, context($client_sndbuf, $client_sndbuf));

if (!$client) {
    die('Unable to create client');
}

$accepted = stream_socket_accept($server, 1);

if (!$accepted) {
    die('Unable to accept connection');
}

echo "Client buffers\n";
[, $client_sndbuf2] = buffers($client);
var_dump($client_sndbuf2 < $client_sndbuf);

fclose($accepted);
fclose($control_accepted);
fclose($client);
fclose($control_client);
fclose($server);
fclose($control);

?>
--EXPECT--
Listen buffers
bool(true)
bool(true)
Client buffers
bool(true)
