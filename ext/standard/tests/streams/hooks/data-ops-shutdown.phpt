--TEST--
IO hooks: request shutdown with a fiber suspended in a read
--FILE--
<?php
include __DIR__ . '/scheduler.inc';

$scheduler = new Scheduler();
Io\Hooks\set_hooks($scheduler);
[$r, $w] = stream_socket_pair(PHP_OS_FAMILY === 'Windows' ? STREAM_PF_INET : STREAM_PF_UNIX, STREAM_SOCK_STREAM, 0);

$fiber = new Fiber(function () use ($r) {
    $data = fread($r, 10);
    echo "unexpected: ", var_export($data, true), "\n";
});
$fiber->start();   // suspends inside fread() with the operation submitted
echo "suspended: ", var_export($fiber->isSuspended(), true), "\n";
echo "done\n";
// The fiber, the streams and the queue are destroyed at shutdown, in that order
?>
--EXPECT--
suspended: true
done
