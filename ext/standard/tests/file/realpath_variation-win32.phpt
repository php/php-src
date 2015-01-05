--TEST--
Test realpath() function: usage variation
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') {
    die('skip only on Windows');
}
?>
--FILE--
<?php
/* Prototype: string realpath ( string $path );
   Description: Returns canonicalized absolute pathname
*/

require dirname(__FILE__).'/file.inc';

echo "*** Testing realpath(): usage variations ***\n";
$name_prefix = dirname(__FILE__);
$filename = "$name_prefix/realpath_variation/home/tests/realpath_variation.tmp";
mkdir("$name_prefix/realpath_variation/home/tests/", 0777, true);

echo "\n*** Testing realpath() with filename stored inside a object ***\n";
// create a temp file
$file_handle = fopen($filename, "w");
fclose($file_handle);

// creating object with members as filename
class object_temp {
  public $filename;
  function object_temp($file) {
    $this->filename = $file;
  }
}
$obj1 = new object_temp("$name_prefix/realpath_variation/../././realpath_variation/home/tests/realpath_variation.tmp");
$obj2 = new object_temp("$name_prefix/realpath_variation/home/..///realpath_variation.tmp");

var_dump( realpath($obj1->filename) );
var_dump( realpath($obj2->filename) );

echo "\n*** Testing realpath() with filename stored in an array ***\n";
$file_arr = array (
  "$name_prefix////realpath_variation/home/tests/realpath_variation.tmp",
  "$name_prefix/./realpath_variation/home/../home//tests//..//..//..//home//realpath_variation.tmp/"
);

var_dump( realpath($file_arr[0]) ); 
var_dump( realpath($file_arr[1]) );

echo "\n*** Testing realpath() with filename as empty string, NULL and single space ***\n";
$file_string = array (
  /* filename as spaces */
  " ",
  ' ',

  /* empty filename */
  "",
  '',
  NULL,
  null
 );
for($loop_counter = 0; $loop_counter < count($file_string); $loop_counter++) {
  echo "-- Iteration";
  echo $loop_counter + 1;
  echo " --\n";
  var_dump( realpath($file_string[$loop_counter]) ); 
}

echo "Done\n";
?>
--CLEAN--
<?php
$name_prefix = dirname(__FILE__)."/realpath_variation";
unlink("$name_prefix/home/tests/realpath_variation.tmp");
rmdir("$name_prefix/home/tests/");
rmdir("$name_prefix/home/");
rmdir("$name_prefix/");
?>
--EXPECTF--
*** Testing realpath(): usage variations ***

*** Testing realpath() with filename stored inside a object ***
string(%d) "%s\realpath_variation\home\tests\realpath_variation.tmp"
bool(false)

*** Testing realpath() with filename stored in an array ***
string(%d) "%s\realpath_variation\home\tests\realpath_variation.tmp"
bool(false)

*** Testing realpath() with filename as empty string, NULL and single space ***
-- Iteration1 --
bool(false)
-- Iteration2 --
bool(false)
-- Iteration3 --
string(%d) "%s"
-- Iteration4 --
string(%d) "%s"
-- Iteration5 --
string(%d) "%s"
-- Iteration6 --
string(%d) "%s"
Done
