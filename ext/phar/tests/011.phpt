--TEST--
Phar::mapPhar filesize too small in manifest
--SKIPIF--
<?php if (!extension_loaded("phar")) print "skip";?>
--INI--
phar.require_hash=0
--FILE--
<?php
$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php';
$pname = 'phar://' . $fname;
$file = "<?php
Phar::mapPhar('hio');
__HALT_COMPILER(); ?>";

// compressed file length does not match incompressed lentgh for an uncompressed file

$files = array();
$files['a'] = array('cont'=>'a','ulen'=>1,'clen'=>2);;
include 'phar_test.inc';

include $fname;
echo file_get_contents('phar://hio/a');
?>
--CLEAN--
<?php unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
--EXPECTF--
%satal error: Phar::mapPhar(): internal corruption of phar "%s" (compressed and uncompressed size does not match for uncompressed entry) in %s on line %d
