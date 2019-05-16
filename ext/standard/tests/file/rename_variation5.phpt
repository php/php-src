--TEST--
Test rename() function: usage variations-6
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip.. only for Linux');
}
?>
--FILE--
<?php

/* test rename() by trying to rename an existing file/dir/link to the same name
  and one another */
// create a dir
$file_path = __DIR__;
$dirname = "$file_path/rename_variation5_dir";
mkdir($dirname);
//create a file
$filename = "$file_path/rename_variation5.tmp";
$fp = fopen($filename, "w");
fclose($fp);
// create a link
$linkname = "$file_path/rename_variation5_link.tmp";
symlink($filename, $linkname);

echo "\n-- Renaming link to same link name --\n";
var_dump( rename($linkname, $linkname) );

echo "\n-- Renaming file to same file name --\n";
var_dump( rename($filename, $filename) );

echo "\n-- Renaming directory to same directory name --\n";
var_dump( rename($dirname, $dirname) );

echo "\n-- Renaming existing link to existing directory name --\n";
var_dump( rename($linkname, $dirname) );
echo "\n-- Renaming existing link to existing file name --\n";
var_dump( rename($linkname, $filename) );

echo "\n-- Renaming existing file to existing directory name --\n";
var_dump( rename($filename, $dirname) );
echo "\n-- Renaming existing file to existing link name --\n";
var_dump( rename($filename, $linkname) );

echo "\n-- Renaming existing directory to existing file name --\n";
$fp = fopen($filename, "w");
fclose($fp);
var_dump( rename($dirname, $filename) );
echo "\n-- Renaming existing directory to existing link name --\n";
var_dump( rename($dirname, $linkname) );

echo "Done\n";
?>
--CLEAN--
<?php
$file_path = __DIR__;
unlink($file_path."/rename_variation5_link.tmp");
unlink($file_path."/rename_variation5.tmp");
rmdir($file_path."/rename_variation5_dir");
?>
--EXPECTF--
-- Renaming link to same link name --
bool(true)

-- Renaming file to same file name --
bool(true)

-- Renaming directory to same directory name --
bool(true)

-- Renaming existing link to existing directory name --

Warning: rename(%s,%s): Is a directory in %s on line %d
bool(false)

-- Renaming existing link to existing file name --
bool(true)

-- Renaming existing file to existing directory name --

Warning: rename(%s,%s): Is a directory in %s on line %d
bool(false)

-- Renaming existing file to existing link name --
bool(true)

-- Renaming existing directory to existing file name --

Warning: rename(%s,%s): Not a directory in %s on line %d
bool(false)

-- Renaming existing directory to existing link name --

Warning: rename(%s,%s): Not a directory in %s on line %d
bool(false)
Done
