--TEST--
test_scheduler: a Fiber whose coroutine cannot be queued fails to start and may start again
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php
ini_set('test_scheduler.fail_enqueue', 1);

$fiber = new Fiber(function () {
    echo "body\n";
});

try {
    $fiber->start();
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

var_dump($fiber->isStarted(), $fiber->isTerminated());

$fiber->start();
var_dump($fiber->isTerminated());
?>
--EXPECT--
Cannot enqueue the coroutine: test_scheduler.fail_enqueue
bool(false)
bool(false)
body
bool(true)
