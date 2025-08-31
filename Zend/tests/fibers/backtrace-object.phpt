--TEST--
Backtrace in with object as fiber callback
--FILE--
<?php

class Test
{
    public function __invoke(string $arg): void
    {
        Fiber::suspend();
        throw new Exception($arg);
    }
}

$fiber = new Fiber(new Test);

$fiber->start('test');

$fiber->resume();

?>
--EXPECTF--
Fatal error: Uncaught Exception: test in %sbacktrace-object.php:%d
Stack trace:
#0 [internal function]: Test->__invoke('test')
#1 %sbacktrace-object.php(%d): Fiber->resume()
#2 {main}
  thrown in %sbacktrace-object.php on line %d
