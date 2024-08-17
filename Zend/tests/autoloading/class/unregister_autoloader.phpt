--TEST--
Test autoload_unregister_class(): basic function behavior
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

autoload_register_class('TestFunc1');
autoload_register_class('TestFunc2');
autoload_register_class('TestFunc1');
var_dump(class_exists("TestClass", true));

var_dump(autoload_unregister_class('TestFunc1'));
var_dump(autoload_unregister_class('TestFunc1'));
var_dump(class_exists("TestClass", true));

?>
--EXPECT--
TestFunc1(TestClass)
TestFunc2(TestClass)
bool(false)
bool(true)
bool(false)
TestFunc2(TestClass)
bool(false)
