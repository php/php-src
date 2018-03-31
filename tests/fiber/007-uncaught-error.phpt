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

?>
--EXPECTF--
Fatal error: Uncaught Error: Call to a member function foo() on null in %s%e007-uncaught-error.php:6
Stack trace:
#0 %s%e007-uncaught-error.php(10): bar()
#1 %s%e007-uncaught-error.php(13): foo()
#2 (0): {closure}()
#3 {main}
  thrown in %s%e007-uncaught-error.php on line 6
