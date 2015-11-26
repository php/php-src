--TEST--
Test rename() function: usage variations-1 (Bug#42638)
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip.. only for Linux');
}
?>
--FILE--
<?php

/* creating directory */
$file_path = dirname(__FILE__);
mkdir("$file_path/rename_variation");

/* rename files across directories */
echo "*** Testing rename() : rename files across directories ***\n";
$src_filenames = array(
  "$file_path/rename_variation/rename_variation.tmp",

  /* Testing a file trailing slash */
  "$file_path/rename_variation/rename_variation.tmp/",

  /* Testing file with double slashes */
  "$file_path/rename_variation//rename_variation.tmp",
  "$file_path//rename_variation//rename_variation.tmp",
);
$counter = 1;
/* loop through each $file and rename it to rename_variation2.tmp */
foreach($src_filenames as $src_filename) {
  echo "-- Iteration $counter --\n";
  $fp = fopen("$file_path/rename_variation/rename_variation.tmp", "w");
  fclose($fp);
  $dest_filename = "$file_path/rename_variation2.tmp";
  var_dump( rename($src_filename, $dest_filename) );
  // ensure that file got renamed to new name 
  var_dump( file_exists($src_filename) );  // expecting false
  var_dump( file_exists($dest_filename) );  // expecting true
  $counter++;
 
  // unlink the file  
  unlink($dest_filename);
}

// clean the temp dir and file
rmdir("$file_path/rename_variation"); 

echo "Done\n";
?>
--EXPECTF--
*** Testing rename() : rename files across directories ***
-- Iteration 1 --
bool(true)
bool(false)
bool(true)
-- Iteration 2 --

Warning: rename(%s,%s): Not a directory in %s on line %d
bool(false)
bool(false)
bool(false)

Warning: unlink(%s): No such file or directory in %s on line %d
-- Iteration 3 --
bool(true)
bool(false)
bool(true)
-- Iteration 4 --
bool(true)
bool(false)
bool(true)
Done
