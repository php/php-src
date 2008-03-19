--TEST--
realpath() with relative directory
--FILE--
<?php

var_dump(realpath('.') == getcwd());
chdir('..');
var_dump(realpath('.') == getcwd());

?>
--EXPECT--
bool(true)
bool(true)
