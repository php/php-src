--TEST--
Immediate constant access on new object with ctor parentheses
--FILE--
<?php

class A
{
    const C = 'constant';
}

echo new A()::C;

?>
--EXPECT--
constant
