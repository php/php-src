--TEST--
Intersection type cannot be nullable
--FILE--
<?php

function foo(): ?Countable&Iterator {}

?>
--EXPECTF--
Parse error: syntax error, unexpected token "&", expecting "=>" or "{" in %s on line %d
