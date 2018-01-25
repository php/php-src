--TEST--
Test lstat() & stat() functions: basic functionality
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
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
@rmdir($dirname);
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
  int(%i)
  [1]=>
  int(%i)
  [2]=>
  int(%i)
  [3]=>
  int(%i)
  [4]=>
  int(%i)
  [5]=>
  int(%i)
  [6]=>
  int(%i)
  [7]=>
  int(%i)
  [8]=>
  int(%i)
  [9]=>
  int(%i)
  [10]=>
  int(%i)
  [11]=>
  int(%i)
  [12]=>
  int(%i)
  ["dev"]=>
  int(%i)
  ["ino"]=>
  int(%i)
  ["mode"]=>
  int(%i)
  ["nlink"]=>
  int(%i)
  ["uid"]=>
  int(%i)
  ["gid"]=>
  int(%i)
  ["rdev"]=>
  int(%i)
  ["size"]=>
  int(%i)
  ["atime"]=>
  int(%i)
  ["mtime"]=>
  int(%i)
  ["ctime"]=>
  int(%i)
  ["blksize"]=>
  int(%i)
  ["blocks"]=>
  int(%i)
}
array(26) {
  [0]=>
  int(%i)
  [1]=>
  int(%i)
  [2]=>
  int(%i)
  [3]=>
  int(%i)
  [4]=>
  int(%i)
  [5]=>
  int(%i)
  [6]=>
  int(%i)
  [7]=>
  int(%i)
  [8]=>
  int(%i)
  [9]=>
  int(%i)
  [10]=>
  int(%i)
  [11]=>
  int(%i)
  [12]=>
  int(%i)
  ["dev"]=>
  int(%i)
  ["ino"]=>
  int(%i)
  ["mode"]=>
  int(%i)
  ["nlink"]=>
  int(%i)
  ["uid"]=>
  int(%i)
  ["gid"]=>
  int(%i)
  ["rdev"]=>
  int(%i)
  ["size"]=>
  int(%i)
  ["atime"]=>
  int(%i)
  ["mtime"]=>
  int(%i)
  ["ctime"]=>
  int(%i)
  ["blksize"]=>
  int(%i)
  ["blocks"]=>
  int(%i)
}
bool(true)
-- comparing difference in file stats before and after creating link to it --
array(26) {
  [0]=>
  int(%i)
  [1]=>
  int(%i)
  [2]=>
  int(%i)
  [3]=>
  int(%i)
  [4]=>
  int(%i)
  [5]=>
  int(%i)
  [6]=>
  int(%i)
  [7]=>
  int(%i)
  [8]=>
  int(%i)
  [9]=>
  int(%i)
  [10]=>
  int(%i)
  [11]=>
  int(%i)
  [12]=>
  int(%i)
  ["dev"]=>
  int(%i)
  ["ino"]=>
  int(%i)
  ["mode"]=>
  int(%i)
  ["nlink"]=>
  int(%i)
  ["uid"]=>
  int(%i)
  ["gid"]=>
  int(%i)
  ["rdev"]=>
  int(%i)
  ["size"]=>
  int(%i)
  ["atime"]=>
  int(%i)
  ["mtime"]=>
  int(%i)
  ["ctime"]=>
  int(%i)
  ["blksize"]=>
  int(%i)
  ["blocks"]=>
  int(%i)
}
array(26) {
  [0]=>
  int(%i)
  [1]=>
  int(%i)
  [2]=>
  int(%i)
  [3]=>
  int(%i)
  [4]=>
  int(%i)
  [5]=>
  int(%i)
  [6]=>
  int(%i)
  [7]=>
  int(%i)
  [8]=>
  int(%i)
  [9]=>
  int(%i)
  [10]=>
  int(%i)
  [11]=>
  int(%i)
  [12]=>
  int(%i)
  ["dev"]=>
  int(%i)
  ["ino"]=>
  int(%i)
  ["mode"]=>
  int(%i)
  ["nlink"]=>
  int(%i)
  ["uid"]=>
  int(%i)
  ["gid"]=>
  int(%i)
  ["rdev"]=>
  int(%i)
  ["size"]=>
  int(%i)
  ["atime"]=>
  int(%i)
  ["mtime"]=>
  int(%i)
  ["ctime"]=>
  int(%i)
  ["blksize"]=>
  int(%i)
  ["blocks"]=>
  int(%i)
}
bool(true)
Done
