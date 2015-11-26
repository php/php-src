--TEST--
Phar: rename test zip-based
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
$phar['a'] = 'a';
$phar->stopBuffering();

include $fname;

echo file_get_contents($alias . '/a') . "\n";
rename($alias . '/a', $alias . '/b');
echo file_get_contents($alias . '/b') . "\n";
echo file_get_contents($alias . '/a') . "\n";
?>
--CLEAN--
<?php unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.zip'); ?>
--EXPECTF--
a
a

Warning: file_get_contents(phar://%srename.phar.zip/a): failed to open stream: phar error: "a" is not a file in phar "%srename.phar.zip" in %srename.php on line %d