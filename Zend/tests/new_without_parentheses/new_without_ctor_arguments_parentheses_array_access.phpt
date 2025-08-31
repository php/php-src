--TEST--
Immediate array access on new object without constructor parentheses
--FILE--
<?php

class A extends ArrayObject
{
}

echo new A['test'];

?>
--EXPECTF--
Parse error: syntax error, unexpected token "[", expecting "," or ";" in %s on line %d
