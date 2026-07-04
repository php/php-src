--TEST--
function_exists is case-sensitive, method_exists is case-insensitive
--FILE--
<?php
class MyClass {
    public function myMethod() {}
}

var_dump(function_exists("STRLEN"));
var_dump(function_exists("strlen"));
var_dump(method_exists("MyClass", "MYMETHOD"));
var_dump(method_exists("MyClass", "myMethod"));
?>
--EXPECT--
bool(false)
bool(true)
bool(false)
bool(true)
