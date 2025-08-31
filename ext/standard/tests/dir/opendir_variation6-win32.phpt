--TEST--
Test opendir() function : usage variations - Different wildcards
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') {
  die("skip Valid only on Windows");
}
?>
--FILE--
<?php
/*
 * Pass paths containing wildcards to test if opendir() recognises them
 */

echo "*** Testing opendir() : usage variations ***\n";
// create the temporary directories
$file_path = __DIR__;
$dir_path = $file_path . "/opendir_variation6";
$sub_dir_path = $dir_path . "/sub_dir1";

mkdir($dir_path);
mkdir($sub_dir_path);

// with different wildcard characters

echo "\n-- Wildcard = '*' --\n";
var_dump( opendir($file_path . "/opendir_var*") );
var_dump( opendir($file_path . "/*") );

echo "\n-- Wildcard = '?' --\n";
var_dump( opendir($dir_path . "/sub_dir?") );
var_dump( opendir($dir_path . "/sub?dir1") );

?>
--CLEAN--
<?php
$dir_path = __DIR__ . "/opendir_variation6";
$sub_dir_path = $dir_path . "/sub_dir1";

rmdir($sub_dir_path);
rmdir($dir_path);
?>
--EXPECTF--
*** Testing opendir() : usage variations ***

-- Wildcard = '*' --

Warning: opendir(%s/opendir_var*): %s in %s on line %d

Warning: opendir(%s/opendir_var*): Failed to open directory: %s in %s on line %d
bool(false)

Warning: opendir(%s/*): %s in %s on line %d

Warning: opendir(%s/*): Failed to open directory: %s in %s on line %d
bool(false)

-- Wildcard = '?' --

Warning: opendir(%s/opendir_variation6/sub_dir?): %s in %s on line %d

Warning: opendir(%s/opendir_variation6/sub_dir?): Failed to open directory: %s in %s on line %d
bool(false)

Warning: opendir(%s/opendir_variation6/sub?dir1): %s in %s on line %d

Warning: opendir(%s/opendir_variation6/sub?dir1): Failed to open directory: %s in %s on line %d
bool(false)
