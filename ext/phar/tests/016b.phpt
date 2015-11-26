--TEST--
Phar::mapPhar invalid file (gzipped file length is too short)
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip");?>
<?php if (!extension_loaded("zlib")) die("skip zlib not present"); ?>
--INI--
phar.require_hash=0
--FILE--
<?php
$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php';
$pname = 'phar://' . $fname;
$file = "<?php __HALT_COMPILER(); ?>";
// file length is too short

$files = array();
$files['a'] = array('cont'=>'a','flags'=>0x00001000, 'clen' => 1);
include 'files/phar_test.inc';

echo file_get_contents($pname . '/a');
?>
--CLEAN--
<?php unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
--EXPECTF--
Warning: file_get_contents(phar://%s/a): failed to open stream: phar error: internal corruption of phar "%s" (actual filesize mismatch on file "a") in %s on line %d
