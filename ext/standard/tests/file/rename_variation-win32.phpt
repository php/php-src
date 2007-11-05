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
mkdir("$file_path/rename_variation/");

/* Testing rename() function on files */
echo "*** Testing variations of rename() on files ***\n";
$files_arr = array(
  "$file_path/rename_variation/rename_variation.tmp",

  /* Testing file with double slashes */
  "$file_path/rename_variation//rename_variation.tmp",
  "$file_path/rename_variation/r*.tmp",

  /* Testing Binary safe */
  "$file_path/rename_variation/rename_variationx000.tmp"
);
$counter = 1;
/* loop through each $file and rename it to rename_variation2.tmp */
foreach($files_arr as $file) {
  $fp = fopen("$file_path/rename_variation/rename_variation.tmp", "w");
  fclose($fp);
  echo "-- Iteration $counter --\n";
  var_dump( rename($file, "$file_path/rename_variation2.tmp") );
  $counter++;
  unlink("$file_path/rename_variation2.tmp");
}
unlink("$file_path/rename_variation/rename_variation.tmp");
rmdir("$file_path/rename_variation/");  // deleting temp directory

/* Testing rename() function on directories */
echo "\n*** Testing variations of rename() on directories ***\n";
$directories = array (
  /* Testing simple directory tree */
  "$file_path/rename_variation/",

  /* Testing a dir with trailing slash */
  "$file_path/rename_variation/",

  /* Testing dir with double trailing slashes */
  "$file_path/rename_variation//",

  /* Testing Binary safe */
  "$file_path/rename_variationx000/",

  /* Testing current directory */
  ".",

  /* Dir name as empty string */
  "",
  '',

  /* Dir name as string with a space */
  " ",
  ' '
);
$counter = 1;
/* loop through each $dir and rename it to rename_variation1 */
foreach($directories as $dir) {
  mkdir("$file_path/rename_variation/");
  echo "-- Iteration $counter --\n";
  var_dump( rename($dir, "$file_path/rename_variation1/") );
  $counter++;
  rmdir("$file_path/rename_variation1/");
}

/* Testing rename() on non-existing file and directory as first argument */
echo "\n*** Testing rename() with non-existing file and directory ***\n";
// renaming a non-existing file to existing file
var_dump( rename(dirname(__FILE__)."/rename_variation123.tmp", __FILE__) );
// renaming a non-existing directory to existing directory
var_dump( rename(dirname(__FILE__)."/rename_variation123/", dirname(__FILE__)) );

/* Renaming a file and directory to numeric name */
echo "\n*** Testing rename() by renaming a file and directory to numeric name ***\n";
$fp = fopen(dirname(__FILE__)."/rename_variation.tmp", "w");
fclose($fp);
// renaming existing file to numeric name
var_dump( rename(dirname(__FILE__)."/rename_variation.tmp", dirname(__FILE__)."/12345.tmp") );
unlink(dirname(__FILE__)."/12345.tmp");
// renaming existing directory to numeric name
var_dump( rename(dirname(__FILE__)."/rename_variation/", dirname(__FILE__)."/12345/") );

echo "\n*** Testing rename() with miscelleneous input ***\n";
$file_path = dirname(__FILE__);
mkdir("$file_path/rename_variation");
$fp = fopen("$file_path/rename_variation.tmp", "w");
fclose($fp);

echo "\n-- Renaming file to same file name --\n";
var_dump( rename("$file_path/rename_variation.tmp", "$file_path/rename_variation.tmp") );

echo "\n-- Renaming directory to same directory name --\n";
var_dump( rename("$file_path/rename_variation/", "$file_path/rename_variation/") );

echo "\n-- Renaming existing file to directory name --\n";
var_dump( rename("$file_path/rename_variation.tmp", "$file_path/rename_variation/") );

echo "\n-- Renaming existing directory to file name --\n";
var_dump( rename("$file_path/rename_variation", "$file_path/rename_variation.tmp") );

echo "Done\n";
?>
--CLEAN--
<?php
unlink(dirname(__FILE__)."/12345.tmp");
unlink(dirname(__FILE__)."/rename_variation.tmp");
rmdir(dirname(__FILE__)."/rename_variation.tmp/");
rmdir(dirname(__FILE__)."/rename_variation/");
rmdir(dirname(__FILE__)."/12345/");
?>
--EXPECTF--
*** Testing variations of rename() on files ***
-- Iteration 1 --
bool(true)
-- Iteration 2 --
bool(true)
-- Iteration 3 --
bool(true)
-- Iteration 4 --

Warning: rename(%s,%s): No such file or directory in %s on line %d
bool(false)

Warning: unlink(%s): No such file or directory in %s on line %d

*** Testing variations of rename() on directories ***
-- Iteration 1 --
bool(true)
-- Iteration 2 --
bool(true)
-- Iteration 3 --
bool(true)
-- Iteration 4 --

Warning: rename(%s,%s): No such file or directory in %s on line %d
bool(false)

Warning: rmdir(%s): No such file or directory in %s on line %d
-- Iteration 5 --

Warning: rename(.,%s): Permission denied in %s on line %d
bool(false)

Warning: rmdir(%s): No such file or directory in %s on line %d
-- Iteration 6 --

Warning: rename(,%s): Permission denied in %s on line %d
bool(false)

Warning: rmdir(%s): No such file or directory in %s on line %d
-- Iteration 7 --

Warning: rename(,%s): Permission denied in %s on line %d
bool(false)

Warning: rmdir(%s): No such file or directory in %s on line %d
-- Iteration 8 --

Warning: rename( ,%s): File exists in %s on line %d
bool(false)

Warning: rmdir(%s): No such file or directory in %s on line %d
-- Iteration 9 --

Warning: rename( ,%s): File exists in %s on line %d
bool(false)

Warning: rmdir(%s): No such file or directory in %s on line %d

*** Testing rename() with non-existing file and directory ***

Warning: rename(%s,%s): No such file or directory in %s on line %d
bool(false)

Warning: rename(%s,%s): No such file or directory in %s on line %d
bool(false)

*** Testing rename() by renaming a file and directory to numeric name ***
bool(true)
bool(true)

*** Testing rename() with miscelleneous input ***

-- Renaming file to same file name --
bool(true)

-- Renaming directory to same directory name --
bool(true)

-- Renaming existing file to directory name --

Warning: rename(%s,%s): File exists in %s on line %d
bool(false)

-- Renaming existing directory to file name --

Warning: rename(%s,%s): File exists in %s on line %d
bool(false)
Done
