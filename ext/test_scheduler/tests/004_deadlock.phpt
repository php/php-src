--TEST--
test_scheduler: suspending with nobody left to resume is a terminal deadlock
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php
use function TestScheduler\suspend;

echo "before\n";
suspend();
echo "unreachable\n";
?>
--EXPECTF--
before

Fatal error: Uncaught TestScheduler\DeadlockError: Deadlock detected: no active coroutines, 1 coroutines in waiting in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
