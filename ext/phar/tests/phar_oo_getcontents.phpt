--TEST--
Phar object: getContents()
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
<?php if (!extension_loaded("spl")) die("skip SPL not available"); ?>
--INI--
phar.readonly=0
--FILE--
<?php
$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php';

$phar = new Phar($fname);
$phar['a'] = 'file contents
this works';
echo $phar['a']->getContents() . "\n";
?>
===DONE===
--CLEAN--
<?php 
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.php');
__halt_compiler();
?>
--EXPECT--
file contents
this works
===DONE===