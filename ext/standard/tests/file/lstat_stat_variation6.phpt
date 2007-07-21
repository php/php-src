--TEST--
Test lstat() and stat() functions: usage variations - dir/file/link names in objects

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

/* test for stats of dir/file/link when their names are stored in objects */

$file_path = dirname(__FILE__);
require "$file_path/file.inc";


/* create temp file, link and directory */
mkdir("$file_path/lstat_stat_variation6/");  // temp dir

$fp = fopen("$file_path/lstat_stat_variation6.tmp", "w");  // temp file
fclose($fp);

// temp link
symlink("$file_path/lstat_stat_variation6.tmp", "$file_path/lstat_stat_variation6_link.tmp");

/* Checking lstat() and stat() on file/link/directory where filenames,
   linknames and directory names are stored in object and array */

echo "\n*** Testing lstat() and stat() with filename, linkname
            and directory name stored inside a object ***\n";

// creating object with members as numeric and non-numeric filename, linkname and directory name
class object_temp {
  public $var_name;
  public function object_temp($name) {
    $this->var_name = $name;
  }
}
// directory as member
$obj1 = new object_temp("$file_path/lstat_stat_variation6/");
$obj2 = new object_temp("$file_path/lstat_stat_variation6a/");

// file as member
$obj3 = new object_temp("$file_path/lstat_stat_variation6.tmp");
$obj4 = new object_temp("$file_path/lstat_stat_variation6a.tmp");

// link as member
$obj5 = new object_temp("$file_path/lstat_stat_variation6_link.tmp");
$obj6 = new object_temp("$file_path/lstat_stat_variation6a_link.tmp");

echo "\n-- Testing lstat() and stat() with softlink, linkname stored inside an object --\n";

var_dump( lstat($obj5->var_name) );

var_dump( symlink($obj5->var_name, $obj6->var_name) );
var_dump( lstat($obj6->var_name) );

echo "\n-- Testing stat() on filename stored inside an object --\n";
var_dump( stat($obj3->var_name) );

$fp = fopen("$file_path/lstat_stat_variation6a.tmp", "w");
fclose($fp);
var_dump( stat($obj4->var_name) );

echo "\n-- Testing stat() on directory name stored inside an object --\n";
var_dump( stat($obj1->var_name) );

mkdir("$file_path/lstat_stat_variation6a/");
var_dump( stat($obj2->var_name) );

echo "\n--- Done ---";
?>

--CLEAN--
<?php
$file_path = dirname(__FILE__);
unlink("$file_path/lstat_stat_variation6_link.tmp");
unlink("$file_path/lstat_stat_variation6.tmp");
rmdir("$file_path/lstat_stat_variation6");
unlink("$file_path/lstat_stat_variation6a_link.tmp");
unlink("$file_path/lstat_stat_variation6a.tmp");
rmdir("$file_path/lstat_stat_variation6a/");
?>

--EXPECTF--
*** Testing lstat() and stat() with filename, linkname
            and directory name stored inside a object ***

-- Testing lstat() and stat() with softlink, linkname stored inside an object --
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

-- Testing stat() on filename stored inside an object --
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

-- Testing stat() on directory name stored inside an object --
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

--- Done ---
