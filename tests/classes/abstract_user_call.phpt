--TEST--
ZE2 An abstrcat method cannot be called indirectly
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
--EXPECT--
test::func()
call_user_func(): Argument #1 ($callback) must be a valid callback, cannot call abstract method test_base::func()
