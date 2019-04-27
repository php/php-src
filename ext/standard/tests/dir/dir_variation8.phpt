--TEST--
Test dir() function : usage variations - checking with wildcard characters
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip.. Not valid for Windows');
}
?>
--FILE--
<?php
/*
 * Prototype  : object dir(string $directory[, resource $context])
 * Description: Directory class with properties, handle and class and methods read, rewind and close
 * Source code: ext/standard/dir.c
 */

/*
 * Create more than one temporary directory & subdirectory and check if dir() function can open
 * those directories when wildcard characters are used to refer to them.
 */

echo "*** Testing dir() : checking with wildcard characters ***\n";

// create the temporary directories
$file_path = __DIR__;
$dir_path = $file_path."/dir_variation81";
$sub_dir_path = $dir_path."/sub_dir1";

@mkdir($dir_path1);
@mkdir($sub_dir_path);

/* with different wildcard characters */

echo "-- wildcard = '*' --\n";
var_dump( dir($file_path."/dir_var*") );
var_dump( dir($file_path."/*") );

echo "-- wildcard = '?' --\n";
var_dump( dir($dir_path."/sub_dir?") );
var_dump( dir($dir_path."/sub?dir1") );

echo "Done";
?>
--EXPECTF--
*** Testing dir() : checking with wildcard characters ***
-- wildcard = '*' --

Warning: dir(%s/dir_var*): failed to open dir: %s in %s on line %d
bool(false)

Warning: dir(%s/*): failed to open dir: %s in %s on line %d
bool(false)
-- wildcard = '?' --

Warning: dir(%s/dir_variation81/sub_dir?): failed to open dir: %s in %s on line %d
bool(false)

Warning: dir(%s/dir_variation81/sub?dir1): failed to open dir: %s in %s on line %d
bool(false)
Done
