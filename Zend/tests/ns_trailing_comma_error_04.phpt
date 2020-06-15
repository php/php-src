--TEST--
Group use declarations mustn't begin with a comma
--FILE--
<?php
use Baz\{,Foo};
?>
--EXPECTF--
Parse error: syntax error, unexpected ',', expecting identifier (T_STRING) or namespaced name (T_NAME_QUALIFIED) or function (T_FUNCTION) or const (T_CONST) in %s on line %d
