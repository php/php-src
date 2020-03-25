--TEST--
Unmixed group use declarations mustn't begin with a comma
--FILE--
<?php
use function Baz\{,Foo};
?>
--EXPECTF--
Parse error: syntax error, unexpected ',', expecting identifier (T_STRING) in %s on line %d
