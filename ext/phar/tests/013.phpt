--TEST--
Phar::mapPhar filesize mismatch
--EXTENSIONS--
phar
--INI--
phar.require_hash=0
--FILE--
<?php
$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar.php';
$pname = 'phar://' . $fname;
$file = "<?php __HALT_COMPILER(); ?>";
// filesize should be 1, and is 2

$files = array();
$files['a'] = array('cont'=>'a', 'ulen'=>2, 'clen'=>2);
include 'files/phar_test.inc';

echo file_get_contents($pname.'/a');
?>
--CLEAN--
<?php unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
--EXPECTF--
Warning: file_get_contents(phar://%s/a): Failed to open stream: phar error: internal corruption of phar "%s" (%s file "a") in %s on line %d
