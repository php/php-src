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
$file = b"<?php
Phar::mapPhar('hio');
__HALT_COMPILER(); ?>";

$files = array();
$files['a/x'] = 'a';
include 'files/phar_test.inc';
include $fname;

echo file_get_contents($pname . '/a/x') . "\n";
var_dump(rmdir($pname . '/a'));
echo file_get_contents($pname . '/a/x') . "\n";
unlink($pname . '/a/x');
var_dump(rmdir($pname . '/a'));
?>
--CLEAN--
<?php unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
--EXPECTF--
a

Warning: rmdir(): phar error: Directory not empty in %srmdir.php on line 14
bool(false)
a
bool(true)
