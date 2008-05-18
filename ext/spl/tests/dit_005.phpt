--TEST--
SPL: FilesystemIterator and clone
--SKIPIF--
<?php if (!extension_loaded("spl")) print "skip"; ?>
--FILE--
<?php
$a = new FileSystemIterator(__DIR__);
$b = clone $a;
var_dump((string)$b == (string)$a);
var_dump($a->key() == $b->key());
$a->next();
$a->next();
$a->next();
$c = clone $a;
var_dump((string)$c == (string)$a);
var_dump($a->key() == $c->key());
?>
===DONE===
--EXPECTF--
bool(true)
bool(true)
bool(true)
bool(true)
===DONE===
