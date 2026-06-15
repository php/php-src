--TEST--
Literal types: interpolated double-quoted string is not a valid literal type
--FILE--
<?php
function f("foo $bar" $x): void {}
?>
--EXPECTF--
Parse error: syntax error, %s in %s on line %d
