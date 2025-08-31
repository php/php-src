--TEST--
Group use declarations mustn't be empty
--FILE--
<?php
use Baz\{};
?>
--EXPECTF--
Parse error: syntax error, unexpected token "}", expecting identifier or namespaced name or "function" or "const" in %s on line %d
