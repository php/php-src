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

$file_path = __DIR__;
mkdir("$file_path/rename_variation2_dir");

/* Renaming a file and directory to numeric name */
echo "\n*** Testing rename() by renaming a file and directory to numeric name ***\n";
$fp = fopen($file_path."/rename_variation2.tmp", "w");
fclose($fp);

// renaming existing file to numeric name
var_dump( rename($file_path."/rename_variation2.tmp", $file_path."/12345") );

// ensure that rename worked fine
var_dump( file_exists($file_path."/rename_variation2.tmp" ) );  // expecting false
var_dump( file_exists($file_path."/12345" ) );  // expecting true

unlink($file_path."/12345");

// renaming a directory to numeric name
var_dump( rename($file_path."/rename_variation2_dir/", $file_path."/12345") );

// ensure that rename worked fine
var_dump( file_exists($file_path."/rename_variation2_dir" ) );  // expecting false
var_dump( file_exists($file_path."/12345" ) );  // expecting true

rmdir($file_path."/12345");

echo "Done\n";
?>
--CLEAN--
<?php
$file_path = __DIR__;
unlink($file_path."/rename_variation2_link.tmp");
unlink($file_path."/rename_variation2.tmp");
rmdir($file_path."/rename_variation2_dir");
?>
--EXPECT--
*** Testing rename() by renaming a file and directory to numeric name ***
bool(true)
bool(false)
bool(true)
bool(true)
bool(false)
bool(true)
Done
