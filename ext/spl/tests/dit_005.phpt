--TEST--
SPL: FilesystemIterator and clone
--FILE--
<?php
// Let's hope nobody writes into this directory while testing...
$a = new FileSystemIterator(__DIR__ . '/..');
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
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
===DONE===
