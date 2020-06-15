--TEST--
Unmixed group use declarations mustn't begin with a comma
--FILE--
<?php
use function Baz\{,Foo};
?>
--EXPECTF--
Parse error: syntax error, unexpected ',', expecting identifier (T_STRING) or namespaced name (T_NAME_QUALIFIED) in %s on line %d
