--TEST--
Immediate property access on new object without constructor parentheses
--FILE--
<?php

class A
{
    public $prop = 'property';
}

echo new A->prop;

?>
--EXPECTF--
Parse error: syntax error, unexpected token "->", expecting "," or ";" in %s on line %d
