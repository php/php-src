--TEST--
Test autoload_unregister_function(): basic function behavior
--FILE--
<?php

function TestFunc1($classname)
{
    echo __METHOD__ . "($classname)\n";
}

function TestFunc2($classname)
{
    echo __METHOD__ . "($classname)\n";
}

autoload_register_function('TestFunc1');
autoload_register_function('TestFunc2');
autoload_register_function('TestFunc1');
var_dump(function_exists("foo", true));

var_dump(autoload_unregister_function('TestFunc1'));
var_dump(autoload_unregister_function('TestFunc1'));
var_dump(function_exists("foo", true));

?>
--EXPECT--
TestFunc1(foo)
TestFunc2(foo)
bool(false)
bool(true)
bool(false)
TestFunc2(foo)
bool(false)
