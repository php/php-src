--TEST--
Phar: create a completely new phar
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
<?php if (!extension_loaded("spl")) die("skip SPL not available"); ?>
--INI--
phar.readonly=0
phar.require_hash=1
--FILE--
<?php

file_put_contents('phar://' . dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php/a.php',
	'brand new!');

$phar = new Phar(dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php');

var_dump($phar->getSignature());
?>
===DONE===
--CLEAN--
<?php unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
--EXPECTF--
array(2) {
  ["hash"]=>
  string(40) "%s"
  ["hash_type"]=>
  string(5) "SHA-1"
}
===DONE===
