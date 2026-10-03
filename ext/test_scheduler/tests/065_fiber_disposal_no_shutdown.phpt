--TEST--
test_scheduler: disposing a suspended fiber does not raise shutdown
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php

$fiber = new Fiber(function () {
    echo "fiber: parking\n";
    Fiber::suspend();
    echo "not reached\n";
});

$fiber->start();

/* The engine sends a graceful exit into the parked stack to unwind it. That
 * is a disposal, not an exit(), and must not shut the scheduler down. */
$fiber = null;
gc_collect_cycles();

echo "disposed\n";
?>
--EXPECT--
fiber: parking
disposed
