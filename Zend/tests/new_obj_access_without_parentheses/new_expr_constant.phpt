--TEST--
Immediate constant access on new object with ctor parentheses created from expr
--FILE--
<?php

class A
{
    const C = 'constant';
}

echo new (trim(' A '))()::C;

?>
--EXPECT--
constant
