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
} catch (TypeError $e) { print $e->getMessage()."\n"; }
try {
    var_dump(Foo::$i);
} catch (Error $e) { print $e->getMessage()."\n"; }
var_dump(nonNumericStringRef());

?>
--EXPECT--
Cannot assign string to property Foo::$i of type int
Typed static property Foo::$i must not be accessed before initialization
string(1) "x"
