--TEST--
SPL: DirectoryIterator and clone
--FILE--
<?php

@mkdir($dir = __DIR__ . '/dit_004');
touch($dir . '/file1');
touch($dir . '/file2');
touch($dir . '/file3');

$a = new DirectoryIterator($dir);
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
--CLEAN--
<?php
$dir = __DIR__ . '/dit_004';
unlink($dir . '/file1');
unlink($dir . '/file2');
unlink($dir . '/file3');
rmdir($dir);
?>
--EXPECT--
bool(true)
int(0)
int(0)
bool(true)
int(3)
int(3)
