--TEST--
Phar::setStub()
--SKIPIF--
<?php if (!extension_loaded("phar")) print "skip"; ?>
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php
$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php';
$p = new Phar($fname);
$p['a'] = '<?php include "b.php";';
?>
===DONE===
--CLEAN--
<?php 
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.php');
__HALT_COMPILER();
?>
--EXPECT--
===DONE===
