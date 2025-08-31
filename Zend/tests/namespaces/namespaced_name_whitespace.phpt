--TEST--
Whitespace between namespace separators is no longer allowed
--FILE--
<?php

Foo \ Bar \ Baz;

?>
--EXPECTF--
Parse error: syntax error, unexpected token "\" in %s on line %d
