--TEST--
Group use declarations mustn't begin with a comma
--FILE--
<?php
use Baz\{,Foo};
?>
--EXPECTF--
Parse error: syntax error, unexpected ',', expecting identifier (T_STRING) or function (T_FUNCTION) or const (T_CONST) in %s on line %d
