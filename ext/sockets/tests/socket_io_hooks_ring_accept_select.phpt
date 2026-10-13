--TEST--
Io\Ring\Engine: socket_select() on a listener sees the connections its multishot accept took
--EXTENSIONS--
sockets
--SKIPIF--
<?php
if (!class_exists(Io\Ring\Engine::class)) die("skip Io\\Ring\\Engine not available");
$caps = (new Io\Ring\Engine())->getSupportedHookCapabilities();
if (!in_array(Io\Hooks\Capability::DirectAccept, $caps, true)) {
    die("skip the backend does not support DirectAccept");
}
if (!in_array(Io\Hooks\Capability::EdgeRegistrations, $caps, true)) {
    die("skip the listener's pair needs EdgeRegistrations");
}
?>
--FILE--
<?php
include __DIR__ . '/../../standard/tests/streams/hooks/scheduler.inc';

$scheduler = new Scheduler(new Io\Ring\Engine(),
    [Io\Hooks\Capability::EdgeRegistrations, Io\Hooks\Capability::DirectAccept]);
Io\Hooks\set_hooks($scheduler);

function connect_to(string $ip, int $port): Socket
{
    $c = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
    socket_connect($c, $ip, $port);
    return $c;
}

$server = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
socket_bind($server, '127.0.0.1', 0);
socket_listen($server, 16);
socket_getsockname($server, $ip, $port);
$clients = [];

$scheduler->spawn(function () use ($server, $ip, $port, &$clients) {
    // The first accept arms the multishot, which takes the connection after it into the ring's
    // buffer; a select with a zero timeout sees it through the queue
    socket_set_nonblock($server);
    var_dump(@socket_accept($server));
    $clients[] = connect_to($ip, $port);
    usleep(50000);
    $r = [$server];
    $w = null;
    $e = null;
    var_dump(socket_select($r, $w, $e, 0), count($r));
    var_dump(@socket_accept($server) !== false);
    // A select that waits is answered when the multishot takes the next connection
    $r = [$server];
    $w = null;
    $e = null;
    $start = hrtime(true);
    var_dump(socket_select($r, $w, $e, 2), (hrtime(true) - $start) < 500_000_000);
    var_dump(@socket_accept($server) !== false);
});
$scheduler->spawn(function () use ($ip, $port, &$clients) {
    usleep(200000);
    $clients[] = connect_to($ip, $port);
});
$scheduler->loop();
Io\Hooks\set_hooks(null);
?>
--EXPECT--
bool(false)
int(1)
int(1)
bool(true)
int(1)
bool(true)
bool(true)
