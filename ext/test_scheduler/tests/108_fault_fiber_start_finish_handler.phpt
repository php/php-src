--TEST--
test_scheduler: a Fiber whose coroutine gets no finish handler fails to start and may start again
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php
$fiber = new Fiber(function () {
    echo "body\n";
});

ini_set('test_scheduler.fail_finish_handler', 1);

try {
    $fiber->start();
} catch (FiberError $e) {
    echo $e->getMessage(), "\n";
}

var_dump($fiber->isStarted());

$fiber->start();
var_dump($fiber->isTerminated());
?>
--EXPECT--
Cannot start the fiber: the scheduler refused to report its end
bool(false)
body
bool(true)
