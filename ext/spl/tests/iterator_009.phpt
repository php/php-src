--TEST--
SPL: EmptyIterator
--SKIPIF--
<?php if (!extension_loaded("spl")) print "skip"; ?>
--FILE--
<?php

class EmptyIteratorEx extends EmptyIterator
{
	function rewind()
	{
		echo __METHOD__ . "\n";
		parent::rewind();
	}
	function valid()
	{
		echo __METHOD__ . "\n";
		return parent::valid();
	}
	function current()
	{
		echo __METHOD__ . "\n";
		return parent::current();
	}
	function key()
	{
		echo __METHOD__ . "\n";
		return parent::key();
	}
	function next()
	{
		echo __METHOD__ . "\n";
		parent::next();
	}
}

foreach (new EmptyIteratorEx() as $v) {
	var_dump($v);
}

?>
===DONE===
<?php exit(0); ?>
--EXPECT--
EmptyIteratorEx::rewind
EmptyIteratorEx::valid
===DONE===
