--TEST--
SPL: ArrayIterator::seek()
--FILE--
<?php

$it = new ArrayIterator(range(0,10));
var_dump($it->count());
$it->seek(5);
var_dump($it->current());
$it->seek(4);
var_dump($it->current());
try
{
	$it->seek(-1);
	var_dump($it->current());
}
catch(Exception $e)
{
	echo $e->getMessage() . "\n";
}

try
{
	$it->seek(12);
	var_dump($it->current());
}
catch(Exception $e)
{
	echo $e->getMessage() . "\n";
}

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
Seek position -1 is out of range
Seek position 12 is out of range
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
