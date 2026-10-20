--TEST--
stream_socket_server() and stream_socket_client() SO_RCVBUF and SO_SNDBUF context options test with unix sockets
--EXTENSIONS--
sockets
--SKIPIF--
<?php
if (array_search("unix", stream_get_transports()) === false) {
    die('skip No support for UNIX domain sockets.');
}
?>
--FILE--
<?php
function buffers($stream): array {
    $sock = socket_import_stream($stream);
    return [
        socket_get_option($sock, SOL_SOCKET, SO_RCVBUF),
        socket_get_option($sock, SOL_SOCKET, SO_SNDBUF),
    ];
}

$control_path = sys_get_temp_dir() . '/' . uniqid('so_buf_control_') . '.sock';
$server_path = sys_get_temp_dir() . '/' . uniqid('so_buf_server_') . '.sock';

$control = stream_socket_server("unix://$control_path", $errno, $errstr,
    STREAM_SERVER_BIND | STREAM_SERVER_LISTEN);

if (!$control) {
    die('Unable to create server');
}

$control_client = stream_socket_client("unix://$control_path", $errno, $errstr, 30);

if (!$control_client) {
    die('Unable to create client');
}

[$rcvbuf, $sndbuf] = buffers($control);
[$client_rcvbuf, $client_sndbuf] = buffers($control_client);

// Shrinking is always honoured, while growing may be capped by the system maximum.
function context(int $rcvbuf, int $sndbuf) {
    return stream_context_create(['socket' => [
        'so_rcvbuf' => intdiv($rcvbuf, 4),
        'so_sndbuf' => intdiv($sndbuf, 4),
    ]]);
}

$server = stream_socket_server("unix://$server_path", $errno, $errstr,
    STREAM_SERVER_BIND | STREAM_SERVER_LISTEN, context($rcvbuf, $sndbuf));

if (!$server) {
    die('Unable to create server');
}

echo "Listen buffers", PHP_EOL;
[$listen_rcvbuf, $listen_sndbuf] = buffers($server);
var_dump($listen_rcvbuf < $rcvbuf);
var_dump($listen_sndbuf < $sndbuf);

$client = stream_socket_client("unix://$server_path", $errno, $errstr, 30,
    STREAM_CLIENT_CONNECT, context($client_rcvbuf, $client_sndbuf));

if (!$client) {
    die('Unable to create client');
}

$accepted = stream_socket_accept($server, 1);

if (!$accepted) {
    die('Unable to accept connection');
}

echo "Client buffers", PHP_EOL;
[$rcvbuf2, $sndbuf2] = buffers($client);
var_dump($rcvbuf2 < $client_rcvbuf);
var_dump($sndbuf2 < $client_sndbuf);

$invalid = stream_context_create(['socket' => ['so_rcvbuf' => 0]]);

echo "Invalid buffer size", PHP_EOL;
var_dump(@stream_socket_server("unix://$server_path.invalid", $errno, $errstr,
    STREAM_SERVER_BIND | STREAM_SERVER_LISTEN, $invalid));
echo $errstr, PHP_EOL;
var_dump(@stream_socket_client("unix://$server_path", $errno, $errstr, 30,
    STREAM_CLIENT_CONNECT, $invalid));
echo $errstr, PHP_EOL;

fclose($accepted);
fclose($client);
fclose($server);
fclose($control_client);
fclose($control);
unlink($server_path);
unlink($control_path);

?>
--EXPECT--
Listen buffers
bool(true)
bool(true)
Client buffers
bool(true)
bool(true)
Invalid buffer size
bool(false)
so_rcvbuf context option must be between 1 and 2147483647
bool(false)
so_rcvbuf context option must be between 1 and 2147483647
