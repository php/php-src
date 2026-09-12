--TEST--
test_scheduler: a fiber awaiting inside its body is running, not suspended
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php
use function TestScheduler\{spawn, await, suspend, resume};

$gate = spawn(function () { suspend(); });

$obs = spawn(function () use (&$fiber, $gate) {
    suspend(); // parked until the fiber wakes us
    var_dump($fiber->isSuspended()); // await inside the body: not suspended
    var_dump($fiber->isRunning());   // ... it is running, blocked internally
    resume($gate);
});

$fiber = new Fiber(function () use ($gate, $obs) {
    resume($obs);
    await($gate);
    Fiber::suspend("yielded");
    return "end";
});

$fiber->start();

// Now parked in Fiber::suspend() proper: the classic contract holds.
var_dump($fiber->isSuspended());
var_dump($fiber->isRunning());

$fiber->resume();
var_dump($fiber->isTerminated());
echo "==DONE==\n";
?>
--EXPECT--
bool(false)
bool(true)
bool(true)
bool(false)
bool(true)
==DONE==
