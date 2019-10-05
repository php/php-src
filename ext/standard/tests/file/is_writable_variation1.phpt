--TEST--
Test is_writable() and its alias is_writeable() function: usage variations - diff. path notations
--SKIPIF--
<?php
require __DIR__ . '/../skipif_root.inc';
?>
--FILE--
<?php
/* Prototype: bool is_writable ( string $filename );
   Description: Tells whether the filename is writable.

   is_writeable() is an alias of is_writable()
*/
/* test is_writable() & is_writeable() with file having different filepath notation */
require __DIR__.'/file.inc';
echo "*** Testing is_writable(): usage variations ***\n";

$file_path = __DIR__;
mkdir("$file_path/is_writable_variation1");

// create a new temporary file
$fp = fopen("$file_path/is_writable_variation1/bar.tmp", "w");
fclose($fp);

/* array of files to be tested to check if they are writable
   using is_writable() function */
$files_arr = array(
  "$file_path/is_writable_variation1/bar.tmp",

  /* Testing a file trailing slash */
  "$file_path/is_writable_variation1/bar.tmp/",

  /* Testing file with double slashes */
  "$file_path/is_writable_variation1//bar.tmp",
  "$file_path//is_writable_variation1//bar.tmp",
  "$file_path/is_writable_variation1/*.tmp",
  "$file_path/is_writable_variation1/b*.tmp",

  /* Testing Binary safe */
  "$file_path/is_writable_variation1".chr(0)."bar.tmp",
  "$file_path".chr(0)."is_writable_variation1/bar.tmp",
  "$file_path".chr(0)."is_writable_variation1/bar.tmp",

  /* Testing directories */
  ".",  // current directory, exp: bool(true)
  "$file_path/is_writable_variation1"  // temp directory, exp: bool(true)
);
$counter = 1;
/* loop through to test each element in the above array
   is a writable file */
foreach($files_arr as $file) {
  echo "-- Iteration $counter --\n";
  var_dump( is_writable($file) );
  var_dump( is_writeable($file) );
  $counter++;
  clearstatcache();
}

echo "Done\n";
?>
--CLEAN--
<?php
unlink(__DIR__."/is_writable_variation1/bar.tmp");
rmdir(__DIR__."/is_writable_variation1/");
?>
--EXPECTF--
*** Testing is_writable(): usage variations ***
-- Iteration 1 --
bool(true)
bool(true)
-- Iteration 2 --
bool(%s)
bool(%s)
-- Iteration 3 --
bool(true)
bool(true)
-- Iteration 4 --
bool(true)
bool(true)
-- Iteration 5 --
bool(false)
bool(false)
-- Iteration 6 --
bool(false)
bool(false)
-- Iteration 7 --

Warning: is_writable() expects parameter 1 to be a valid path, string given in %s on line %d
NULL

Warning: is_writeable() expects parameter 1 to be a valid path, string given in %s on line %d
NULL
-- Iteration 8 --

Warning: is_writable() expects parameter 1 to be a valid path, string given in %s on line %d
NULL

Warning: is_writeable() expects parameter 1 to be a valid path, string given in %s on line %d
NULL
-- Iteration 9 --

Warning: is_writable() expects parameter 1 to be a valid path, string given in %s on line %d
NULL

Warning: is_writeable() expects parameter 1 to be a valid path, string given in %s on line %d
NULL
-- Iteration 10 --
bool(true)
bool(true)
-- Iteration 11 --
bool(true)
bool(true)
Done
