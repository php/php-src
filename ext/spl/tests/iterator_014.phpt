--TEST--
SPL: RecursiveIteratorIterator and beginChildren/endChildren
--FILE--
<?php

class MyRecursiveArrayIterator extends RecursiveArrayIterator
{
	function valid()
	{
		if (!parent::valid())
		{
			echo __METHOD__ . " = false\n";
			return false;
		}
		else
		{
			return true;
		}
	}

	function getChildren()
	{
		echo __METHOD__ . "\n";
		return parent::getChildren();
	}
}

class RecursiveArrayIteratorIterator extends RecursiveIteratorIterator
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

	function beginChildren()
	{
		echo __METHOD__ . "(".$this->getDepth().")\n";
	}

	function endChildren()
	{
		echo __METHOD__ . "(".$this->getDepth().")\n";
	}
}

foreach(new RecursiveArrayIteratorIterator(new MyRecursiveArrayIterator(array("a", array("ba", array("bba", "bbb"), array(array("bcaa"))), array("ca"), "d"))) as $k=>$v)
{
	echo "$k=>$v\n";
}
?>
===DONE===
<?php exit(0); ?>
--EXPECT--
RecursiveArrayIteratorIterator::rewind
RecursiveArrayIteratorIterator::valid
RecursiveArrayIteratorIterator::current
RecursiveArrayIteratorIterator::key
0=>a
RecursiveArrayIteratorIterator::next
MyRecursiveArrayIterator::getChildren
RecursiveArrayIteratorIterator::beginChildren(1)
RecursiveArrayIteratorIterator::valid
RecursiveArrayIteratorIterator::current
RecursiveArrayIteratorIterator::key
0=>ba
RecursiveArrayIteratorIterator::next
MyRecursiveArrayIterator::getChildren
RecursiveArrayIteratorIterator::beginChildren(2)
RecursiveArrayIteratorIterator::valid
RecursiveArrayIteratorIterator::current
RecursiveArrayIteratorIterator::key
0=>bba
RecursiveArrayIteratorIterator::next
RecursiveArrayIteratorIterator::valid
RecursiveArrayIteratorIterator::current
RecursiveArrayIteratorIterator::key
1=>bbb
RecursiveArrayIteratorIterator::next
MyRecursiveArrayIterator::valid = false
RecursiveArrayIteratorIterator::endChildren(2)
MyRecursiveArrayIterator::getChildren
RecursiveArrayIteratorIterator::beginChildren(2)
MyRecursiveArrayIterator::getChildren
RecursiveArrayIteratorIterator::beginChildren(3)
RecursiveArrayIteratorIterator::valid
RecursiveArrayIteratorIterator::current
RecursiveArrayIteratorIterator::key
0=>bcaa
RecursiveArrayIteratorIterator::next
MyRecursiveArrayIterator::valid = false
RecursiveArrayIteratorIterator::endChildren(3)
MyRecursiveArrayIterator::valid = false
RecursiveArrayIteratorIterator::endChildren(2)
MyRecursiveArrayIterator::valid = false
RecursiveArrayIteratorIterator::endChildren(1)
MyRecursiveArrayIterator::getChildren
RecursiveArrayIteratorIterator::beginChildren(1)
RecursiveArrayIteratorIterator::valid
RecursiveArrayIteratorIterator::current
RecursiveArrayIteratorIterator::key
0=>ca
RecursiveArrayIteratorIterator::next
MyRecursiveArrayIterator::valid = false
RecursiveArrayIteratorIterator::endChildren(1)
RecursiveArrayIteratorIterator::valid
RecursiveArrayIteratorIterator::current
RecursiveArrayIteratorIterator::key
3=>d
RecursiveArrayIteratorIterator::next
MyRecursiveArrayIterator::valid = false
RecursiveArrayIteratorIterator::valid
MyRecursiveArrayIterator::valid = false
===DONE===
