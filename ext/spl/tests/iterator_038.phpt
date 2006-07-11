--TEST--
SPL: RoRewindIterator and string keys
--SKIPIF--
<?php if (!extension_loaded("spl")) print "skip"; ?>
--FILE--
<?php

foreach(new NoRewindIterator(new ArrayIterator(array('Hello'=>0, 'World'=>1))) as $k => $v)
{
	var_dump($v);
	var_dump($k);
}

?>
===DONE===
--EXPECT--
int(0)
string(5) "Hello"
int(1)
string(5) "World"
===DONE===
