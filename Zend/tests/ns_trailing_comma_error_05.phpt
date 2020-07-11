--TEST--
Group use declarations mustn't contain two commas mid-list
--FILE--
<?php
use Baz\{Foo,,Bar};
?>
--EXPECTF--
Parse error: syntax error, unexpected ',', expecting '}' in %s on line %d
