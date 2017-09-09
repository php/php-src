--TEST--
tests Fiber status
--FILE--
<?php
function bar()
{
    $a = null;
    Fiber::yield();
    $a->foo();
}
function foo()
{
    bar();
}
$f = new Fiber(function () {
    foo();
});

$f->resume();
$f->resume();

--EXPECTF--
Fatal error: Uncaught Error: Call to a member function foo() on null in %s/Zend/tests/fiber/007.php:6
Stack trace:
#0 %s/Zend/tests/fiber/007.php(10): bar()
#1 %s/Zend/tests/fiber/007.php(13): foo()
#2 [internal function]: {closure}()
#3 %s/Zend/tests/fiber/007.php(17): Fiber->resume()
#4 {main}
  thrown in %s/Zend/tests/fiber/007.php on line 6
