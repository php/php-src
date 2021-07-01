--TEST--
Cannot start a new fiber in a finally block in a force-closed fiber
--FILE--
<?php

$fiber = new Fiber(function() {
    try {
        Fiber::suspend();
    } finally {
        echo "finally\n";

        $fiber = new Fiber(function() {
            echo "not executed\n";
        });

        $fiber->start();
    }
});

$fiber->start();

?>
--EXPECTF--
finally

Fatal error: Uncaught FiberError: Cannot switch fibers in current execution context in %sno-switch-force-close-finally.php:%d
Stack trace:
#0 %sno-switch-force-close-finally.php(%d): Fiber->start()
#1 [internal function]: {closure}()
#2 {main}
  thrown in %sno-switch-force-close-finally.php on line %d
