--TEST--
Immediate property access on new object with ctor parentheses
--FILE--
<?php

class A
{
    public $prop = 'property';
}

echo new A()->prop;

?>
--EXPECT--
property
