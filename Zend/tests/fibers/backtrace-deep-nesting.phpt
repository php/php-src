--TEST--
Backtrace in deeply nested function call
--FILE--
<?php

function suspend_fiber(int $level): void
{
    if ($level >= 10) {
        $value = \Fiber::suspend($level);
        failing_function($value);
    }

    suspend_fiber($level + 1);
}

function failing_function(string $value): never
{
    throw_exception();
}

function throw_exception(): never
{
    throw new Exception;
}

$fiber = new Fiber(function (): void {
    suspend_fiber(0);
});

$fiber->start();

$fiber->resume('test');

?>
--EXPECTF--
Fatal error: Uncaught Exception in %sbacktrace-deep-nesting.php:%d
Stack trace:
#0 %sbacktrace-deep-nesting.php(%d): throw_exception()
#1 %sbacktrace-deep-nesting.php(%d): failing_function('test')
#2 %sbacktrace-deep-nesting.php(%d): suspend_fiber(10)
#3 %sbacktrace-deep-nesting.php(%d): suspend_fiber(9)
#4 %sbacktrace-deep-nesting.php(%d): suspend_fiber(8)
#5 %sbacktrace-deep-nesting.php(%d): suspend_fiber(7)
#6 %sbacktrace-deep-nesting.php(%d): suspend_fiber(6)
#7 %sbacktrace-deep-nesting.php(%d): suspend_fiber(5)
#8 %sbacktrace-deep-nesting.php(%d): suspend_fiber(4)
#9 %sbacktrace-deep-nesting.php(%d): suspend_fiber(3)
#10 %sbacktrace-deep-nesting.php(%d): suspend_fiber(2)
#11 %sbacktrace-deep-nesting.php(%d): suspend_fiber(1)
#12 %sbacktrace-deep-nesting.php(%d): suspend_fiber(0)
#13 [internal function]: {closure}()
#14 %sbacktrace-deep-nesting.php(%d): Fiber->resume('test')
#15 {main}
  thrown in %sbacktrace-deep-nesting.php on line %d
