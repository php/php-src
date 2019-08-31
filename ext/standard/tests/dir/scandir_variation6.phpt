--TEST--
Test scandir() function : usage variations - Wildcards in directory path
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip.. Not valid for Windows');
}
?>
--FILE--
<?php
/* Prototype  : array scandir(string $dir [, int $sorting_order [, resource $context]])
 * Description: List files & directories inside the specified path
 * Source code: ext/standard/dir.c
 */

/*
 * Pass a directory path using wildcards as $dir argument to test how scandir() behaves
 */

echo "*** Testing scandir() : usage variations ***\n";

// create the temporary directories
$file_path = __DIR__;
$dir_path = $file_path . "/scandir_variation6";
$sub_dir_path = $dir_path . "/sub_dir1";

mkdir($dir_path);
mkdir($sub_dir_path);

// with different wildcard characters

echo "\n-- Wildcard = '*' --\n";
var_dump( scandir($file_path . "/scandir_var*") );
var_dump( scandir($file_path . "/*") );

echo "\n-- Wildcard = '?' --\n";
var_dump( scandir($dir_path . "/sub_dir?") );
var_dump( scandir($dir_path . "/sub?dir1") );

?>
===DONE===
--CLEAN--
<?php
$dir_path = __DIR__ . "/scandir_variation6";
$sub_dir_path = $dir_path . "/sub_dir1";

rmdir($sub_dir_path);
rmdir($dir_path);
?>
--EXPECTF--
*** Testing scandir() : usage variations ***

-- Wildcard = '*' --

Warning: scandir(%s/scandir_var*): failed to open dir: %s in %s on line %d

Warning: scandir(): (errno %d): %s in %s on line %d
bool(false)

Warning: scandir(%s/*): failed to open dir: %s in %s on line %d

Warning: scandir(): (errno %d): %s in %s on line %d
bool(false)

-- Wildcard = '?' --

Warning: scandir(%s/scandir_variation6/sub_dir?): failed to open dir: %s in %s on line %d

Warning: scandir(): (errno %d): %s in %s on line %d
bool(false)

Warning: scandir(%s/scandir_variation6/sub?dir1): failed to open dir: %s in %s on line %d

Warning: scandir(): (errno %d): %s in %s on line %d
bool(false)
===DONE===
