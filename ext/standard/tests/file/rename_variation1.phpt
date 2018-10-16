--TEST--
Test rename() function: usage variations-2
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

// rename dirs across directories
echo "\n*** Testing rename() : renaming directory across directories ***\n";
$src_dirs = array (
  /* Testing simple directory tree */
  "$file_path/rename_variation1/",

  /* Testing a dir with trailing slash */
  "$file_path/rename_variation1/",

  /* Testing dir with double trailing slashes */
  "$file_path//rename_variation1//",
);

$dest_dir = "$file_path/rename_variation1_dir";
// create the $dest_dir
mkdir($dest_dir);

$counter = 1;
/* loop through each $src_dirs and rename it to  $dest_dir */
foreach($src_dirs as $src_dir) {
  echo "-- Iteration $counter --\n";

  // create the src dir
  mkdir("$file_path/rename_variation1/");
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
rmdir($file_path."/rename_variation1_dir");
?>
--EXPECT--
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
