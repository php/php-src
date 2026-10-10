--TEST--
test_scheduler: exit() inside a fiber raises the shutdown notification
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php

$fiber = new Fiber(function () {
    echo "fiber: running\n";
    exit(7);
});

$fiber->start();

echo "not reached\n";
?>
--EXPECT--
fiber: running
[scheduler] shutdown requested
