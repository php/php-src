--TEST--
url stat
--SKIPIF--
<?php if (!extension_loaded("phar")) print "skip"; ?>
--FILE--
<?php
function cleanup() { unlink(dirname(__FILE__) . '/008_phar.php'); }
register_shutdown_function('cleanup');
$file = "<?php
PHP_Archive::mapPhar(5, 'hio', false);
__HALT_COMPILER(); ?>";
$manifest = '';
$manifest .= pack('V', 1) . 'a' . pack('VVVV', 1, time(), 0, 9);
$manifest .= pack('V', 3) . 'b/a' . pack('VVVV', 1, time(), 0, 9);
$manifest .= pack('V', 5) . 'b/c/d' . pack('VVVV', 1, time(), 0, 9);
$manifest .= pack('V', 5) . 'bad/c' . pack('VVVV', 1, time(), 0, 9);
$file .= pack('VV', strlen($manifest) + 4, 4) .
	 $manifest .
	 pack('VV', crc32('a'), 1) . 'a' .
	 pack('VV', crc32('b'), 1) . 'b';
	 pack('VV', crc32('c'), 1) . 'c';
	 pack('VV', crc32('d'), 1) . 'd';
file_put_contents(dirname(__FILE__) . '/008_phar.php', $file);
include dirname(__FILE__) . '/008_phar.php';
var_dump(stat('phar://hio/a'), stat('phar://hio/b'));
?>
--EXPECTF--
array(26) {
  [0]=>
  int(0)
  [1]=>
  int(0)
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
  int(0)
  ["ino"]=>
  int(0)
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
  int(0)
  [1]=>
  int(0)
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
  int(0)
  ["ino"]=>
  int(0)
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