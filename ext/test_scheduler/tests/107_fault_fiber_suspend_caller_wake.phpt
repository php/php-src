--TEST--
test_scheduler: Fiber::suspend() that cannot wake the caller throws in the body, which goes on
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php
$fiber = new Fiber(function () {
    try {
        Fiber::suspend('value');
    } catch (Error $e) {
        echo "body: ", $e->getMessage(), "\n";
    }

    echo "body goes on\n";

    return 'done';
});

// The caller's wake by the suspend is the first wake.
ini_set('test_scheduler.fail_wake', 1);
var_dump($fiber->start());
var_dump($fiber->getReturn());
?>
--EXPECT--
body: Cannot enqueue the coroutine: test_scheduler.fail_wake
body goes on
NULL
string(4) "done"
