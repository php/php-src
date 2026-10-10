--TEST--
IO hooks: a blocking Socket's op is bounded by SO_RCVTIMEO, read once and again after socket_set_option()
--EXTENSIONS--
sockets
--SKIPIF--
<?php if (PHP_OS_FAMILY === 'Windows') die('skip SO_RCVTIMEO is rounded to the tick on Windows'); ?>
--FILE--
<?php
include __DIR__ . '/../../standard/tests/streams/hooks/scheduler.inc';

$scheduler = new Scheduler();
Io\Hooks\set_hooks($scheduler);

socket_create_pair(AF_UNIX, SOCK_STREAM, 0, $pair);
[$a, $b] = $pair;

function timed_recv(Socket $s): float
{
    $start = hrtime(true);
    $r = @socket_recv($s, $buf, 10, 0);
    var_dump($r, socket_last_error($s) === SOCKET_EAGAIN || socket_last_error($s) === SOCKET_ETIMEDOUT);
    return (hrtime(true) - $start) / 1e6;
}

$scheduler->spawn(function () use ($a) {
    socket_set_option($a, SOL_SOCKET, SO_RCVTIMEO, ['sec' => 0, 'usec' => 100000]);
    $ms = timed_recv($a);
    var_dump($ms >= 90 && $ms < 1000);
    // The cached timeout is forgotten by the next socket_set_option()
    socket_set_option($a, SOL_SOCKET, SO_RCVTIMEO, ['sec' => 0, 'usec' => 300000]);
    $ms = timed_recv($a);
    var_dump($ms >= 290 && $ms < 1500);
    // An unrelated option forgets it too, and the value is read again unchanged
    socket_set_option($a, SOL_SOCKET, SO_KEEPALIVE, 1);
    $ms = timed_recv($a);
    var_dump($ms >= 290 && $ms < 1500);
});
$scheduler->loop();
Io\Hooks\set_hooks(null);
?>
--EXPECT--
bool(false)
bool(true)
bool(true)
bool(false)
bool(true)
bool(true)
bool(false)
bool(true)
bool(true)
