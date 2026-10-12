--TEST--
test_scheduler: a fiber body that throws and cannot hand the error to the caller in start() leaves it to the deadlock detection
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php
$fiber = new Fiber(function () {
    echo "body\n";
    throw new Exception('body error');
});

// The caller's wake with the body's error is the first wake.
ini_set('test_scheduler.fail_wake', 1);

try {
    $fiber->start();
} catch (TestScheduler\CancellationError $e) {
    echo "start: ", $e->getMessage(), "\n";
}

var_dump($fiber->isTerminated());
?>
--EXPECTF--
body
start: Deadlock detected
bool(true)

Fatal error: Uncaught TestScheduler\DeadlockError: Deadlock detected: %s
Stack trace:
#0 {main}
  thrown in %s on line %d
