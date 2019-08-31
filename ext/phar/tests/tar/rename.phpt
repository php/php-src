--TEST--
Phar: rename test tar-based
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.readonly=0
phar.require_hash=0
--FILE--
<?php
include __DIR__ . '/files/tarmaker.php.inc';
$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar.tar';
$alias = 'phar://' . $fname;

$tar = new tarmaker($fname, 'none');
$tar->init();
$tar->addFile('.phar/stub.php', "<?php
Phar::mapPhar('hio');
__HALT_COMPILER(); ?>");

$files = array();
$files['a'] = 'a';

foreach ($files as $n => $file) {
	$tar->addFile($n, $file);
}

$tar->close();

include $fname;

echo file_get_contents($alias . '/a') . "\n";
rename($alias . '/a', $alias . '/b');
echo file_get_contents($alias . '/b') . "\n";
echo file_get_contents($alias . '/a') . "\n";
?>
--CLEAN--
<?php unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar.tar'); ?>
--EXPECTF--
a
a

Warning: file_get_contents(phar://%srename.phar.tar/a): failed to open stream: phar error: "a" is not a file in phar "%srename.phar.tar" in %srename.php on line %d
