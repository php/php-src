--TEST--
Phar: rename_dir test zip-based
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.readonly=0
phar.require_hash=0
--FILE--
<?php

$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.zip';
$alias = 'phar://' . $fname;

$phar = new Phar($fname);
$phar->setStub("<?php
Phar::mapPhar('hio');
__HALT_COMPILER(); ?>");
$phar['a/x'] = 'a';
$phar->stopBuffering();

include $fname;

echo file_get_contents($alias . '/a/x') . "\n";
rename($alias . '/a', $alias . '/b');
echo file_get_contents($alias . '/b/x') . "\n";
echo file_get_contents($alias . '/a/x') . "\n";
?>
--CLEAN--
<?php unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.zip'); ?>
--EXPECTF--
a
a

Warning: file_get_contents(phar://%srename_dir.phar.zip/a/x): failed to open stream: phar error: "a/x" is not a file in phar "%srename_dir.phar.zip" in %srename_dir.php on line %d
