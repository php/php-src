--TEST--
test_scheduler: Fiber::throw() whose body cannot be woken throws and leaves the fiber suspended
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php
$fiber = new Fiber(function () {
    try {
        Fiber::suspend();
    } catch (Exception $e) {
        echo "body caught ", $e->getMessage(), "\n";
    }
});
$fiber->start();

ini_set('test_scheduler.fail_wake', 1);

try {
    $fiber->throw(new Exception('first'));
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

var_dump($fiber->isSuspended());

$fiber->throw(new Exception('second'));
var_dump($fiber->isTerminated());
?>
--EXPECT--
Cannot enqueue the coroutine: test_scheduler.fail_wake
bool(true)
body caught second
bool(true)
