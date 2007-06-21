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

require dirname(__FILE__).'/file.inc';

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

// rename dirs across directories
echo "\n*** Testing rename() : renaming directory across directories ***\n";
$src_dirs = array (
  /* Testing simple directory tree */
  "$file_path/rename_variation/",

  /* Testing a dir with trailing slash */
  "$file_path/rename_variation/",

  /* Testing dir with double trailing slashes */
  "$file_path//rename_variation//",
);

$dest_dir = "$file_path/rename_variation_dir";
// create the $dest_dir
mkdir($dest_dir);

$counter = 1;
/* loop through each $src_dirs and rename it to  $dest_dir */
foreach($src_dirs as $src_dir) {
  echo "-- Iteration $counter --\n";

  // create the src dir
  mkdir("$file_path/rename_variation/");
  // rename the src dir to a new dir in dest dir
  var_dump( rename($src_dir, $dest_dir."/new_dir") );
  // ensure that dir was renamed 
  var_dump( file_exists($src_dir) );  // expecting false
  var_dump( file_exists($dest_dir."/new_dir") ); // expecting true

  // remove the new dir
  rmdir($dest_dir."/new_dir");
  $counter++;
}

/* Renaming a file and directory to numeric name */
echo "\n*** Testing rename() by renaming a file and directory to numeric name ***\n";
$fp = fopen($file_path."/rename_variation.tmp", "w");
fclose($fp);
// renaming existing file to numeric name
var_dump( rename($file_path."/rename_variation.tmp", $file_path."/12345") );
// ensure that rename worked fine
var_dump( file_exists($file_path."/rename_variation.tmp" ) );  // expecting false
var_dump( file_exists($file_path."/12345" ) );  // expecting true
// remove the file
unlink($file_path."/12345");

// renaming a directory to numeric name
var_dump( rename($file_path."/rename_variation_dir/", $file_path."/12345") );
// ensure that rename worked fine
var_dump( file_exists($file_path."/rename_variation_dir" ) );  // expecting false
var_dump( file_exists($file_path."/12345" ) );  // expecting true

// delete the file and dir
rmdir($file_path."/12345");

/* test rename() by trying to rename an existing file/dir to the same name
  and one another */
// create a dir 
$file_path = dirname(__FILE__);
$dirname = "$file_path/rename_variation_dir"; 
mkdir($dirname);
//create a file
$filename = "$file_path/rename_variation.tmp"; 
$fp = fopen($filename, "w");
fclose($fp);

echo "\n-- Renaming file to same file name --\n";
var_dump( rename($filename, $filename) );

echo "\n-- Renaming directory to same directory name --\n";
var_dump( rename($dirname, $dirname) );

echo "\n-- Renaming existing file to existing directory name --\n";
var_dump( rename($filename, $dirname) );

echo "\n-- Renaming existing directory to existing file name --\n";
$fp = fopen($filename, "w");
fclose($fp);
var_dump( rename($dirname, $filename) );

echo "Done\n";
?>
--CLEAN--
<?php
$file_path = dirname(__FILE__);
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
bool(true)
bool(false)
bool(true)
-- Iteration 3 --
bool(true)
bool(false)
bool(true)
-- Iteration 4 --
bool(true)
bool(false)
bool(true)

*** Testing rename() : renaming directory across directories ***
-- Iteration 1 --
bool(true)
bool(false)
bool(true)
-- Iteration 2 --
bool(true)
bool(false)
bool(true)
-- Iteration 3 --
bool(true)
bool(false)
bool(true)

*** Testing rename() by renaming a file and directory to numeric name ***
bool(true)
bool(false)
bool(true)
bool(true)
bool(false)
bool(true)

-- Renaming file to same file name --
bool(true)

-- Renaming directory to same directory name --
bool(true)

-- Renaming existing file to existing directory name --

Warning: rename(%s,%s): File exists in %s on line %d
bool(false)

-- Renaming existing directory to existing file name --

Warning: rename(%s,%s): File exists in %s on line %d
bool(false)
Done