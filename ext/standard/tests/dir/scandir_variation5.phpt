--TEST--
Test scandir() function : usage variations - different directory permissions
--SKIPIF--
<?php
if( substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip Not for Windows');
}
require __DIR__ . '/../skipif_root.inc';
?>
--FILE--
<?php
/*
 * remove the execute permission from the parent dir and test scandir() on child dir
 * 1. remove write & execute permission from the 1st parent and test scandir()
 * 2. remove execute permission from 2nd parent and test scandir()
 */

echo "*** Testing scandir() : usage variations ***\n";

/*
 * create the temporary directory :
 * scandir_variation5  ( parent )
 *  |-> sub_dir     ( sub parent )
 *      |-> child_dir  ( child dir)
 */

$parent_dir_path = __DIR__ . "/scandir_variation5";
mkdir($parent_dir_path);
chmod($parent_dir_path, 0777);

// create sub_dir
$sub_dir_path = $parent_dir_path . "/sub_dir";
mkdir($sub_dir_path);
chmod($sub_dir_path, 0777);

//create sub_sub_dir
$child_dir_path = $sub_dir_path."/child_dir";
mkdir($child_dir_path);

// remove the write and execute permission from sub parent
chmod($sub_dir_path, 0444);

echo "\n-- After restricting 1st level parent directory --\n";
var_dump(scandir($child_dir_path));

// remove the execute permission from parent dir, allowing all permission for sub dir
chmod($sub_dir_path, 0777); // all permission to sub dir
chmod($parent_dir_path, 0666); // restricting parent directory

echo "\n-- After restricting parent directory --\n";
var_dump(scandir($child_dir_path));
?>
--CLEAN--
<?php
$parent_dir_path = __DIR__ . "/scandir_variation5";
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
*** Testing scandir() : usage variations ***

-- After restricting 1st level parent directory --

Warning: scandir(%s/scandir_variation5/sub_dir/child_dir): Failed to open directory: %s in %s on line %d

Warning: scandir(): (errno %d): %s in %s on line %d
bool(false)

-- After restricting parent directory --

Warning: scandir(%s/scandir_variation5/sub_dir/child_dir): Failed to open directory: %s in %s on line %d

Warning: scandir(): (errno %d): %s in %s on line %d
bool(false)
