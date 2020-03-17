--TEST--
Test that final properties cannot be modified even when they were serialized as non-final
--XFAIL--
Doesn't work yet
--FILE--
<?php

class MyClass
{
    final public int $finalProp = 1;
}

$foo = unserialize('O:7:"MyClass":2:{s:9:"finalProp";i:2;s:7:"refProp";R:2;}');

$foo->refProp = 4;
var_dump($foo->finalProp);

?>
--EXPECT--
int(2)
