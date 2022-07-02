--TEST--
Unmixed group use declarations mustn't contain two commas mid-list
--FILE--
<?php
use const Baz\{Foo,,Bar};
?>
--EXPECTF--
Parse error: syntax error, unexpected token ",", expecting "}" in %s on line %d
