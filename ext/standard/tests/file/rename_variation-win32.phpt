--TEST--
Test rename() function: usage variations
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') {
    die('skip.. only for Windows');
}
?>
--FILE--
<?php
/* Prototype: bool rename ( string $oldname, string $newname [, resource $context] );
   Description: Renames a file or directory
*/

require __DIR__.'/file.inc';

/* create directory */
$file_path = __DIR__;
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

rmdir("$file_path/rename_variation");

echo "Done\n";
?>
--CLEAN--
<?php
$file_path = __DIR__;
unlink($file_path."/rename_variation_link.tmp");
unlink($file_path."/rename_variation.tmp");
rmdir($file_path."/rename_variation_dir");
?>
--EXPECTF--
*** Testing rename() : rename files across directories ***
-- Iteration 1 --
bool(true)
bool(false)
bool(true)
-- Iteration 2 --

Warning: rename(%s/rename_variation/rename_variation.tmp/,%s/rename_variation2.tmp): The filename, directory name, or volume label syntax is incorrect. (code: 123) in %s on line %d
bool(false)
bool(false)
bool(false)

Warning: unlink(%s/rename_variation2.tmp): No such file or directory in %s on line %d
-- Iteration 3 --
bool(true)
bool(false)
bool(true)
-- Iteration 4 --
bool(true)
bool(false)
bool(true)
Done
