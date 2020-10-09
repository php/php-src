--TEST--
Phar::mapPhar filesize mismatch
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip");?>
--INI--
phar.require_hash=0
--FILE--
<?php
$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar.php';
$pname = 'phar://' . $fname;
$file = "<?php __HALT_COMPILER(); ?>";
// wrong crc32

$files = array();
$files['a'] = array('cont'=>'a', 'crc32'=>crc32('aX'));
include 'files/phar_test.inc';

echo file_get_contents($pname.'/a');
?>
--CLEAN--
<?php unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
--EXPECTF--
Warning: file_get_contents(phar://%s/a): Failed to open stream: phar error: internal corruption of phar "%s" (crc32 mismatch on file "a") in %s on line %d
