--TEST--
Phar: test stat function interceptions
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip");?>
--INI--
phar.require_hash=1
phar.readonly=0
--FILE--
<?php
umask(0);
Phar::interceptFileFuncs();
var_dump(stat(""));

$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar.php';
$fname2 = __DIR__ . '/' . basename(__FILE__, '.php') . '.tar';
$fname3 = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar.tar';
$a = new Phar($fname);
$a['my/index.php'] = '<?php
echo "stat\n";
var_dump(stat("dir/file1.txt"));
echo "lstat\n";
var_dump(lstat("dir/file1.txt"));
echo "fileperms\n";
var_dump(fileperms("dir/file1.txt"));
echo "fileinode\n";
var_dump(fileinode("dir/file1.txt"));
echo "filesize\n";
var_dump(filesize("dir/file1.txt"));
echo "fileowner\n";
var_dump(fileowner("dir/file1.txt"));
echo "filegroup\n";
var_dump(filegroup("dir/file1.txt"));
echo "filemtime\n";
var_dump(filemtime("dir/file1.txt"));
echo "fileatime\n";
var_dump(fileatime("dir/file1.txt"));
echo "filectime\n";
var_dump(filectime("dir/file1.txt"));
echo "filetype\n";
var_dump(filetype("dir/file1.txt"));
echo "is_writable\n";
var_dump(is_writable("dir/file1.txt"));
echo "is_writeable\n";
var_dump(is_writeable("dir/file1.txt"));
echo "is_readable\n";
var_dump(is_readable("dir/file1.txt"));
echo "is_executable\n";
var_dump(is_executable("dir/file1.txt"));
echo "file_exists\n";
var_dump(file_exists("dir/file1.txt"));
echo "is_dir\n";
var_dump(is_dir("dir/file1.txt"));
echo "is_file\n";
var_dump(is_file("dir/file1.txt"));
echo "is_link\n";
var_dump(is_link("dir/file1.txt"));
echo "not found\n";
var_dump(file_exists("not/found"));
echo "not found 2\n";
var_dump(fileperms("not/found"));
?>';
$a['dir/file1.txt'] = 'hi';
$a['dir/file2.txt'] = 'hi2';
$a['dir/file3.txt'] = 'hi3';
$a->setStub('<?php
set_include_path("phar://" . __FILE__ . "/dir" . PATH_SEPARATOR . "phar://" . __FILE__);
include "my/index.php";
__HALT_COMPILER();');
include $fname;
?>
===DONE===
--CLEAN--
<?php unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
--EXPECTF--
bool(false)
stat
array(26) {
  [0]=>
  int(12)
  [1]=>
  int(%d)
  [2]=>
  int(33206)
  [3]=>
  int(1)
  [4]=>
  int(0)
  [5]=>
  int(0)
  [6]=>
  int(%s)
  [7]=>
  int(2)
  [8]=>
  int(%d)
  [9]=>
  int(%d)
  [10]=>
  int(%d)
  [11]=>
  int(%s)
  [12]=>
  int(%s)
  ["dev"]=>
  int(12)
  ["ino"]=>
  int(%d)
  ["mode"]=>
  int(33206)
  ["nlink"]=>
  int(1)
  ["uid"]=>
  int(0)
  ["gid"]=>
  int(0)
  ["rdev"]=>
  int(%s)
  ["size"]=>
  int(2)
  ["atime"]=>
  int(%d)
  ["mtime"]=>
  int(%d)
  ["ctime"]=>
  int(%d)
  ["blksize"]=>
  int(%s)
  ["blocks"]=>
  int(%s)
}
lstat
array(26) {
  [0]=>
  int(12)
  [1]=>
  int(%d)
  [2]=>
  int(33206)
  [3]=>
  int(1)
  [4]=>
  int(0)
  [5]=>
  int(0)
  [6]=>
  int(%s)
  [7]=>
  int(2)
  [8]=>
  int(%d)
  [9]=>
  int(%d)
  [10]=>
  int(%d)
  [11]=>
  int(%s)
  [12]=>
  int(%s)
  ["dev"]=>
  int(12)
  ["ino"]=>
  int(%d)
  ["mode"]=>
  int(33206)
  ["nlink"]=>
  int(1)
  ["uid"]=>
  int(0)
  ["gid"]=>
  int(0)
  ["rdev"]=>
  int(%s)
  ["size"]=>
  int(2)
  ["atime"]=>
  int(%d)
  ["mtime"]=>
  int(%d)
  ["ctime"]=>
  int(%d)
  ["blksize"]=>
  int(%s)
  ["blocks"]=>
  int(%s)
}
fileperms
int(33206)
fileinode
int(%d)
filesize
int(2)
fileowner
int(0)
filegroup
int(0)
filemtime
int(%d)
fileatime
int(%d)
filectime
int(%d)
filetype
string(4) "file"
is_writable
bool(true)
is_writeable
bool(false)
is_readable
bool(true)
is_executable
bool(false)
file_exists
bool(true)
is_dir
bool(false)
is_file
bool(true)
is_link
bool(false)
not found
bool(false)
not found 2

Warning: fileperms(): stat failed for not/found in phar://%sstat.phar.php/my/index.php on line %d
bool(false)
===DONE===
