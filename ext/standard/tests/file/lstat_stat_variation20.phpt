--TEST--
Test lstat() and stat() functions: usage variations - link names stored in array/object
--SKIPIF--
<?php
if (PHP_OS_FAMILY === 'Windows') {
    include_once __DIR__ . '/windows_links/common.inc';
    skipIfSeCreateSymbolicLinkPrivilegeIsDisabled(__FILE__);
}
?>
--FILE--
<?php
/* test for stats of link when their names are stored in object and array */

$file_path = __DIR__;
require "$file_path/file.inc";

$fp = fopen("$file_path/lstat_stat_variation20.tmp", "w");  // temp file
fclose($fp);

// temp link
symlink("$file_path/lstat_stat_variation20.tmp", "$file_path/lstat_stat_variation20_link.tmp");

echo "*** Testing lstat() with linkname stored inside an object/array ***\n";
class names {
  public $var_name;
  public function __construct($name) {
    $this->var_name = $name;
  }
}

// link name stored in an object
$link_object = new names("$file_path/lstat_stat_variation20_link.tmp");

// link name stored in side an array
// with default numeric key
$link_array = array("$file_path/lstat_stat_variation20_link.tmp");

// with string key index
$link_array_with_key = array("linkname" => "$file_path/lstat_stat_variation20_link.tmp");

echo "\n-- Testing lstat() on link name stored inside an object --\n";
var_dump( lstat($link_object->var_name) );

echo "\n-- Testing stat() on link name stored inside an array --\n";
var_dump( stat($link_array[0]) ); // with default numeric index
var_dump( stat($link_array_with_key["linkname"]) ); // with string key
var_dump( stat($link_array_with_key['linkname']) );

echo "\n--- Done ---";
?>
--CLEAN--
<?php
$file_path = __DIR__;
unlink("$file_path/lstat_stat_variation20_link.tmp");
unlink("$file_path/lstat_stat_variation20.tmp");
?>
--EXPECTF--
*** Testing lstat() with linkname stored inside an object/array ***

-- Testing lstat() on link name stored inside an object --
array(26) {
  [0]=>
  int(%i)
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
  int(%i)
  [12]=>
  int(%i)
  ["dev"]=>
  int(%i)
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
  int(%i)
  ["blocks"]=>
  int(%i)
}

-- Testing stat() on link name stored inside an array --
array(26) {
  [0]=>
  int(%i)
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
  int(%i)
  [12]=>
  int(%i)
  ["dev"]=>
  int(%i)
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
  int(%i)
  ["blocks"]=>
  int(%i)
}
array(26) {
  [0]=>
  int(%i)
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
  int(%i)
  [12]=>
  int(%i)
  ["dev"]=>
  int(%i)
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
  int(%i)
  ["blocks"]=>
  int(%i)
}
array(26) {
  [0]=>
  int(%i)
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
  int(%i)
  [12]=>
  int(%i)
  ["dev"]=>
  int(%i)
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
  int(%i)
  ["blocks"]=>
  int(%i)
}

--- Done ---
