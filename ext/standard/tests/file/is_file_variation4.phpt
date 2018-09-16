--TEST--
Test is_file() function: usage variations - diff. path notations (Bug #42027)
--FILE--
<?php
/* Prototype: bool is_file ( string $filename );
   Description: Tells whether the filename is a regular file
     Returns TRUE if the filename exists and is a regular file
*/

/* Passing file names with different notations, using slashes, wild-card chars */

$file_path = dirname(__FILE__);

echo "*** Testing is_file() with different notations of file names ***\n";
$dir_name = $file_path."/is_file_variation4";
mkdir($dir_name);
$file_handle = fopen($dir_name."/is_file_variation4.tmp", "w");
fclose($file_handle);

$files_arr = array(
  "/is_file_variation4/is_file_variation4.tmp",

  /* Testing a file trailing slash */
  "/is_file_variation4/is_file_variation4.tmp/",

  /* Testing file with double slashes */
  "/is_file_variation4//is_file_variation4.tmp",
  "//is_file_variation4//is_file_variation4.tmp",
  "/is_file_variation4/*.tmp",
  "is_file_variation4/is_file*.tmp",

  /* Testing Binary safe */
  "/is_file_variation4/is_file_variation4.tmp".chr(0),
  "/is_file_variation4/is_file_variation4.tmp\0"
);

$count = 1;
/* loop through to test each element in the above array */
foreach($files_arr as $file) {
  echo "- Iteration $count -\n";
  var_dump( is_file( $file_path."/".$file ) );
  clearstatcache();
  $count++;
}

echo "\n*** Done ***";
?>
--CLEAN--
<?php
$file_path = dirname(__FILE__);
$dir_name = $file_path."/is_file_variation4";
unlink($dir_name."/is_file_variation4.tmp");
rmdir($dir_name);
?>
--EXPECTF--
*** Testing is_file() with different notations of file names ***
- Iteration 1 -
bool(true)
- Iteration 2 -
bool(false)
- Iteration 3 -
bool(true)
- Iteration 4 -
bool(true)
- Iteration 5 -
bool(false)
- Iteration 6 -
bool(false)
- Iteration 7 -

Warning: is_file() expects parameter 1 to be a valid path, string given in %s on line %d
NULL
- Iteration 8 -

Warning: is_file() expects parameter 1 to be a valid path, string given in %s on line %d
NULL

*** Done ***
