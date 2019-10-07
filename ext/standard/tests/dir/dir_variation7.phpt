--TEST--
Test dir() function : usage variations - directories with restricted permissions
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
 * Prototype  : object dir(string $directory[, resource $context])
 * Description: Directory class with properties, handle and class and methods read, rewind and close
 * Source code: ext/standard/dir.c
 */

/*
 * remove the execute permission from the parent dir and test dir() on child dir
 *   1) remove write & execute permission from the 1st parent and test dir()
 *   2) remove execute permission from 2nd parent and test dir()
 */

echo "*** Testing dir() : remove execute permission from the parent dir ***\n";

/* create the temporary directory :
  dir_variation7  ( parent )
    |-> sub_dir    ( sub parent )
         |-> child_dir  ( child dir)
*/
$file_path = __DIR__;
$parent_dir_path = $file_path."/dir_variation7";
@mkdir($parent_dir_path);
chmod($parent_dir_path, 0777);

// create sub_dir
$sub_dir_path = $parent_dir_path."/sub_dir";
@mkdir($sub_dir_path);
chmod($sub_dir_path, 0777);

//create sub_sub_dir
$child_dir_path = $sub_dir_path."/child_dir";
@mkdir($child_dir_path);

// remove the write and execute permisson from sub parent
chmod($sub_dir_path, 0444);
echo "-- After restricting 1st level parent directory --\n";
$d = dir($child_dir_path); // try to open, expected failure
var_dump( $d ); // dump it

// remove the execute permisson from parent dir, allowing all permission for sub dir
chmod($sub_dir_path, 0777); // all permisson to sub dir
chmod($parent_dir_path, 0666); // restricting parent directory
echo "-- After restricting parent directory --\n";
$d = dir($child_dir_path); // try to open, expected failure
var_dump( $d ); // dump it

echo "Done";
?>
--CLEAN--
<?php
$file_path = __DIR__;
$parent_dir_path = $file_path."/dir_variation7";
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
*** Testing dir() : remove execute permission from the parent dir ***
-- After restricting 1st level parent directory --

Warning: dir(%s/dir_variation7/sub_dir/child_dir): failed to open dir: %s in %s on line %d
bool(false)
-- After restricting parent directory --

Warning: dir(%s/dir_variation7/sub_dir/child_dir): failed to open dir: %s in %s on line %d
bool(false)
Done
