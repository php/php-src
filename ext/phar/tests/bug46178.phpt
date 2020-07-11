--TEST--
Phar: PHP bug #46178: "memory leak in ext/phar"
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php
$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar';

$phar = new Phar($fname);
$phar['long/path/name.txt'] = 'hi';
$phar->addEmptyDir('long/path');
?>
===DONE===
--CLEAN--
<?php unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar');?>
--EXPECT--
===DONE===
