--TEST--
test_scheduler: Fiber::resume() whose body cannot be woken throws and leaves the fiber suspended
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php
$fiber = new Fiber(function () {
    $value = Fiber::suspend();
    echo "resumed with $value\n";
});
$fiber->start();

ini_set('test_scheduler.fail_wake', 1);

try {
    $fiber->resume('first');
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

var_dump($fiber->isSuspended());

$fiber->resume('second');
var_dump($fiber->isTerminated());
?>
--EXPECT--
Cannot enqueue the coroutine: test_scheduler.fail_wake
bool(true)
resumed with second
bool(true)
