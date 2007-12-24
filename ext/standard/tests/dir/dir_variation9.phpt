--TEST--
Test dir() function : usage variations - relative valid and invalid paths
--FILE--
<?php
/* 
 * Prototype  : object dir(string $directory[, resource $context])
 * Description: Directory class with properties, handle and class and methods read, rewind and close
 * Source code: ext/standard/dir.c
 */

/* 
 * Checking the behavior of dir() function by passing directories which
 * have valid and invalid relative path.
 */

echo "*** Testing dir() : checking with valid and invalid paths ***\n";

/* create the temporary directories */

$file_path = dirname(__FILE__);

// directory dir_variation91 with one sub-directory sub_dir11 and sub-sub-directory sub_dir111
$dir_path1 = $file_path."/dir_variation91";
$sub_dir11 = $dir_path1."/sub_dir11";
$sub_dir111 = $sub_dir11."/sub_dir111";

// directory dir_variation92 with one sub-directory sub_dir21
$dir_path2 = $file_path."/dir_variation92";
$sub_dir21 = $dir_path2."/sub_dir21";

@mkdir($dir_path1);
@mkdir($dir_path2);
@mkdir($sub_dir11);
@mkdir($sub_dir111);
@mkdir($sub_dir21);

// open the directory with valid paths
echo "\n-- With valid paths --\n";
var_dump( dir("$dir_path1/sub_dir11/sub_dir111/..") );
var_dump( dir("$dir_path2/sub_dir21/../../dir_variation91") );
var_dump( dir("$dir_path2/sub_dir21/../../dir_variation91/sub_dir11/..") );
var_dump( dir("$dir_path1/sub_dir11/sub_dir111/../../../dir_variation92/sub_dir21/..") );

// open the directory with invalid path
echo "\n-- With invalid paths --\n";
var_dump( dir("$dir_path1/sub_dir12/sub_dir111/..") );
var_dump( dir("$dir_path2/sub_dir21/../dir_variation91") );
var_dump( dir("$dir_path2/sub_dir21/../../dir_variation91/sub_dir12/..") );
var_dump( dir("$dir_path1/sub_dir11/sub_dir111/../../dir_variation92/sub_dir21/..") );

echo "Done";
?>
--CLEAN--
<?php
$file_path = dirname(__FILE__);

$dir_path1 = $file_path."/dir_variation91";
$sub_dir11 = $dir_path1."/sub_dir11";
$sub_dir111 = $sub_dir11."/sub_dir111";
$dir_path2 = $file_path."/dir_variation92";
$sub_dir21 = $dir_path2."/sub_dir21";

rmdir($sub_dir21);
rmdir($sub_dir111);
rmdir($sub_dir11);
rmdir($dir_path1);
rmdir($dir_path2);
?>
--EXPECTF--
*** Testing dir() : checking with valid and invalid paths ***

-- With valid paths --
object(Directory)#%d (2) {
  ["path"]=>
  string(%d) "%s/dir_variation91/sub_dir11/sub_dir111/.."
  ["handle"]=>
  resource(%d) of type (stream)
}
object(Directory)#%d (2) {
  ["path"]=>
  string(%d) "%s/dir_variation92/sub_dir21/../../dir_variation91"
  ["handle"]=>
  resource(%d) of type (stream)
}
object(Directory)#%d (2) {
  ["path"]=>
  string(%d) "%s/dir_variation92/sub_dir21/../../dir_variation91/sub_dir11/.."
  ["handle"]=>
  resource(%d) of type (stream)
}
object(Directory)#%d (2) {
  ["path"]=>
  string(%d) "%s/dir_variation91/sub_dir11/sub_dir111/../../../dir_variation92/sub_dir21/.."
  ["handle"]=>
  resource(%d) of type (stream)
}

-- With invalid paths --

Warning: dir(%s/dir_variation91/sub_dir12/sub_dir111/..): failed to open dir: %s in %s on line %d
bool(false)

Warning: dir(%s/dir_variation92/sub_dir21/../dir_variation91): failed to open dir: %s in %s on line %d
bool(false)

Warning: dir(%s/dir_variation92/sub_dir21/../../dir_variation91/sub_dir12/..): failed to open dir: %s in %s on line %d
bool(false)

Warning: dir(%s/dir_variation91/sub_dir11/sub_dir111/../../dir_variation92/sub_dir21/..): failed to open dir: %s in %s on line %d
bool(false)
Done
