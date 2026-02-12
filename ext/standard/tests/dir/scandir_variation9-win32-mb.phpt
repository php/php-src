--TEST--
Test scandir() function : usage variations - different ints as $sorting_order arg
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') {
  die("skip Valid only on Windows");
}
?>
--FILE--
<?php
/*
 * Pass different integers as $sorting_order argument to test how scandir()
 * re-orders the array
 */

echo "*** Testing scandir() : usage variations ***\n";

// include for create_files/delete_files functions
include(__DIR__ . '/../file/file.inc');

// create directory and files
$dir = __DIR__ . '/私はガラスを食べられますscandir_variation9';
mkdir($dir);
@create_files($dir, 2, "numeric", 0755, 1, "w", "私はガラスを食べられますfile");

// different ints to pass as $sorting_order argument
$ints = array (PHP_INT_MAX, -PHP_INT_MAX, 0);

foreach($ints as $sorting_order) {
  try {
    var_dump( scandir($dir, $sorting_order) );
  } catch (ValueError $e) {
    echo $e->getMessage() . PHP_EOL;
  }
}

delete_files($dir, 2, "私はガラスを食べられますfile");
?>
--CLEAN--
<?php
$dir = __DIR__ . '/私はガラスを食べられますscandir_variation9';
rmdir($dir);
?>
--EXPECT--
*** Testing scandir() : usage variations ***
scandir(): Argument #2 ($sorting_order) must be one of PHP_SCANDIR_SORT_ASCENDING, or SCANDIR_SORT_NONE
scandir(): Argument #2 ($sorting_order) must be one of PHP_SCANDIR_SORT_ASCENDING, or SCANDIR_SORT_NONE
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
