--TEST--
Bug #36941 (ArrayIterator does not clone itself)
--FILE--
===ArrayObject===
<?php
$a = new ArrayObject();
$a[] = 1;

$b = clone $a;

var_dump($a[0], $b[0]);
$b[0] = $b[0] + 1;
var_dump($a[0], $b[0]);
$b[0] = 3;
var_dump($a[0], $b[0]);
?>
===ArrayIterator===
<?php
$a = new ArrayIterator();
$a[] = 1;

$b = clone $a;

var_dump($a[0], $b[0]);
$b[0] = $b[0] + 1;
var_dump($a[0], $b[0]);
$b[0] = 3;
var_dump($a[0], $b[0]);
?>
--EXPECT--
===ArrayObject===
int(1)
int(1)
int(1)
int(2)
int(1)
int(3)
===ArrayIterator===
int(1)
int(1)
int(2)
int(2)
int(3)
int(3)
