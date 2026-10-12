--TEST--
test_scheduler: a fiber whose last holder, its starter, is cancelled while the body waits lives until the body ends, with no finish handler
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php
use function TestScheduler\{spawn, await, suspend, resume, cancel};

$blocker = spawn(function () {
    suspend();
});

$starter = spawn(function () use ($blocker) {
    $fiber = new Fiber(function () use ($blocker) {
        try {
            await($blocker);
            echo "not reached\n";
        } finally {
            echo "body finally: current is running: ", var_export(Fiber::getCurrent()?->isRunning(), true), "\n";
        }
    });
    $fiber->start();
});

// The starter waits in start(), the body in its await.
await(spawn(function () {}));

// Whatever the fiber's destructor adds is refused.
ini_set('test_scheduler.fail_finish_handler', 1);
cancel($starter);

try {
    await($starter);
} catch (TestScheduler\CancellationError $e) {
    echo "starter cancelled\n";
}

resume($blocker);
await($blocker);
echo "main end\n";
?>
--EXPECT--
body finally: current is running: true
starter cancelled
main end
