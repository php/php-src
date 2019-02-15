--TEST--
Test scandir() function : basic functionality
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

/*
 * Test basic functionality of scandir()
 */

echo "*** Testing scandir() : basic functionality ***\n";

// include file.inc for create_files function
include (dirname(__FILE__) . '/../file/file.inc');

// set up directory
$directory = dirname(__FILE__) . '/私はガラスを食べられますscandir_basic';
mkdir($directory);
create_files($directory, 3, "numeric", 0755, 1, "w", "私はガラスを食べられますfile");

echo "\n-- scandir() with mandatory arguments --\n";
var_dump(scandir($directory));

echo "\n-- scandir() with all arguments --\n";
$sorting_order = SCANDIR_SORT_DESCENDING;
$context = stream_context_create();
var_dump(scandir($directory, $sorting_order, $context));

delete_files($directory, 3, "私はガラスを食べられますfile");
?>
===DONE===
--CLEAN--
<?php
$directory = dirname(__FILE__) . '/私はガラスを食べられますscandir_basic';
rmdir($directory);
?>
--EXPECT--
*** Testing scandir() : basic functionality ***

-- scandir() with mandatory arguments --
array(5) {
  [0]=>
  string(1) "."
  [1]=>
  string(2) ".."
  [2]=>
  string(45) "私はガラスを食べられますfile1.tmp"
  [3]=>
  string(45) "私はガラスを食べられますfile2.tmp"
  [4]=>
  string(45) "私はガラスを食べられますfile3.tmp"
}

-- scandir() with all arguments --
array(5) {
  [0]=>
  string(45) "私はガラスを食べられますfile3.tmp"
  [1]=>
  string(45) "私はガラスを食べられますfile2.tmp"
  [2]=>
  string(45) "私はガラスを食べられますfile1.tmp"
  [3]=>
  string(2) ".."
  [4]=>
  string(1) "."
}
===DONE===
