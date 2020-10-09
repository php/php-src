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
require __DIR__.'/file.inc';

echo "*** Testing realpath(): usage variations ***\n";
$name_prefix = __DIR__;
$filename = "$name_prefix/realpath_variation_私はガラスを食べられます/home/tests/realpath_variation_私はガラスを食べられます.tmp";
mkdir("$name_prefix/realpath_variation_私はガラスを食べられます/home/tests/", 0777, true);

echo "\n*** Testing realpath() with filename stored inside a object ***\n";
// create a temp file
$file_handle = fopen($filename, "w");
fclose($file_handle);

// creating object with members as filename
class object_temp {
  public $filename;
  function __construct($file) {
    $this->filename = $file;
  }
}
$obj1 = new object_temp("$name_prefix/realpath_variation_私はガラスを食べられます/../././realpath_variation_私はガラスを食べられます/home/tests/realpath_variation_私はガラスを食べられます.tmp");
$obj2 = new object_temp("$name_prefix/realpath_variation_私はガラスを食べられます/home/..///realpath_variation_私はガラスを食べられます.tmp");

var_dump( realpath($obj1->filename) );
var_dump( realpath($obj2->filename) );

echo "\n*** Testing realpath() with filename stored in an array ***\n";
$file_arr = array (
  "$name_prefix////realpath_variation_私はガラスを食べられます/home/tests/realpath_variation_私はガラスを食べられます.tmp",
  "$name_prefix/./realpath_variation_私はガラスを食べられます/home/../home//tests//..//..//..//home//realpath_variation_私はガラスを食べられます.tmp/"
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
$name_prefix = __DIR__."/realpath_variation_私はガラスを食べられます";
unlink("$name_prefix/home/tests/realpath_variation_私はガラスを食べられます.tmp");
rmdir("$name_prefix/home/tests/");
rmdir("$name_prefix/home/");
rmdir("$name_prefix/");
?>
--EXPECTF--
*** Testing realpath(): usage variations ***

*** Testing realpath() with filename stored inside a object ***
string(%d) "%s\realpath_variation_私はガラスを食べられます\home\tests\realpath_variation_私はガラスを食べられます.tmp"
bool(false)

*** Testing realpath() with filename stored in an array ***
string(%d) "%s\realpath_variation_私はガラスを食べられます\home\tests\realpath_variation_私はガラスを食べられます.tmp"
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
