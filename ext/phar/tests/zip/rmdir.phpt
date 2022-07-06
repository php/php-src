--TEST--
Phar: rmdir test zip-based
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.readonly=0
phar.require_hash=0
--FILE--
<?php

$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar.zip';
$alias = 'phar://' . $fname;

$phar = new Phar($fname);
$phar->setStub("<?php
Phar::mapPhar('hio');
__HALT_COMPILER(); ?>");
$phar->addEmptyDir('a');
$phar['a/x'] = 'a';
$phar->stopBuffering();

include $fname;

echo file_get_contents($alias . '/a/x') . "\n";
var_dump(rmdir($alias . '/a'));
echo file_get_contents($alias . '/a/x') . "\n";
unlink($alias . '/a/x');
var_dump(rmdir($alias . '/a'));
?>
--CLEAN--
<?php unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar.zip'); ?>
--EXPECTF--
a

Warning: rmdir(): phar error: Directory not empty in %srmdir.php on line %d
bool(false)
a
bool(true)
