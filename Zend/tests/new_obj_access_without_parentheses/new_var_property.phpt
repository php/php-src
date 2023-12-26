--TEST--
Immediate property access on new object with ctor parentheses created from var
--FILE--
<?php

class A
{
    public $prop = 'property';
}

$class = A::class;

echo new $class()->prop;

?>
--EXPECT--
property
