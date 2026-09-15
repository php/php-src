--TEST--
test_scheduler: a destructor deadlocking inside a GC run terminates without crashing
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php
use function TestScheduler\suspend;

class Cyc {
    public $self;
    public function __destruct() {
        echo "dtor suspends\n";
        suspend();
        echo "dtor resumed\n";
    }
}

$a = new Cyc;
$a->self = $a;
unset($a);

gc_collect_cycles();
echo "after gc\n";
?>
--EXPECTF--
dtor suspends

Fatal error: Uncaught TestScheduler\DeadlockError: Deadlock detected: no active coroutines, 3 coroutines in waiting in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
