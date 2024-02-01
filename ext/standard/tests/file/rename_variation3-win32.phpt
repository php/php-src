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
require __DIR__.'/file.inc';

/* creating directory */
$file_path = __DIR__;
$dirname = "$file_path/rename_variation3_dir";
mkdir($dirname);

/* test rename() by trying to rename an existing file/dir to the same name
  and one another */

$filename = "$file_path/rename_variation3.tmp";
$fp = fopen($filename, "w");
if (!$fp) {
    die("Cannot create $filename\n");
}
fclose($fp);

echo "\n-- Renaming file to same file name --\n";
var_dump( rename($filename, $filename) );
var_dump( file_exists($filename) );

echo "\n-- Renaming directory to same directory name --\n";
var_dump( rename($dirname, $dirname) );
var_dump( file_exists($dirname) );

echo "\n-- Renaming existing file to existing directory name --\n";
var_dump( rename($filename, $dirname) );
var_dump( file_exists($filename) );
var_dump( file_exists($dirname) );

echo "\n-- Renaming existing directory to existing file name --\n";
$fp = fopen($filename, "w");
fclose($fp);

var_dump( rename($dirname, $filename) );
var_dump( file_exists($filename) );
var_dump( file_exists($dirname) );

echo "Done\n";
?>
--CLEAN--
<?php
$file_path = __DIR__;
rmdir($file_path."/rename_variation3.tmp");
?>
--EXPECTF--
-- Renaming file to same file name --
bool(true)
bool(true)

-- Renaming directory to same directory name --
bool(true)
bool(true)

-- Renaming existing file to existing directory name --

Warning: rename(%s/rename_variation3.tmp,%s/rename_variation3_dir): Access is denied (code: 5) in %s on line %d
bool(false)
bool(true)
bool(true)

-- Renaming existing directory to existing file name --
bool(true)
bool(true)
bool(false)
Done
