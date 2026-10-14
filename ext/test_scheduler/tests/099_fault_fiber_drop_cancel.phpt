--TEST--
test_scheduler: a suspended fiber dropped when its body cannot be cancelled throws at the drop, and the body unwinds after main
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php
$fiber = new Fiber(function () {
    try {
        Fiber::suspend();
        echo "not reached\n";
    } finally {
        echo "body finally\n";
    }
});
$fiber->start();

ini_set('test_scheduler.fail_cancel', 1);

try {
    $fiber = null;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

echo "main end\n";
?>
--EXPECT--
Cannot cancel the coroutine: test_scheduler.fail_cancel
main end
body finally
