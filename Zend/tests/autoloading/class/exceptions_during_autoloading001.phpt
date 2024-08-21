--TEST--
Exception thrown from within autoloading function
--FILE--
<?php

function TestFunc1($classname) {
    echo __METHOD__ . "($classname)\n";
}

function TestFunc2($classname) {
    echo __METHOD__ . "($classname)\n";
    throw new Exception("Class $classname missing");
}

function TestFunc3($classname) {
    echo __METHOD__ . "($classname)\n";
}

autoload_register_class("TestFunc1");
autoload_register_class("TestFunc2");
autoload_register_class("TestFunc3");

try {
    var_dump(class_exists("TestClass", true));
} catch(Exception $e) {
    echo 'Exception: ' . $e->getMessage() . "\n";
}

?>
--EXPECT--
TestFunc1(TestClass)
TestFunc2(TestClass)
Exception: Class TestClass missing
