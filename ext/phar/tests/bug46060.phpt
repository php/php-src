--TEST--
Phar: Bug #46060: addEmptyDir() breaks
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
<?php if (getenv('SKIP_SLOW_TESTS')) die('skip'); ?>
--INI--
phar.require_hash=0
--FILE--
<?php
$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.tar';
$fname2 = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.2.tar';

$phar = new PharData($fname);
$phar->addEmptyDir('blah/');
$phar->addFromString('test/', '');

copy($fname, $fname2);
$phar = new PharData($fname2);

var_dump($phar['blah']->isDir(), $phar['test']->isDir());
?>
===DONE===
--CLEAN--
<?php 
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.tar');
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.2.tar');
__HALT_COMPILER();
?>
--EXPECT--
bool(true)
bool(false)
===DONE===
