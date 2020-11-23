--TEST--
Test scandir() function : error conditions - Non-existent directory
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip.. Not valid for Windows');
}
?>
--FILE--
<?php
/*
 * Pass a directory that does not exist to scandir() to test error messages
 */

echo "*** Testing scandir() : error conditions ***\n";

$directory = __DIR__ . '/idonotexist';

echo "\n-- Pass scandir() an absolute path that does not exist --\n";
var_dump(scandir($directory));

echo "\n-- Pass scandir() a relative path that does not exist --\n";
var_dump(scandir('/idonotexist'));
?>
--EXPECTF--
*** Testing scandir() : error conditions ***

-- Pass scandir() an absolute path that does not exist --

Warning: scandir(%s/idonotexist): Failed to open directory: %s in %s on line %d

Warning: scandir(): (errno %d): %s in %s on line %d
bool(false)

-- Pass scandir() a relative path that does not exist --

Warning: scandir(/idonotexist): Failed to open directory: %s in %s on line %d

Warning: scandir(): (errno %d): %s in %s on line %d
bool(false)
