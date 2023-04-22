--TEST--
Closure self-reference can't reuse parameter variable name
--FILE--
<?php

function(int $a) as $a {};

?>
--EXPECTF--
Fatal error: Cannot use lexical variable $a as self reference in %s on line %d
