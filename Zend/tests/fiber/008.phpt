--TEST--
tests Fiber in internal call
--FILE--
<?php
function foo()
{
    Fiber::yield();
}
$f = new Fiber(function () {
    array_map('foo', [1,2]);
});

echo $f->resume();
--EXPECTF--
Fatal error: Uncaught Error: Cannot call Fiber::yield in internal call in %s/Zend/tests/fiber/008.php:4
Stack trace:
#0 %s/Zend/tests/fiber/008.php(4): Fiber::yield()
#1 [internal function]: foo(1)
#2 %s/Zend/tests/fiber/008.php(7): array_map('foo', Array)
#3 [internal function]: {closure}()
#4 %s/Zend/tests/fiber/008.php(10): Fiber->resume()
#5 {main}
  thrown in %s/Zend/tests/fiber/008.php on line 4
