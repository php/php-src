--TEST--
Test scandir() function : usage variations - different ints as $sorting_order arg
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
$dir = __DIR__ . '/scandir_variation9';
mkdir($dir);
@create_files($dir, 2);

// different ints to pass as $sorting_order argument
$ints = array (PHP_INT_MAX, -PHP_INT_MAX, 0);

foreach($ints as $sorting_order) {
    var_dump( scandir($dir, $sorting_order) );
}

delete_files($dir, 2);
?>
--CLEAN--
<?php
$dir = __DIR__ . '/scandir_variation9';
rmdir($dir);
?>
--EXPECT--
*** Testing scandir() : usage variations ***
array(4) {
  [0]=>
  string(9) "file2.tmp"
  [1]=>
  string(9) "file1.tmp"
  [2]=>
  string(2) ".."
  [3]=>
  string(1) "."
}
array(4) {
  [0]=>
  string(9) "file2.tmp"
  [1]=>
  string(9) "file1.tmp"
  [2]=>
  string(2) ".."
  [3]=>
  string(1) "."
}
array(4) {
  [0]=>
  string(1) "."
  [1]=>
  string(2) ".."
  [2]=>
  string(9) "file1.tmp"
  [3]=>
  string(9) "file2.tmp"
}
