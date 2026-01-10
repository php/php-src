--TEST--
GH-10992 (Improper long path support for relative paths)
--FILE--
<?php
$dir = str_repeat('b', 250 - strlen(getcwd()));
var_dump(mkdir($dir));
var_dump(rmdir($dir));
$dir = str_repeat('b', 265 - strlen(getcwd()));
var_dump(mkdir($dir));
var_dump(rmdir($dir));
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
