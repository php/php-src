--TEST--
Phar::convertToPhar() from tar
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php
$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php';
$pname = 'phar://' . $fname;
$fname2 = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.2.phar.php';
$pname2 = 'phar://' . $fname;

$phar = new Phar($fname);
$phar->convertToTar();
var_dump($phar->isTar());
$phar['a'] = 'hi there';
$phar->convertToPhar();
var_dump($phar->isPhar());
copy($fname, $fname2);

$phar = new Phar($fname2);
var_dump($phar->isPhar());
?>
===DONE===
--CLEAN--
<?php 
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.php');
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.2.phar.php');
__HALT_COMPILER();
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
===DONE===
