--TEST--
Phar: url stat
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.require_hash=0
--FILE--
<?php
$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php';
$pname = 'phar://' . $fname;
$file = (binary)"<?php
Phar::mapPhar('hio');
__HALT_COMPILER(); ?>";

$pfile = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php';
$files = array();
$files['a'] = 'a';
$files['b/a'] = 'b';
$files['b/c/d'] = 'c';
$files['bad/c'] = 'd';
include 'files/phar_test.inc';
include $fname;

var_dump(stat('phar://hio/a'), stat('phar://hio/b'));
?>
--CLEAN--
<?php unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
--EXPECTF--
array(26) {
  [0]=>
  int(12)
  [1]=>
  int(%d)
  [2]=>
  int(33060)
  [3]=>
  int(1)
  [4]=>
  int(0)
  [5]=>
  int(0)
  [6]=>
  int(-1)
  [7]=>
  int(1)
  [8]=>
  int(%d)
  [9]=>
  int(%d)
  [10]=>
  int(%d)
  [11]=>
  int(-1)
  [12]=>
  int(-1)
  [u"dev"]=>
  int(12)
  [u"ino"]=>
  int(%d)
  [u"mode"]=>
  int(33060)
  [u"nlink"]=>
  int(1)
  [u"uid"]=>
  int(0)
  [u"gid"]=>
  int(0)
  [u"rdev"]=>
  int(-1)
  [u"size"]=>
  int(1)
  [u"atime"]=>
  int(%d)
  [u"mtime"]=>
  int(%d)
  [u"ctime"]=>
  int(%d)
  [u"blksize"]=>
  int(-1)
  [u"blocks"]=>
  int(-1)
}
array(26) {
  [0]=>
  int(12)
  [1]=>
  int(%d)
  [2]=>
  int(16749)
  [3]=>
  int(1)
  [4]=>
  int(0)
  [5]=>
  int(0)
  [6]=>
  int(-1)
  [7]=>
  int(0)
  [8]=>
  int(%d)
  [9]=>
  int(%d)
  [10]=>
  int(%d)
  [11]=>
  int(-1)
  [12]=>
  int(-1)
  [u"dev"]=>
  int(12)
  [u"ino"]=>
  int(%d)
  [u"mode"]=>
  int(16749)
  [u"nlink"]=>
  int(1)
  [u"uid"]=>
  int(0)
  [u"gid"]=>
  int(0)
  [u"rdev"]=>
  int(-1)
  [u"size"]=>
  int(0)
  [u"atime"]=>
  int(%d)
  [u"mtime"]=>
  int(%d)
  [u"ctime"]=>
  int(%d)
  [u"blksize"]=>
  int(-1)
  [u"blocks"]=>
  int(-1)
}