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
Done

