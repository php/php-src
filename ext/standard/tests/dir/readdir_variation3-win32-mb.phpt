--TEST--
Test readdir() function : usage variations - sub-directories
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') {
  die("skip Valid only on Windows");
}
?>
--FILE--
<?php
/*
 * Pass a directory handle pointing to a directory that has a sub-directory
 * to test behaviour of readdir()
 */

echo "*** Testing readdir() : usage variations ***\n";

// include the file.inc for Function: function create_files()
chdir(__DIR__);
include(__DIR__."/../file/file.inc");

$path_top = __DIR__ . '/readdir_variation3-win32-mb';
$path_sub = $path_top . '/私はガラスを食べられますsub_folder';
mkdir($path_top);
mkdir($path_sub);

create_files($path_top, 2);
create_files($path_sub, 2);

$dir_handle = opendir($path_top);
while(FALSE !== ($file = readdir($dir_handle))) {

    // different OS order files differently so will
    // store file names into an array so can use sorted in expected output
    $contents[] = $file;
}

// more important to check that all contents are present than order they are returned in
sort($contents);
var_dump($contents);

delete_files($path_top, 2);
delete_files($path_sub, 2);

closedir($dir_handle);
?>
--CLEAN--
<?php
$path_top = __DIR__ . '/readdir_variation3-win32-mb';
$path_sub = $path_top . '/私はガラスを食べられますsub_folder';
rmdir($path_sub);
rmdir($path_top);
?>
--EXPECT--
*** Testing readdir() : usage variations ***
array(5) {
  [0]=>
  string(1) "."
  [1]=>
  string(2) ".."
  [2]=>
  string(9) "file1.tmp"
  [3]=>
  string(9) "file2.tmp"
  [4]=>
  string(46) "私はガラスを食べられますsub_folder"
}
