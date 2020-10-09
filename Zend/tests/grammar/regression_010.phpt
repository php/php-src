--TEST--
Test to check regressions on T_IMPLEMENTS followed by a T_NS_SEPARATOR
--FILE--
<?php

interface A{}

// No longer considered legal in PHP 8.
class B implements\A {}

echo "Done", PHP_EOL;

?>
--EXPECTF--
Parse error: syntax error, unexpected namespaced name "implements\A", expecting "{" in %s on line %d
