--TEST--
Phar: create a completely new phar
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.readonly=0
phar.require_hash=1
--FILE--
<?php

file_put_contents('phar://' . dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php/a.php',
	'brand new!');
include 'phar://' . dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php/a.php';
?>

===DONE===
--CLEAN--
<?php unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
--EXPECT--
brand new!
===DONE===
