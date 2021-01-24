--TEST--
Test parse errors for invalid destructuring.
--FILE--
<?php
[: $$varName] = $values;
?>
--EXPECTF--
Parse error: syntax error, unexpected token "$", expecting "&" or variable in %s on line 2
