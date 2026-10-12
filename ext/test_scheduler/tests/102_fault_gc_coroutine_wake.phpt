--TEST--
test_scheduler: a GC coroutine its last destructor iterator cannot wake stays parked, and the waiting caller meets the deadlock
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php
class Node {
    public $self;

    public function __destruct() {
        echo "destructor\n";
    }
}

$node = new Node();
$node->self = $node;
unset($node);

// The GC coroutine's wake by the iterator's finish handler is the first wake.
ini_set('test_scheduler.fail_wake', 1);

try {
    gc_collect_cycles();
} catch (TestScheduler\CancellationError $e) {
    echo get_class($e), ": ", $e->getMessage(), "\n";
    var_dump($e->getPrevious());
}
?>
--EXPECTF--
destructor
TestScheduler\CancellationError: Deadlock detected
NULL

Fatal error: Uncaught TestScheduler\DeadlockError: Deadlock detected: %s
Stack trace:
#0 {main}
  thrown in %s on line %d
