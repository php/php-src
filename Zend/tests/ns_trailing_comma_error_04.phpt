--TEST--
Group use declarations mustn't begin with a comma
--FILE--
<?php
use Baz\{,Foo};
?>
--EXPECTF--
Parse error: syntax error, unexpected token ",", expecting identifier or "function" or "const" in %s on line %d
