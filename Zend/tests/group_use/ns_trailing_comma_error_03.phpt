--TEST--
Group use declarations mustn't allow more than one comma
--FILE--
<?php
use Baz\{Foo,,};
?>
--EXPECTF--
Parse error: syntax error, unexpected token ",", expecting "}" in %s on line %d
