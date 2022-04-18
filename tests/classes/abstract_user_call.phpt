--TEST--
ZE2 An abstract method cannot be called indirectly
--FILE--
<?php

abstract class test_base
{
    abstract function func();
}

class test extends test_base
{
    function func()
    {
        echo __METHOD__ . "()\n";
    }
}

$o = new test;

$o->func();

try {
    call_user_func(array($o, 'test_base::func'));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
test::func()

Deprecated: Callables of the form ["test", "test_base::func"] are deprecated in %s on line %d
call_user_func(): Argument #1 ($callback) must be a valid callback, cannot call abstract method test_base::func()
