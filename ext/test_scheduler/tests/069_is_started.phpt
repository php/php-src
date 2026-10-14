--TEST--
test_scheduler: isStarted() is true once the body began executing, never for a coroutine cancelled before it ran
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php
use function TestScheduler\{spawn, await, suspend, resume, cancel, current};

var_dump(current()->isStarted());

$worker = spawn(function () {
    var_dump(current()->isStarted());
    suspend();
    var_dump(current()->isStarted());
});

// Queued for its first run: QUEUED, but the body has not begun.
var_dump($worker->isStarted());

spawn(function () use ($worker) {
    // Parked, then queued again by resume(): started both times.
    var_dump($worker->isStarted());
    resume($worker);
    var_dump($worker->isStarted());
});

await($worker);
var_dump($worker->isStarted());

$unstarted = spawn(function () {
    echo "unreachable body\n";
});
cancel($unstarted);

try {
    await($unstarted);
} catch (TestScheduler\CancellationError $e) {
    echo $e->getMessage(), "\n";
}

var_dump($unstarted->isFinished(), $unstarted->isStarted());
?>
--EXPECT--
bool(true)
bool(false)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
The coroutine has been cancelled
bool(true)
bool(false)
