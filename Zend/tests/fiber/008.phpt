--TEST--
tests Fiber during internal call
--FILE--
<?php
function foo()
{
    await;
}
$f = new Fiber(function () {
    array_map('foo', [1,2]);
});

echo $f->resume();
--EXPECTF--
Fatal error: Uncaught Error: Cannot await in internal call in %s/Zend/tests/fiber/008.php:4
Stack trace:
#0 [internal function]: foo(1)
#1 %s/Zend/tests/fiber/008.php(7): array_map('foo', Array)
#2 [internal function]: {closure}()
#3 %s/Zend/tests/fiber/008.php(10): Fiber->resume()
#4 {main}
  thrown in %s/Zend/tests/fiber/008.php on line 4
