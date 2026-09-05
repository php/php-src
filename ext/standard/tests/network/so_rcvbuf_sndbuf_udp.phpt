--TEST--
stream_socket_server() SO_RCVBUF and SO_SNDBUF context options test with UDP
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

$control = stream_socket_server("udp://127.0.0.1:0", $errno, $errstr, STREAM_SERVER_BIND);

if (!$control) {
    die('Unable to create server');
}

[$rcvbuf, $sndbuf] = buffers($control);

// Shrinking is always honoured, while growing may be capped by the system maximum.
$context = stream_context_create(['socket' => [
    'so_rcvbuf' => intdiv($rcvbuf, 4),
    'so_sndbuf' => intdiv($sndbuf, 4),
]]);

$server = stream_socket_server("udp://127.0.0.1:0", $errno, $errstr, STREAM_SERVER_BIND, $context);

if (!$server) {
    die('Unable to create server');
}

[$server_rcvbuf, $server_sndbuf] = buffers($server);
echo "Server buffers\n";
var_dump($server_rcvbuf < $rcvbuf);
var_dump($server_sndbuf < $sndbuf);

fclose($server);
fclose($control);

?>
--EXPECT--
Server buffers
bool(true)
bool(true)
