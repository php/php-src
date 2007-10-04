--TEST--
Phar::mapPhar valid file (gzipped)
--SKIPIF--
<?php if (!extension_loaded("phar")) print "skip"; ?>
<?php if (!extension_loaded("zlib")) print "skip zlib not present"; ?>
--INI--
phar.require_hash=0
--FILE--
<?php
$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php';
$pname = 'phar://' . $fname;
$file = "<?php __HALT_COMPILER(); ?>";

$files = array();
$files['a'] = array('cont'=>'a','comp'=>chr(75) . chr(4) . chr(0) /* 'a' gzdeflated */, 'flags'=>0x00001000);
include 'phar_test.inc';

echo file_get_contents($pname .'/a');
?>
--CLEAN--
<?php unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
--EXPECT--
a
