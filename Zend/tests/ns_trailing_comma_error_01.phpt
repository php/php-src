--TEST--
Group use declarations mustn't be empty
--FILE--
<?php
use Baz\{};
?>
--EXPECTF--
Parse error: syntax error, unexpected '}', expecting identifier (T_STRING) or function (T_FUNCTION) or const (T_CONST) in %s on line %d
