--TEST--
tests Fiber status
--FILE--
<?php
function bar()
{
    $a = null;
    await;
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
try {
    $f->resume();
} catch (Error $e) {
    echo $e->getMessage(), "\n";
    echo $e->getTraceAsString(), "\n";
}

--EXPECTF--
Call to a member function foo() on null
#0 %s/Zend/tests/fiber/007.php(10): bar()
#1 %s/Zend/tests/fiber/007.php(13): foo()
#2 [internal function]: {closure}()
#3 %s/Zend/tests/fiber/007.php(18): Fiber->resume()
#4 {main}
