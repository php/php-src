--TEST--
Closure self-reference can't reuse parameter variable name (arrow function)
--FILE--
<?php

fn(int $a) as $a => 1;

?>
--EXPECTF--
Fatal error: Cannot use lexical variable $a as self reference in %s on line %d
