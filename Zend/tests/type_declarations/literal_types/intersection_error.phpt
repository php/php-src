--TEST--
Literal types: literals may not appear in intersection types
--FILE--
<?php
interface Foo {}
function f(1&Foo $x): void {}
?>
--EXPECTF--
Parse error: syntax error, %s in %s on line %d
