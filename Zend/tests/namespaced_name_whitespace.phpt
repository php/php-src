--TEST--
Whitespace between namespace separators is no longer allowed
--FILE--
<?php

Foo \ Bar \ Baz;

?>
--EXPECTF--
Parse error: syntax error, unexpected '\' (T_NS_SEPARATOR) in %s on line %d
