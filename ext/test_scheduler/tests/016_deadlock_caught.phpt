--TEST--
test_scheduler: catching the cancellation does not suppress the terminal DeadlockError
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php
use function TestScheduler\{spawn, await, suspend};

$c1 = null;
$c2 = null;

$c1 = spawn(function () use (&$c2) {
    try {
        suspend();
        await($c2);
    } finally {
        echo "c1 finally\n";
    }
});

$c2 = spawn(function () use (&$c1) {
    try {
        suspend();
        await($c1);
    } finally {
        echo "c2 finally\n";
    }
});

try {
    await($c1);
    echo "unreachable\n";
} catch (TestScheduler\CancellationError $e) {
    echo "caught: ", $e->getMessage(), "\n";
}

echo "main continues\n";
?>
--EXPECTF--
caught: Deadlock detected
main continues
c1 finally
c2 finally

Fatal error: Uncaught TestScheduler\DeadlockError: Deadlock detected: no active coroutines, 3 coroutines in waiting in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
