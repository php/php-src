--TEST--
realpath() emits deprecation warning when passing empty string
--FILE--
<?php

var_dump(realpath(""));

?>
--EXPECTF--
Deprecated: realpath(): Passing empty string to realpath() is deprecated, use getcwd() instead in %s on line %d
string(%d) "%s"
