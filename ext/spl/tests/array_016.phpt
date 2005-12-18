--TEST--
SPL: ArrayIterator/Object and IteratorIterator
--SKIPIF--
<?php if (!extension_loaded("spl")) print "skip"; ?>
--FILE--
<?php

$it = new ArrayIterator(range(0,3));

foreach(new IteratorIterator($it) as $v)
{
	var_dump($v);
}

$it = new ArrayObject(range(0,3));

foreach(new IteratorIterator($it) as $v)
{
	var_dump($v);
}

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
int(0)
int(1)
int(2)
int(3)
int(0)
int(1)
int(2)
int(3)
===DONE===
