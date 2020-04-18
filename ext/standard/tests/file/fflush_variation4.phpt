--TEST--
Test fflush() function: usage variations - file opened in read-only mode
--FILE--
<?php
/*  Prototype: bool fflush ( resource $handle );
    Description: Flushes the output to a file
*/

/* test fflush() with handle to a file opened in read-only mode as resource */

$file_path = __DIR__;
require $file_path.'/file.inc';

echo "*** Testing fflush(): with file handles of files opened in various read modes ***\n";
$file_modes = array("r", "rb", "rt");

$file_name = "$file_path/fflush_variation4.tmp";

$count = 1;

foreach( $file_modes as $mode ) {
  echo "-- Iteration $count with file opened in $mode mode --\n";

  // creating a file
  $file_handle = fopen($file_name, "w");
  if($file_handle == false)
    exit("Error:failed to open file $file_name");
  fclose($file_handle);

  // opening the file in different read modes
  $file_handle = fopen($file_name, $mode);
  if($file_handle == false)
    exit("Error:failed to open file $file_name");
  var_dump( fflush($file_handle) );
  fclose($file_handle);

  unlink($file_name);
  $count++;
}

echo "\n*** Done ***";
?>
--EXPECT--
*** Testing fflush(): with file handles of files opened in various read modes ***
-- Iteration 1 with file opened in r mode --
bool(true)
-- Iteration 2 with file opened in rb mode --
bool(true)
-- Iteration 3 with file opened in rt mode --
bool(true)

*** Done ***
