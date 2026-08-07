--TEST--
Test assign of invalid string to typed static int property
--FILE--
<?php

function &nonNumericStringRef() {
    static $a = "x";
    return $a;
}

class Foo {
    public static int $i;
}

try {
    Foo::$i = &nonNumericStringRef();
} catch (TypeError $e) { echo $e::class, ': ', $e->getMessage(), "\n"; }
try {
    var_dump(Foo::$i);
} catch (Error $e) { echo $e::class, ': ', $e->getMessage(), "\n"; }
var_dump(nonNumericStringRef());

?>
--EXPECT--
TypeError: Cannot assign string to property Foo::$i of type int
Error: Typed static property Foo::$i must not be accessed before initialization
string(1) "x"
