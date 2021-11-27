--TEST--
Test autoload_register_function(): basic behaviour
--FILE--
<?php

function TestFunc1($classname) {
    echo __METHOD__ . "($classname)\n";
}

function TestFunc2($classname) {
    echo __METHOD__ . "($classname)\n";
}

echo "===REGISTER===\n";

autoload_register_function("TestFunc1");
autoload_register_function("TestFunc2");
autoload_register_function("TestFunc2"); // 2nd call ignored

var_dump(function_exists("foo", true));

echo "===NOFUNCTION===\n";

try {
    autoload_register_function("unavailable_autoload_function");
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}

?>
--EXPECT--
===REGISTER===
TestFunc1(foo)
TestFunc2(foo)
bool(false)
===NOFUNCTION===
TestFunc1(unavailable_autoload_function)
TestFunc2(unavailable_autoload_function)
autoload_register_function(): Argument #1 ($callback) must be a valid callback, function "unavailable_autoload_function" not found or invalid function name
