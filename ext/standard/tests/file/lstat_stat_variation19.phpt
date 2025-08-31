--TEST--
Test lstat() and stat() functions: usage variations - dir/file names in array
--FILE--
<?php
/* test for stats of dir/file when their names are stored in an array */

$file_path = __DIR__;
require "$file_path/file.inc";


/* create temp file, link and directory */
@rmdir("$file_path/lstat_stat_variation19");  // ensure that dir doesn't exists
mkdir("$file_path/lstat_stat_variation19");  // temp dir

$fp = fopen("$file_path/lstat_stat_variation19.tmp", "w");  // temp file
fclose($fp);

echo "*** Testing stat() with filename & directory name stored inside an array ***\n";

// array with default numeric index
$names = array(
  "$file_path/lstat_stat_variation19.tmp",
  "$file_path/lstat_stat_variation19"
);

//array with string key index
$names_with_key = array (
  'file' => "$file_path/lstat_stat_variation19.tmp",
  "dir" => "$file_path/lstat_stat_variation19"
);

echo "\n-- Testing stat() on filename stored inside an array --\n";
var_dump( stat($names[0]) ); // values stored with numeric index
var_dump( stat($names_with_key['file']) ); // value stored with string key

echo "\n-- Testing stat() on dir name stored inside an array --\n";
var_dump( stat($names[1]) ); // values stored with numeric index
var_dump( stat($names_with_key["dir"]) ); // value stored with string key

echo "\n--- Done ---";
?>
--CLEAN--
<?php
$file_path = __DIR__;
unlink("$file_path/lstat_stat_variation19.tmp");
rmdir("$file_path/lstat_stat_variation19");
?>
--EXPECTF--
*** Testing stat() with filename & directory name stored inside an array ***

-- Testing stat() on filename stored inside an array --
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
  int(%i)
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
  int(%i)
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

-- Testing stat() on dir name stored inside an array --
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
  int(%i)
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
  int(%i)
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
