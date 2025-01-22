--TEST--
Backtrace in nested function call
--FILE--
<?php

function suspend_fiber(): void
{
    \Fiber::suspend();
    throw new Exception;
}

$fiber = new Fiber(function (): void {
    suspend_fiber();
});

$fiber->start();

$fiber->resume();

?>
--EXPECTF--
Fatal error: Uncaught Exception in %sbacktrace-nested.php:%d
Stack trace:
#0 %sbacktrace-nested.php(%d): suspend_fiber()
#1 [internal function]: {closure:%s:%d}()
#2 %sbacktrace-nested.php(%d): Fiber->resume()
#3 {main}
  thrown in %sbacktrace-nested.php on line %d
