--TEST--
Test is_dir() function: usage variations - dir/subdir
--FILE--
<?php
/* Prototype: bool is_dir ( string $dirname );
   Description: Tells whether the dirname is a directory
     Returns TRUE if the dirname exists and is a directory, FALSE  otherwise.
*/

/* Testing is_dir() with base and sub dirs */

$file_path = dirname(__FILE__);

echo "-- Testing is_dir() with an empty dir --\n";
$dirname = $file_path."/is_dir_variation1";
mkdir($dirname);
var_dump( is_dir($dirname) );
clearstatcache();

echo "-- Testing is_dir() with a subdir in base dir --\n";
$subdirname = $dirname."/is_dir_variation1_sub";
mkdir($subdirname);
var_dump( is_dir($subdirname) );
var_dump( is_dir($dirname) );

echo "\n*** Done ***";
?>
--CLEAN--
<?php
$file_path = dirname(__FILE__);
$dir_name = $file_path."/is_dir_variation1";
rmdir($dir_name."/is_dir_variation1_sub");
rmdir($dir_name);
?>
--EXPECT--
-- Testing is_dir() with an empty dir --
bool(true)
-- Testing is_dir() with a subdir in base dir --
bool(true)
bool(true)

*** Done ***
