--TEST--
stream_socket_server() and stream_socket_client() SO_LINGER context option test with unix sockets
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
// macOS expresses SO_LINGER in ticks, SO_LINGER_SEC in seconds.
$so_linger = defined('SO_LINGER_SEC') ? SO_LINGER_SEC : SO_LINGER;

function linger($stream): array {
    global $so_linger;

    return socket_get_option(socket_import_stream($stream), SOL_SOCKET, $so_linger);
}

$server_path = sys_get_temp_dir() . '/' . uniqid('so_linger_server_') . '.sock';
$dgram_path = sys_get_temp_dir() . '/' . uniqid('so_linger_dgram_') . '.sock';

$server = stream_socket_server("unix://$server_path", $errno, $errstr,
    STREAM_SERVER_BIND | STREAM_SERVER_LISTEN,
    stream_context_create(['socket' => ['so_linger' => 10]]));

if (!$server) {
    die('Unable to create server');
}

$client = stream_socket_client("unix://$server_path", $errno, $errstr, 30,
    STREAM_CLIENT_CONNECT,
    stream_context_create(['socket' => ['so_linger' => 8]]));

if (!$client) {
    die('Unable to create client');
}

$accepted = stream_socket_accept($server, 1);

if (!$accepted) {
    die('Unable to accept connection');
}

$listen_linger = linger($server);
echo "Listen SO_LINGER", PHP_EOL;
var_dump($listen_linger['l_onoff'] > 0);
var_dump($listen_linger['l_linger']);

$client_linger = linger($client);
echo "Client SO_LINGER", PHP_EOL;
var_dump($client_linger['l_onoff'] > 0);
var_dump($client_linger['l_linger']);

// The option is meaningless on datagram sockets and is not applied there.
$dgram = stream_socket_server("udg://$dgram_path", $errno, $errstr, STREAM_SERVER_BIND,
    stream_context_create(['socket' => ['so_linger' => 10]]));

if (!$dgram) {
    die('Unable to create server');
}

$dgram_linger = linger($dgram);
echo "Datagram SO_LINGER", PHP_EOL;
var_dump($dgram_linger['l_onoff'] > 0);

fclose($accepted);
fclose($client);
fclose($server);
fclose($dgram);
unlink($server_path);
unlink($dgram_path);

?>
--EXPECT--
Listen SO_LINGER
bool(true)
int(10)
Client SO_LINGER
bool(true)
int(8)
Datagram SO_LINGER
bool(false)
