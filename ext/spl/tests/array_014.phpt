--TEST--
SPL: ArrayIterator::seek()
--SKIPIF--
<?php if (!extension_loaded("spl")) print "skip"; ?>
--FILE--
<?php

$it = new ArrayIterator(range(0,10));
var_dump($it->count());
$it->seek(5);
var_dump($it->current());
$it->seek(4);
var_dump($it->current());
$it->seek(-1);
var_dump($it->current());
$it->seek(12);
var_dump($it->current());

$pos = 0;
foreach($it as $v)
{
	$it->seek($pos++);
	var_dump($v);
}

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
int(11)
int(5)
int(4)
int(0)
NULL
int(0)
int(1)
int(2)
int(3)
int(4)
int(5)
int(6)
int(7)
int(8)
int(9)
int(10)
===DONE===
