--TEST--
Phar: rename test zip-based
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.readonly=0
phar.require_hash=0
--FILE--
<?php
include dirname(__FILE__) . '/tarmaker.php.inc';
$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php';
$pname = 'phar://' . $fname;
$a = new tarmaker($fname, 'none');
$a->init();
$a->addFile('.phar/stub.php', "<?php
Phar::mapPhar('hio');
__HALT_COMPILER(); ?>");

$files = array();
$files['a'] = 'a';
foreach ($files as $n => $file) {
$a->addFile($n, $file);
}
$a->close();

include $fname;

echo file_get_contents($pname . '/a') . "\n";
rename($pname . '/a', $pname . '/b');
echo file_get_contents($pname . '/b') . "\n";
echo file_get_contents($pname . '/a') . "\n";
?>
--CLEAN--
<?php unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
--EXPECTF--
a
a

Warning: file_get_contents(phar://%srename.phar.php/a): failed to open stream: phar error: "a" is not a file in phar "%srename.phar.php" in %srename.php on line %d