--TEST--
Phar: url stat
--SKIPIF--
<?php if (!extension_loaded("phar")) print "skip"; ?>
--FILE--
<?php
$file = "<?php
Phar::mapPhar('hio');
__HALT_COMPILER(); ?>";

$files = array();
$files['a'] = 'a';
$files['b/a'] = 'b';
$files['b/c/d'] = 'c';
$files['bad/c'] = 'd';
$manifest = '';
foreach($files as $name => $cont) {
	$len = strlen($cont);
	$manifest .= pack('V', strlen($name)) . $name . pack('VVVVC', $len, time(), $len, crc32($cont), 0x00);
}
$alias = 'hio';
$manifest = pack('VnV', count($files), 0x0800, strlen($alias)) . $alias . $manifest;
$file .= pack('V', strlen($manifest)) . $manifest;
foreach($files as $cont)
{
	$file .= $cont;
}

file_put_contents(dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php', $file);
include dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php';
var_dump(stat('phar://hio/a'), stat('phar://hio/b'));
?>
--CLEAN--
<?php unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
--EXPECTF--
array(26) {
  [0]=>
  int(12)
  [1]=>
  int(2090329158)
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
  ["dev"]=>
  int(12)
  ["ino"]=>
  int(2090329158)
  ["mode"]=>
  int(33060)
  ["nlink"]=>
  int(1)
  ["uid"]=>
  int(0)
  ["gid"]=>
  int(0)
  ["rdev"]=>
  int(-1)
  ["size"]=>
  int(1)
  ["atime"]=>
  int(%d)
  ["mtime"]=>
  int(%d)
  ["ctime"]=>
  int(%d)
  ["blksize"]=>
  int(-1)
  ["blocks"]=>
  int(-1)
}
array(26) {
  [0]=>
  int(12)
  [1]=>
  int(2090329108)
  [2]=>
  int(16676)
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
  int(0)
  [9]=>
  int(0)
  [10]=>
  int(0)
  [11]=>
  int(-1)
  [12]=>
  int(-1)
  ["dev"]=>
  int(12)
  ["ino"]=>
  int(2090329108)
  ["mode"]=>
  int(16676)
  ["nlink"]=>
  int(1)
  ["uid"]=>
  int(0)
  ["gid"]=>
  int(0)
  ["rdev"]=>
  int(-1)
  ["size"]=>
  int(0)
  ["atime"]=>
  int(0)
  ["mtime"]=>
  int(0)
  ["ctime"]=>
  int(0)
  ["blksize"]=>
  int(-1)
  ["blocks"]=>
  int(-1)
}