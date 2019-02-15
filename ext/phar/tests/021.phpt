--TEST--
Phar: stream stat
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.require_hash=0
--FILE--
<?php
$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php';
$pname = 'phar://' . $fname;
$file = "<?php
Phar::mapPhar('hio');
__HALT_COMPILER(); ?>";

$files = array();
$files['a'] = 'a';
$files['b/a'] = 'b';
$files['b/c/d'] = 'c';
$files['bad/c'] = 'd';

include 'files/phar_test.inc';

include $fname;

$fp = fopen('phar://hio/a', 'r');
var_dump(fstat($fp));
fclose($fp);
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
  [%sdev"]=>
  int(12)
  [%sino"]=>
  int(%d)
  [%smode"]=>
  int(33060)
  [%snlink"]=>
  int(1)
  [%suid"]=>
  int(0)
  [%sgid"]=>
  int(0)
  [%srdev"]=>
  int(-1)
  [%ssize"]=>
  int(1)
  [%satime"]=>
  int(%d)
  [%smtime"]=>
  int(%d)
  [%sctime"]=>
  int(%d)
  [%sblksize"]=>
  int(-1)
  [%sblocks"]=>
  int(-1)
}
