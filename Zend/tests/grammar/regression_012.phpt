--TEST--
Testing for regression on const list syntax and arrays
--FILE--
<?php

class A
{
    const A = [1, foreach ];
}

?>
--EXPECTF--
Parse error: syntax error, unexpected 'FOREACH' (T_FOREACH), expecting ']' in %s on line %d
