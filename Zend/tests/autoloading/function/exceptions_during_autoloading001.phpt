--TEST--
Exception thrown from within autoloading function
--FILE--
<?php

function TestFunc1($classname) {
    echo __METHOD__ . "($classname)\n";
}

function TestFunc2($classname) {
    echo __METHOD__ . "($classname)\n";
    throw new Exception("Function $classname missing");
}

function TestFunc3($classname) {
    echo __METHOD__ . "($classname)\n";
}

autoload_register_function("TestFunc1");
autoload_register_function("TestFunc2");
autoload_register_function("TestFunc3");

try {
    var_dump(function_exists("foo", true));
} catch(Exception $e) {
    echo 'Exception: ' . $e->getMessage() . "\n";
}

?>
--EXPECT--
TestFunc1(foo)
TestFunc2(foo)
Exception: Function foo missing
