--TEST--
Immediate constant access on new object with ctor parentheses created from var
--FILE--
<?php

class A
{
    const C = 'constant';
}

$class = A::class;

echo new $class()::C;

?>
--EXPECT--
constant
