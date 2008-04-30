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

$file_path = dirname(__FILE__);
mkdir("$file_path/rename_variation_dir");

/* Renaming a file and directory to numeric name */
echo "\n*** Testing rename() by renaming a file and directory to numeric name ***\n";
$fp = fopen($file_path."/rename_variation.tmp", "w");
fclose($fp);

// renaming existing file to numeric name
var_dump( rename($file_path."/rename_variation.tmp", $file_path."/12345") );

// ensure that rename worked fine
var_dump( file_exists($file_path."/rename_variation.tmp" ) );  // expecting false
var_dump( file_exists($file_path."/12345" ) );  // expecting true

unlink($file_path."/12345");

// renaming a directory to numeric name
var_dump( rename($file_path."/rename_variation_dir/", $file_path."/12345") );

// ensure that rename worked fine
var_dump( file_exists($file_path."/rename_variation_dir" ) );  // expecting false
var_dump( file_exists($file_path."/12345" ) );  // expecting true

rmdir($file_path."/12345");

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
*** Testing rename() by renaming a file and directory to numeric name ***
bool(true)
bool(false)
bool(true)
bool(true)
bool(false)
bool(true)
Done

