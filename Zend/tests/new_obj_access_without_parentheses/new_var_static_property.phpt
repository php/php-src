--TEST--
Immediate static property access on new object with ctor parentheses created from var
--FILE--
<?php

class A
{
    public static $prop = 'property';
}

$class = A::class;

echo new $class()::$prop;

?>
--EXPECT--
property
