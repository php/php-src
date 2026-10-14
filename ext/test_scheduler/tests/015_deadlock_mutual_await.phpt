--TEST--
test_scheduler: two coroutines awaiting each other deadlock the waiting main
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php
use function TestScheduler\{spawn, await, suspend};

echo "start\n";

$c1 = null;
$c2 = null;

$c1 = spawn(function () use (&$c2) {
    try {
        echo "c1 running\n";
        suspend();
        await($c2);
        echo "c1 unreachable\n";
    } finally {
        echo "c1 finally\n";
    }
});

$c2 = spawn(function () use (&$c1) {
    try {
        echo "c2 running\n";
        suspend();
        await($c1);
        echo "c2 unreachable\n";
    } finally {
        echo "c2 finally\n";
    }
});

await($c1);
echo "main unreachable\n";
?>
--EXPECTF--
start
c1 running
c2 running
c1 finally
c2 finally

Fatal error: Uncaught TestScheduler\DeadlockError: Deadlock detected: no active coroutines, 3 coroutines in waiting in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
