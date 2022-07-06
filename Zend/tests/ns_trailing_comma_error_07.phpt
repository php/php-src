--TEST--
Unmixed group use declarations mustn't begin with a comma
--FILE--
<?php
use function Baz\{,Foo};
?>
--EXPECTF--
Parse error: syntax error, unexpected token ",", expecting identifier or namespaced name in %s on line %d
