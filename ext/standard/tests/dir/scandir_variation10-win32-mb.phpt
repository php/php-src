--TEST--
Test scandir() function : usage variations - different sorting constants
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') {
  die("skip Valid only on Windows");
}
?>
--FILE--
<?php
/* Prototype  : array scandir(string $dir [, int $sorting_order [, resource $context]])
 * Description: List files & directories inside the specified path
 * Source code: ext/standard/dir.c
 */

printf("SCANDIR_SORT_ASCENDING: %d\n", SCANDIR_SORT_ASCENDING);
printf("SCANDIR_SORT_DESCENDING: %d\n", SCANDIR_SORT_DESCENDING);
printf("SCANDIR_SORT_NONE: %d\n", SCANDIR_SORT_NONE);

/*
 * Pass different integers as $sorting_order argument to test how scandir()
 * re-orders the array
 */

echo "*** Testing scandir() : usage variations ***\n";

// include for create_files/delete_files functions
include(dirname(__FILE__) . '/../file/file.inc');

// create directory and files
$dir = dirname(__FILE__) . '/私はガラスを食べられますscandir_variation10';
mkdir($dir);
@create_files($dir, 2, "numeric", 0755, 1, "w", "私はガラスを食べられますfile");

// Deterministic tests.
var_dump(scandir($dir, SCANDIR_SORT_ASCENDING));
var_dump(scandir($dir, SCANDIR_SORT_DESCENDING));

// Non-deterministic tests.
$files = scandir($dir, SCANDIR_SORT_NONE);
var_dump(count($files));
var_dump(in_array('.', $files));
var_dump(in_array('..', $files));
var_dump(in_array('私はガラスを食べられますfile1.tmp', $files));
var_dump(in_array('私はガラスを食べられますfile2.tmp', $files));

delete_files($dir, 2, "私はガラスを食べられますfile");
?>
===DONE===
--CLEAN--
<?php
$dir = dirname(__FILE__) . '/私はガラスを食べられますscandir_variation10';
rmdir($dir);
?>
--EXPECTF--
SCANDIR_SORT_ASCENDING: 0
SCANDIR_SORT_DESCENDING: 1
SCANDIR_SORT_NONE: 2
*** Testing scandir() : usage variations ***
array(4) {
  [0]=>
  string(1) "."
  [1]=>
  string(2) ".."
  [2]=>
  string(45) "私はガラスを食べられますfile1.tmp"
  [3]=>
  string(45) "私はガラスを食べられますfile2.tmp"
}
array(4) {
  [0]=>
  string(45) "私はガラスを食べられますfile2.tmp"
  [1]=>
  string(45) "私はガラスを食べられますfile1.tmp"
  [2]=>
  string(2) ".."
  [3]=>
  string(1) "."
}
int(4)
bool(true)
bool(true)
bool(true)
bool(true)
===DONE===
