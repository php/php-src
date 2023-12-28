--TEST--
Immediate constant access on new object with ctor parentheses
--FILE--
<?php

class A
{
    const C = 'constant';
}

echo new A::C;

?>
--EXPECTF--
Parse error: syntax error, unexpected identifier "C", expecting variable or "$" in %s on line %d
