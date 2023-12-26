--TEST--
Immediate static property access on new object with ctor parentheses
--FILE--
<?php

class A
{
    public static $prop = 'property';
}

echo new A()::$prop;

?>
--EXPECT--
property
