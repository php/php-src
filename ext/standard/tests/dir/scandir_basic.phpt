--TEST--
Test scandir() function : basic functionality
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
include (__DIR__ . '/../file/file.inc');

// set up directory
$directory = __DIR__ . '/scandir_basic';
mkdir($directory);
create_files($directory, 3);

echo "\n-- scandir() with mandatory arguments --\n";
var_dump(scandir($directory));

echo "\n-- scandir() with all arguments --\n";
$sorting_order = SCANDIR_SORT_DESCENDING;
$context = stream_context_create();
var_dump(scandir($directory, $sorting_order, $context));

delete_files($directory, 3);
?>
===DONE===
--CLEAN--
<?php
$directory = __DIR__ . '/scandir_basic';
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
  string(9) "file1.tmp"
  [3]=>
  string(9) "file2.tmp"
  [4]=>
  string(9) "file3.tmp"
}

-- scandir() with all arguments --
array(5) {
  [0]=>
  string(9) "file3.tmp"
  [1]=>
  string(9) "file2.tmp"
  [2]=>
  string(9) "file1.tmp"
  [3]=>
  string(2) ".."
  [4]=>
  string(1) "."
}
===DONE===
