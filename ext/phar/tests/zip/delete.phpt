--TEST--
Phar: delete test, zip-based phar
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.readonly=0
phar.require_hash=0
--FILE--
<?php

$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.zip';
$alias = 'phar://' . $fname;
$file = "<?php
Phar::mapPhar('hio');
__HALT_COMPILER(); ?>";

$phar = new Phar($fname);
$phar['a'] = 'a';
$phar->setStub($file);
$phar->stopBuffering();

echo file_get_contents($alias . '/a') . "\n";
$phar->delete('a');
echo file_get_contents($alias . '/a') . "\n";
?>
--CLEAN--
<?php unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.zip'); ?>
--EXPECTF--
a

Warning: file_get_contents(phar://%sdelete.phar.zip/a): failed to open stream: phar error: "a" is not a file in phar "%sdelete.phar.zip" in %sdelete.php on line %d
