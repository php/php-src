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
/* Prototype  : array scandir(string $dir [, int $sorting_order [, resource $context]])
 * Description: List files & directories inside the specified path
 * Source code: ext/standard/dir.c
 */

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
===DONE===
--EXPECTF--
*** Testing scandir() : error conditions ***

-- Pass scandir() an absolute path that does not exist --

Warning: scandir(%s/idonotexist): failed to open dir: %s in %s on line %d

Warning: scandir(): (errno %d): %s in %s on line %d
bool(false)

-- Pass scandir() a relative path that does not exist --

Warning: scandir(/idonotexist): failed to open dir: %s in %s on line %d

Warning: scandir(): (errno %d): %s in %s on line %d
bool(false)
===DONE===
