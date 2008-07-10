--TEST--
Phar: rmdir test
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.readonly=0
phar.require_hash=0
--FILE--
<?php
$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php';
$pname = 'phar://' . $fname;
$file = "<?php
Phar::mapPhar('hio');
__HALT_COMPILER(); ?>";

$files = array();
$files['a/x'] = 'a';
include 'files/phar_test.inc';
include $fname;

echo file_get_contents($pname . '/a/x') . "\n";
rmdir($pname . '/a');
echo file_get_contents($pname . '/a/x') . "\n";
?>
--CLEAN--
<?php unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
--EXPECTF--
a

Warning: file_get_contents(phar://%srename.phar.php/a/x): failed to open stream: phar error: "a" is not a file in phar "%srename.phar.php" in %srename.php on line %d