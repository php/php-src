--TEST--
Test lstat() & stat() functions: basic functionality
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip.. lstat() not available on Windows');
}
?>
--FILE--
<?php
/* Prototype: array lstat ( string $filename );
   Description: Gives information about a file or symbolic link

   Prototype: array stat ( string $filename );
   Description: Gives information about a file
*/

$file_path = dirname(__FILE__);
require("$file_path/file.inc");

echo "*** Testing lstat() & stat() : basic functionality ***\n";

/* creating temp directory and file */

// creating dir
$dirname = "$file_path/lstat_stat_basic";
mkdir($dirname);
// stat of the dir created
$dir_stat = stat($dirname);
clearstatcache();
sleep(2);

// creating file
$filename = "$dirname/lstat_stat_basic.tmp";
$file_handle = fopen($filename, "w");
fclose($file_handle);
// stat of the file created
$file_stat = stat($filename);
sleep(2);

// now new stat of the dir after file is created
$new_dir_stat = stat($dirname);
clearstatcache();

// create soft link and record stat
$sym_linkname = "$file_path/lstat_stat_basic_link.tmp";
symlink($filename, $sym_linkname);
// stat of the link created
$link_stat = lstat($sym_linkname);
sleep(2);
// new stat of the file, after a softlink to this file is created
$new_file_stat = stat($filename);
clearstatcache();

// stat contains 13 different values stored twice, can be accessed using 
// numeric and named keys, compare them to see they are same  
echo "*** Testing stat() and lstat() : validating the values stored in stat ***\n";
// Initial stat values
var_dump( compare_self_stat($file_stat) ); //expect true
var_dump( compare_self_stat($dir_stat) );  //expect true
var_dump( compare_self_stat($link_stat) ); // expect true

// New stat values taken after creation of file & link
var_dump( compare_self_stat($new_file_stat) ); //expect true
var_dump( compare_self_stat($new_dir_stat) );  // expect true

// compare the two stat values, initial stat and stat recorded after 
// creating files and link, also dump the value of stats
echo "*** Testing stat() and lstat() : comparing stats (recorded before and after file/link creation) ***\n";
echo "-- comparing difference in dir stats before and after creating file in it --\n";
$affected_elements = array( 9, 10, 'mtime', 'ctime' );
var_dump( compare_stats($dir_stat, $new_dir_stat, $affected_elements, '!=', true) ); // expect true

echo "-- comparing difference in file stats before and after creating link to it --\n";
var_dump( compare_stats($file_stat, $new_file_stat, $all_stat_keys, "==", true) ); // expect true

echo "Done\n";
?>
--CLEAN--
<?php
$file_path = dirname(__FILE__);
unlink("$file_path/lstat_stat_basic_link.tmp");
unlink("$file_path/lstat_stat_basic/lstat_stat_basic.tmp");
rmdir("$file_path/lstat_stat_basic");
?>
--EXPECTF--
*** Testing lstat() & stat() : basic functionality ***
*** Testing stat() and lstat() : validating the values stored in stat ***
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
*** Testing stat() and lstat() : comparing stats (recorded before and after file/link creation) ***
-- comparing difference in dir stats before and after creating file in it --
array(26) {
  [0]=>
  int(%d)
  [1]=>
  int(%d)
  [2]=>
  int(%d)
  [3]=>
  int(%d)
  [4]=>
  int(%d)
  [5]=>
  int(%d)
  [6]=>
  int(%d)
  [7]=>
  int(%d)
  [8]=>
  int(%d)
  [9]=>
  int(%d)
  [10]=>
  int(%d)
  [11]=>
  int(%d)
  [12]=>
  int(%d)
  ["dev"]=>
  int(%d)
  ["ino"]=>
  int(%d)
  ["mode"]=>
  int(%d)
  ["nlink"]=>
  int(%d)
  ["uid"]=>
  int(%d)
  ["gid"]=>
  int(%d)
  ["rdev"]=>
  int(%d)
  ["size"]=>
  int(%d)
  ["atime"]=>
  int(%d)
  ["mtime"]=>
  int(%d)
  ["ctime"]=>
  int(%d)
  ["blksize"]=>
  int(%d)
  ["blocks"]=>
  int(%d)
}
array(26) {
  [0]=>
  int(%d)
  [1]=>
  int(%d)
  [2]=>
  int(%d)
  [3]=>
  int(%d)
  [4]=>
  int(%d)
  [5]=>
  int(%d)
  [6]=>
  int(%d)
  [7]=>
  int(%d)
  [8]=>
  int(%d)
  [9]=>
  int(%d)
  [10]=>
  int(%d)
  [11]=>
  int(%d)
  [12]=>
  int(%d)
  ["dev"]=>
  int(%d)
  ["ino"]=>
  int(%d)
  ["mode"]=>
  int(%d)
  ["nlink"]=>
  int(%d)
  ["uid"]=>
  int(%d)
  ["gid"]=>
  int(%d)
  ["rdev"]=>
  int(%d)
  ["size"]=>
  int(%d)
  ["atime"]=>
  int(%d)
  ["mtime"]=>
  int(%d)
  ["ctime"]=>
  int(%d)
  ["blksize"]=>
  int(%d)
  ["blocks"]=>
  int(%d)
}
bool(true)
-- comparing difference in file stats before and after creating link to it --
array(26) {
  [0]=>
  int(%d)
  [1]=>
  int(%d)
  [2]=>
  int(%d)
  [3]=>
  int(%d)
  [4]=>
  int(%d)
  [5]=>
  int(%d)
  [6]=>
  int(%d)
  [7]=>
  int(%d)
  [8]=>
  int(%d)
  [9]=>
  int(%d)
  [10]=>
  int(%d)
  [11]=>
  int(%d)
  [12]=>
  int(%d)
  ["dev"]=>
  int(%d)
  ["ino"]=>
  int(%d)
  ["mode"]=>
  int(%d)
  ["nlink"]=>
  int(%d)
  ["uid"]=>
  int(%d)
  ["gid"]=>
  int(%d)
  ["rdev"]=>
  int(%d)
  ["size"]=>
  int(%d)
  ["atime"]=>
  int(%d)
  ["mtime"]=>
  int(%d)
  ["ctime"]=>
  int(%d)
  ["blksize"]=>
  int(%d)
  ["blocks"]=>
  int(%d)
}
array(26) {
  [0]=>
  int(%d)
  [1]=>
  int(%d)
  [2]=>
  int(%d)
  [3]=>
  int(%d)
  [4]=>
  int(%d)
  [5]=>
  int(%d)
  [6]=>
  int(%d)
  [7]=>
  int(%d)
  [8]=>
  int(%d)
  [9]=>
  int(%d)
  [10]=>
  int(%d)
  [11]=>
  int(%d)
  [12]=>
  int(%d)
  ["dev"]=>
  int(%d)
  ["ino"]=>
  int(%d)
  ["mode"]=>
  int(%d)
  ["nlink"]=>
  int(%d)
  ["uid"]=>
  int(%d)
  ["gid"]=>
  int(%d)
  ["rdev"]=>
  int(%d)
  ["size"]=>
  int(%d)
  ["atime"]=>
  int(%d)
  ["mtime"]=>
  int(%d)
  ["ctime"]=>
  int(%d)
  ["blksize"]=>
  int(%d)
  ["blocks"]=>
  int(%d)
}
bool(true)
Done
--UEXPECTF--
*** Testing lstat() & stat() : basic functionality ***
*** Testing stat() and lstat() : validating the values stored in stat ***
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
*** Testing stat() and lstat() : comparing stats (recorded before and after file/link creation) ***
-- comparing difference in dir stats before and after creating file in it --
array(26) {
  [0]=>
  int(%d)
  [1]=>
  int(%d)
  [2]=>
  int(%d)
  [3]=>
  int(%d)
  [4]=>
  int(%d)
  [5]=>
  int(%d)
  [6]=>
  int(%d)
  [7]=>
  int(%d)
  [8]=>
  int(%d)
  [9]=>
  int(%d)
  [10]=>
  int(%d)
  [11]=>
  int(%d)
  [12]=>
  int(%d)
  [u"dev"]=>
  int(%d)
  [u"ino"]=>
  int(%d)
  [u"mode"]=>
  int(%d)
  [u"nlink"]=>
  int(%d)
  [u"uid"]=>
  int(%d)
  [u"gid"]=>
  int(%d)
  [u"rdev"]=>
  int(%d)
  [u"size"]=>
  int(%d)
  [u"atime"]=>
  int(%d)
  [u"mtime"]=>
  int(%d)
  [u"ctime"]=>
  int(%d)
  [u"blksize"]=>
  int(%d)
  [u"blocks"]=>
  int(%d)
}
array(26) {
  [0]=>
  int(%d)
  [1]=>
  int(%d)
  [2]=>
  int(%d)
  [3]=>
  int(%d)
  [4]=>
  int(%d)
  [5]=>
  int(%d)
  [6]=>
  int(%d)
  [7]=>
  int(%d)
  [8]=>
  int(%d)
  [9]=>
  int(%d)
  [10]=>
  int(%d)
  [11]=>
  int(%d)
  [12]=>
  int(%d)
  [u"dev"]=>
  int(%d)
  [u"ino"]=>
  int(%d)
  [u"mode"]=>
  int(%d)
  [u"nlink"]=>
  int(%d)
  [u"uid"]=>
  int(%d)
  [u"gid"]=>
  int(%d)
  [u"rdev"]=>
  int(%d)
  [u"size"]=>
  int(%d)
  [u"atime"]=>
  int(%d)
  [u"mtime"]=>
  int(%d)
  [u"ctime"]=>
  int(%d)
  [u"blksize"]=>
  int(%d)
  [u"blocks"]=>
  int(%d)
}
bool(true)
-- comparing difference in file stats before and after creating link to it --
array(26) {
  [0]=>
  int(%d)
  [1]=>
  int(%d)
  [2]=>
  int(%d)
  [3]=>
  int(%d)
  [4]=>
  int(%d)
  [5]=>
  int(%d)
  [6]=>
  int(%d)
  [7]=>
  int(%d)
  [8]=>
  int(%d)
  [9]=>
  int(%d)
  [10]=>
  int(%d)
  [11]=>
  int(%d)
  [12]=>
  int(%d)
  [u"dev"]=>
  int(%d)
  [u"ino"]=>
  int(%d)
  [u"mode"]=>
  int(%d)
  [u"nlink"]=>
  int(%d)
  [u"uid"]=>
  int(%d)
  [u"gid"]=>
  int(%d)
  [u"rdev"]=>
  int(%d)
  [u"size"]=>
  int(%d)
  [u"atime"]=>
  int(%d)
  [u"mtime"]=>
  int(%d)
  [u"ctime"]=>
  int(%d)
  [u"blksize"]=>
  int(%d)
  [u"blocks"]=>
  int(%d)
}
array(26) {
  [0]=>
  int(%d)
  [1]=>
  int(%d)
  [2]=>
  int(%d)
  [3]=>
  int(%d)
  [4]=>
  int(%d)
  [5]=>
  int(%d)
  [6]=>
  int(%d)
  [7]=>
  int(%d)
  [8]=>
  int(%d)
  [9]=>
  int(%d)
  [10]=>
  int(%d)
  [11]=>
  int(%d)
  [12]=>
  int(%d)
  [u"dev"]=>
  int(%d)
  [u"ino"]=>
  int(%d)
  [u"mode"]=>
  int(%d)
  [u"nlink"]=>
  int(%d)
  [u"uid"]=>
  int(%d)
  [u"gid"]=>
  int(%d)
  [u"rdev"]=>
  int(%d)
  [u"size"]=>
  int(%d)
  [u"atime"]=>
  int(%d)
  [u"mtime"]=>
  int(%d)
  [u"ctime"]=>
  int(%d)
  [u"blksize"]=>
  int(%d)
  [u"blocks"]=>
  int(%d)
}
bool(true)
Done
