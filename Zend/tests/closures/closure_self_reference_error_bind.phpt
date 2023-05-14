--TEST--
Closure self-reference can't reuse static variable name
--FILE--
<?php

function() as $a use($a) {};

?>
--EXPECTF--
Fatal error: Cannot use lexical variable $a as self reference in %s on line %d
