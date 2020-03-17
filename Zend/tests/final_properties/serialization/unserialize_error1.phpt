--TEST--
Test that final properties can only be assigned to once during unserialization
--XFAIL--
Doesn't work yet.
--FILE--
<?php

class MyClass
{
    final public int $finalProp;
}

var_dump(unserialize('O:7:"MyClass":2:{s:9:"finalProp";i:2;s:9:"finalProp";i:3;}'));

?>
--EXPECT--
Cannot modify final property Foo::$finalProp after initialization
