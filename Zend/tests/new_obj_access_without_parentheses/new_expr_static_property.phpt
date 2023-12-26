--TEST--
Immediate static property access on new object with ctor parentheses created from expr
--FILE--
<?php

class A
{
    public static $prop = 'property';
}

echo new (trim(' A '))()::$prop;

?>
--EXPECT--
property
