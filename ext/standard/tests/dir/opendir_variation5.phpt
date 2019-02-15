--TEST--
Test opendir() function : usage variations - directories with restricted permissions
--SKIPIF--
<?php
if( substr(PHP_OS, 0, 3) == 'WIN') {
	die('skip Not for Windows');
}
// Skip if being run by root (files are always readable, writeable and executable)
$filename = dirname(__FILE__)."/dir_root_check.tmp";
$fp = fopen($filename, 'w');
fclose($fp);
if(fileowner($filename) == 0) {
	unlink ($filename);
	die('skip...cannot be run as root\n');
}
unlink($filename);
?>
--FILE--
<?php
/* Prototype  : mixed opendir(string $path[, resource $context])
 * Description: Open a directory and return a dir_handle
 * Source code: ext/standard/dir.c
 */

/*
 * remove the execute permission from the parent dir and test opendir() on child dir
 *   1) remove write & execute permission from the 1st parent and test opendir()
 *   2) remove execute permission from 2nd parent and test opendir()
 */

echo "*** Testing opendir() : usage variations ***\n";

/* create the temporary directory :
 * opendir_variation5  ( parent )
 *  |-> sub_dir    ( sub parent )
 *      |-> child_dir  ( child dir)
 */

$parent_dir_path = dirname(__FILE__) . "/opendir_variation5";
mkdir($parent_dir_path);
chmod($parent_dir_path, 0777);

// create sub_dir
$sub_dir_path = $parent_dir_path . "/sub_dir";
mkdir($sub_dir_path);
chmod($sub_dir_path, 0777);

//create sub_sub_dir
$child_dir_path = $sub_dir_path."/child_dir";
mkdir($child_dir_path);

// remove the write and execute permisson from sub parent
chmod($sub_dir_path, 0444);

echo "\n-- After restricting 1st level parent directory --\n";
$dir_handle1 = opendir($child_dir_path);
var_dump( $dir_handle1 );

// remove the execute permisson from parent dir, allowing all permission for sub dir
chmod($sub_dir_path, 0777); // all permisson to sub dir
chmod($parent_dir_path, 0666); // restricting parent directory

echo "\n-- After restricting parent directory --\n";
$dir_handle2 = opendir($child_dir_path); // try to open, expected failure
var_dump( $dir_handle2 ); // dump it

if (is_resource($dir_handle1)) {
	closedir($dir_handle1);
}
if (is_resource($dir_handle2)) {
	closedir($dir_handle2);
}
?>
===DONE===
--CLEAN--
<?php
$parent_dir_path = dirname(__FILE__) . "/opendir_variation5";
$sub_dir_path = $parent_dir_path."/sub_dir";
$child_dir_path = $sub_dir_path."/child_dir";

// changing permissions for each temporary directory to delete them
chmod($parent_dir_path, 0777);
chmod($sub_dir_path, 0777);
chmod($child_dir_path, 0777);

rmdir($child_dir_path);
rmdir($sub_dir_path);
rmdir($parent_dir_path);
?>
--EXPECTF--
*** Testing opendir() : usage variations ***

-- After restricting 1st level parent directory --

Warning: opendir(%s/opendir_variation5/sub_dir/child_dir): failed to open dir: %s in %s on line %d
bool(false)

-- After restricting parent directory --

Warning: opendir(%s/opendir_variation5/sub_dir/child_dir): failed to open dir: %s in %s on line %d
bool(false)
===DONE===
