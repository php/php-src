--TEST--
Io\Ring\Engine: a select on a listener sees the connections its multishot accept took
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
include __DIR__ . '/scheduler.inc';

$scheduler = new Scheduler(new Io\Ring\Engine(),
    [Io\Hooks\Capability::EdgeRegistrations, Io\Hooks\Capability::DirectAccept]);
Io\Hooks\set_hooks($scheduler);

function select_then_accept($server, int $timeout, bool $write = false): string
{
    $r = [$server];
    $w = $write ? [$server] : null;
    $e = null;
    $start = hrtime(true);
    $n = stream_select($r, $w, $e, $timeout);
    $atOnce = (hrtime(true) - $start) < 500_000_000;
    $conn = @stream_socket_accept($server, 0);
    return sprintf("select: %d, at once: %s, accepted: %s",
        $n, var_export($atOnce, true), var_export($conn !== false, true));
}

// A blocking accept arms the multishot; the two connections after it go to the ring's buffer,
// and the usual loop of a select and a non-blocking accept takes them
$server = stream_socket_server('tcp://127.0.0.1:0');
$addr = 'tcp://' . stream_socket_get_name($server, false);
$clients = [];
$scheduler->spawn(function () use ($server, $addr, &$clients) {
    $clients[] = stream_socket_client($addr);
    var_dump(stream_socket_accept($server, 5) !== false);
    $clients[] = stream_socket_client($addr);
    $clients[] = stream_socket_client($addr);
    usleep(50000);
    stream_set_blocking($server, false);
    echo select_then_accept($server, 2), "\n";
    // A stream in both sets is one poll for reading and writing
    echo select_then_accept($server, 2, true), "\n";
});
$scheduler->loop();

// A select waiting while the multishot is armed sees the connection the multishot takes
$scheduler->spawn(function () use ($server) {
    echo select_then_accept($server, 2), "\n";
});
$scheduler->spawn(function () use ($addr, &$clients) {
    $clients[] = stream_socket_client($addr);
});
$scheduler->loop();

// A non-blocking accept with nothing pending arms no multishot: the next connection stays in the
// kernel, where even a select with a zero timeout sees it
$server = stream_socket_server('tcp://127.0.0.1:0');
$addr = 'tcp://' . stream_socket_get_name($server, false);
$scheduler->spawn(function () use ($server, $addr, &$clients) {
    var_dump(@stream_socket_accept($server, 0));
    $clients[] = stream_socket_client($addr);
    usleep(50000);
    stream_set_blocking($server, false);
    echo select_then_accept($server, 0), "\n";
});
$scheduler->loop();

Io\Hooks\set_hooks(null);
?>
--EXPECT--
bool(true)
select: 1, at once: true, accepted: true
select: 1, at once: true, accepted: true
select: 1, at once: true, accepted: true
bool(false)
select: 1, at once: true, accepted: true
