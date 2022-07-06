--TEST--
Phar: delete test
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.readonly=0
phar.require_hash=0
--FILE--
<?php
$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar.php';
$pname = 'phar://' . $fname;
$file = "<?php
Phar::mapPhar('hio');
__HALT_COMPILER(); ?>";

$files = array();
$files['a'] = 'a';
include 'files/phar_test.inc';
include $fname;
$phar = new Phar($fname);

echo file_get_contents($pname . '/a') . "\n";
$phar->delete('a');
echo file_get_contents($pname . '/a') . "\n";
?>
--CLEAN--
<?php unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
--EXPECTF--
a

Warning: file_get_contents(phar://%sdelete.phar.php/a): Failed to open stream: phar error: "a" is not a file in phar "%sdelete.phar.php" in %sdelete.php on line 16
