--TEST--
Primary constructors: not allowed on interfaces
--FILE--
<?php
interface I(public int $x) {}
?>
--EXPECTF--
Parse error: syntax error, unexpected token "(", expecting "{" in %s on line %d
