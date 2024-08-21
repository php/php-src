--TEST--
Test autoload_register_class(): basic function behavior 001
--FILE--
<?php

//autoload_register_class(var_dump(...));
//autoload_register_class('var_export');

/*
try {
    foo();
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}
*/
echo "===EMPTY===\n";

function TestFunc1($classname) {
    echo __METHOD__ . "($classname)\n";
}

function TestFunc2($classname) {
    echo __METHOD__ . "($classname)\n";
}

echo "===REGISTER===\n";

autoload_register_class("TestFunc1");
autoload_register_class("TestFunc2");
autoload_register_class("TestFunc2"); // 2nd call ignored

var_dump(class_exists("TestClass", true));

echo "===LOAD===\n";

autoload_register_class("spl_autoload");
var_dump(class_exists("TestClass", true));

echo "===NOFUNCTION===\n";

try {
    autoload_register_class("unavailable_autoload_function");
} catch(\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
===EMPTY===
===REGISTER===
TestFunc1(TestClass)
TestFunc2(TestClass)
bool(false)
===LOAD===
TestFunc1(TestClass)
TestFunc2(TestClass)
bool(false)
===NOFUNCTION===
autoload_register_class(): Argument #1 ($callback) must be a valid callback, function "unavailable_autoload_function" not found or invalid function name
