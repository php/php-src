--TEST--
SPL: RecursiveFilterIterator
--SKIPIF--
<?php if (!extension_loaded("spl")) print "skip"; ?>
--FILE--
<?php

class MyRecursiveFilterIterator extends RecursiveFilterIterator
{
	function accept()
	{
		return true;
	}
}

$ar = array(1, array(21, 22), 3);
$it = new RecursiveArrayIterator($ar);
$it = new MyRecursiveFilterIterator($it);
$it = new RecursiveIteratorIterator($it);

foreach($it as $k => $v)
{
	echo "===\n";
	var_dump($it->getDepth());
	var_dump($k);
	var_dump($v);
}

?>
===DONE===
<?php exit(0); ?>
--EXPECT--
===
int(0)
int(0)
int(1)
===
int(1)
int(0)
int(21)
===
int(1)
int(1)
int(22)
===
int(0)
int(2)
int(3)
===DONE===
