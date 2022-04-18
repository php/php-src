--TEST--
Phar: create a completely new phar
--EXTENSIONS--
phar
--INI--
phar.readonly=0
phar.require_hash=1
--FILE--
<?php

file_put_contents('phar://' . __DIR__ . '/' . basename(__FILE__, '.php') . '.phar.php/a.php',
    'brand new!');

$phar = new Phar(__DIR__ . '/' . basename(__FILE__, '.php') . '.phar.php');

var_dump($phar->getSignature());
?>
--CLEAN--
<?php unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
--EXPECTF--
array(2) {
  ["hash"]=>
  string(64) "%s"
  ["hash_type"]=>
  string(7) "SHA-256"
}
