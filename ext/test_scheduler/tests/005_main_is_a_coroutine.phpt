--TEST--
test_scheduler: the top-level script is the main coroutine
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php
use function TestScheduler\{current, spawn, suspend, resume};

$main = current();

var_dump($main instanceof TestScheduler\Coroutine);
var_dump($main->isRunning());

spawn(function () use ($main) {
    var_dump(current() !== $main);
    var_dump($main->isSuspended());
    resume($main);
});

suspend();
var_dump($main->isRunning());
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
