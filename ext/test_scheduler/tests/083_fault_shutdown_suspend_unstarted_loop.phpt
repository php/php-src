--TEST--
test_scheduler: a shutdown destructor that parks with no iterator and no loop running ends in a deadlock, not a crash
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php
class Parker {
    public function __destruct() {
        echo "parking\n";
        TestScheduler\suspend();
        echo "not reached\n";
    }
}

$parker = new Parker();

/* Nothing was spawned, so no loop runs after the script, and the iterator
 * that would start one cannot be created. */
ini_set('test_scheduler.fail_new_coroutine', 1);
echo "end of script\n";
?>
--EXPECTF--
end of script
parking

Fatal error: Uncaught TestScheduler\CancellationError: Deadlock detected in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
