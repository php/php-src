--TEST--
Start on already running fiber
--FILE--
<?php

$fiber = new Fiber(function (): void {
    Fiber::suspend();
});

$fiber->start();

$fiber->start();

?>
--EXPECTF--
Fatal error: Uncaught FiberError: Cannot start a fiber that has already been started in %sdouble-start.php:%d
Stack trace:
#0 %sdouble-start.php(%d): Fiber->start()
#1 {main}
  thrown in %sdouble-start.php on line %d
