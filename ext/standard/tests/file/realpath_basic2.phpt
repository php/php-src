--TEST--
realpath() with relative directory
--FILE--
<?php

var_dump(realpath('.') == realpath(getcwd()));
chdir('..');
var_dump(realpath('.') == realpath(getcwd()));

?>
--EXPECT--
bool(true)
bool(true)
