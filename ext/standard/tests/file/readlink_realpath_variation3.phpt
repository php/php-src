--TEST--
Test readlink() and realpath() functions: usage variation - invalid args
--FILE--
<?php
echo "*** Testing readlink() and realpath() : usage variations ***\n";

echo "\n*** Testing readlink() and realpath() with link name as empty string, NULL and single space ***\n";

echo 'readlink with link as spaces', PHP_EOL;
var_dump(readlink(' '));
echo 'realpath with link as spaces', PHP_EOL;
var_dump(realpath(' '));

echo 'readlink with link as empty string', PHP_EOL;
var_dump(readlink(''));
echo 'realpath with link as empty string', PHP_EOL;
var_dump(realpath(''));

echo "Done\n";
?>
--EXPECTF--
*** Testing readlink() and realpath() : usage variations ***

*** Testing readlink() and realpath() with link name as empty string, NULL and single space ***
readlink with link as spaces

Warning: readlink(): %s in %s on line %d
bool(false)
realpath with link as spaces
bool(false)
readlink with link as empty string

Warning: readlink(): %s in %s on line %d
bool(false)
realpath with link as empty string
string(%d) "%s"
Done
