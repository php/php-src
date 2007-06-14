--TEST--
Test rename() function: basic functionality
--FILE--
<?php
/* Prototype: bool rename ( string $oldname, string $newname [, resource $context] );
   Description: Renames a file or directory
*/

echo "*** Testing rename() for basic functions on existing file ***\n";
$file_path = dirname(__FILE__);
$src_name = "$file_path/rename_basic.tmp";
$dest_name = "$file_path/rename_basic_new.tmp";

// create the file
$fp = fopen($src_name, "w");
$s1 = stat($src_name);
fclose($fp);

var_dump( rename($src_name, $dest_name) );
// ensure that $dest_name didn't get created
var_dump( file_exists($src_name) );  // expecting false
var_dump( file_exists($dest_name) ); // expecting true

$s2 = stat("$file_path/rename_basic_new.tmp");
// checking statistics of old and renamed file
// both should be same
for ($i = 0; $i <= 12; $i++) {
  if ($s1[$i] != $s2[$i]) {
    echo "rename_basic.tmp and rename_basic_new.tmp stat differ at element $i\n";
  }
}

echo "\n*** Testing rename() on non-existing file ***\n";
// try renaming a non existing file
$src_name = $file_path."/non_existent_file.tmp";
$dest_name = $file_path."/rename_basic_new1.tmp";
var_dump( rename($src_name, $dest_name) );
// ensure that $dest_name didn't get created
var_dump( file_exists($src_name) );  // expecting false
var_dump( file_exists($dest_name) ); // expecting false

// rename a existing dir to new name
echo "\n*** Testing rename() on existing directory ***\n";
$dir_name = $file_path."/rename_basic_dir";
mkdir($dir_name);
$new_dir_name = $file_path."/rename_basic_dir1";
var_dump( rename($dir_name, $new_dir_name) );
//ensure that $new_dir_name got created
var_dump( file_exists($dir_name) );  // expecting false
var_dump( file_exists($new_dir_name) );  // expecting true

// try to rename an non_existing dir 
echo "\n*** Testing rename() on non-existing directory ***\n";
$non_existent_dir_name = $file_path."/non_existent_dir";
$new_dir_name = "$file_path/rename_basic_dir2";
var_dump( rename($non_existent_dir_name, $new_dir_name) );
// ensure that $new_dir_name didn't get created
var_dump( file_exists($non_existent_dir_name) );  // expecting flase
var_dump( file_exists($new_dir_name) );  // expecting false

echo "\n*** Testing rename() by giving stream context as third argument ***\n";
$context = stream_context_create();
// on directory
$dir_name = "$file_path/rename_basic_dir1";
$new_dir_name = "$file_path/rename_basic_dir3";
var_dump( rename($dir_name, $new_dir_name, $context) );
// ensure that $new_dir_name got created
var_dump( file_exists($dir_name) );  // expecting flase
var_dump( file_exists($new_dir_name) ); // expecting true

//on file
$src_name = "$file_path/rename_basic_new.tmp";
$dest_name = "$file_path/rename_basic_new2.tmp";
var_dump( rename($src_name, $dest_name, $context) );
// ensure that $dest_name got created
var_dump( file_exists($src_name) );  // expecting false
var_dump( file_exists($dest_name) );  // expecting true

echo "Done\n";
?>
--CLEAN--
<?php
unlink(dirname(__FILE__)."/rename_basic_new2.tmp");
rmdir(dirname(__FILE__)."/rename_basic_dir3");
?>
--EXPECTF--
*** Testing rename() for basic functions on existing file ***
bool(true)
bool(false)
bool(true)

*** Testing rename() on non-existing file ***

Warning: rename(%s/non_existent_file.tmp,%s/rename_basic_new1.tmp): No such file or directory in %s on line %d
bool(false)
bool(false)
bool(false)

*** Testing rename() on existing directory ***
bool(true)
bool(false)
bool(true)

*** Testing rename() on non-existing directory ***

Warning: rename(%s/non_existent_dir,%s/rename_basic_dir2): No such file or directory in %s on line %d
bool(false)
bool(false)
bool(false)

*** Testing rename() by giving stream context as third argument ***
bool(true)
bool(false)
bool(true)
bool(true)
bool(false)
bool(true)
Done
