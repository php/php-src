--TEST--
socket_create_listen() and socket_listen(): $backlog beyond the int range is clamped, not truncated
--EXTENSIONS--
sockets
--SKIPIF--
<?php
if (PHP_INT_SIZE < 8) {
    die('skip 64-bit only');
}
if (PHP_OS_FAMILY !== 'Linux') {
    die('skip Linux only');
}
?>
--FILE--
<?php
function count_queued(int $backlog): int {
    $server = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
    socket_bind($server, '127.0.0.1', 0);
    socket_getsockname($server, $address, $port);
    var_dump(socket_listen($server, $backlog));

    $clients = [];
    for ($i = 0; $i < 20; $i++) {
        $client = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
        socket_set_nonblock($client);
        @socket_connect($client, $address, $port);
        $clients[] = $client;
    }
    usleep(200000);

    socket_set_nonblock($server);
    $queued = 0;
    while (($peer = @socket_accept($server)) !== false) {
        $queued++;
    }
    return $queued;
}

// 2 ** 32 used to wrap to 0, which leaves room for a single connection
foreach ([2 ** 32, 2 ** 32 + 5, PHP_INT_MAX, PHP_INT_MIN, -(2 ** 31) - 1] as $backlog) {
    var_dump(count_queued($backlog) >= 20);
}

$listener = socket_create_listen(0, 2 ** 32);
var_dump($listener instanceof Socket);

// The "backlog" context option of stream sockets is clamped the same way
$context = stream_context_create(['socket' => ['backlog' => 2 ** 32]]);
$server = stream_socket_server('tcp://127.0.0.1:0', $errno, $errstr, STREAM_SERVER_BIND | STREAM_SERVER_LISTEN, $context);
$name = stream_socket_get_name($server, false);
$clients = [];
for ($i = 0; $i < 20; $i++) {
    $clients[] = stream_socket_client("tcp://$name", $errno, $errstr, 1, STREAM_CLIENT_ASYNC_CONNECT);
}
usleep(200000);
$queued = 0;
while (@stream_socket_accept($server, 0) !== false) {
    $queued++;
}
var_dump($queued >= 20);
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
