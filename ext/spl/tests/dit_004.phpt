--TEST--
SPL: DirectoryIterator and clone
--FILE--
<?php
$a = new DirectoryIterator(__DIR__);
$b = clone $a;
var_dump((string)$b == (string)$a);
var_dump($a->key(), $b->key());
$a->next();
$a->next();
$a->next();
$c = clone $a;
var_dump((string)$c == (string)$a);
var_dump($a->key(), $c->key());
?>
===DONE===
--EXPECTF--
bool(true)
int(0)
int(0)
bool(true)
int(3)
int(3)
===DONE===
