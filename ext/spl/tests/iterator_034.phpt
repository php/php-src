--TEST--
SPL: RecursiveIteratorIterator and break deep
--FILE--
<?php

class MyRecursiveArrayIterator extends RecursiveArrayIterator
{
	function valid()
	{
		if (!parent::valid())
		{
			echo __METHOD__ . "() = false\n";
			return false;
		}
		else
		{
			return true;
		}
	}

	function getChildren()
	{
		echo __METHOD__ . "()\n";
		return parent::getChildren();
	}

	function rewind()
	{
		echo __METHOD__ . "()\n";
		parent::rewind();
	}
}

class RecursiveArrayIteratorIterator extends RecursiveIteratorIterator
{
	private $max_depth;
	private $over = 0;

	function __construct($it, $max_depth)
	{
		$this->max_depth = $max_depth;
		parent::__construct($it);
	}

	function rewind()
	{
		echo __METHOD__ . "() - BEGIN\n";
		parent::rewind();
		echo __METHOD__ . "() - DONE\n";
	}

	function valid()
	{
		echo __METHOD__ . "()\n";
		return parent::valid();
	}

	function current()
	{
		echo __METHOD__ . "()\n";
		return parent::current();
	}

	function key()
	{
		echo __METHOD__ . "()\n";
		return parent::key();
	}

	function next()
	{
		echo __METHOD__ . "()\n";
		parent::next();
	}

	function callHasChildren()
	{
		$has = parent::callHasChildren();
		$res = $this->getDepth() < $this->max_depth && $has;
		echo __METHOD__ . "(".$this->getDepth().") = ".($res?"yes":"no")."/".($has?"yes":"no")."\n";
		return $res;
	}

	function beginChildren()
	{
		echo __METHOD__ . "(".$this->getDepth().")\n";
		parent::beginChildren();
	}

	function endChildren()
	{
		echo __METHOD__ . "(".$this->getDepth().")\n";
		parent::endChildren();
	}
}

$p = 0;
$it = new RecursiveArrayIteratorIterator(new MyRecursiveArrayIterator(array("a", array("ba", array("bba", "bbb"), array(array("bcaa"), array("bcba"))), array("ca"), "d")), 2);
foreach($it as $k=>$v)
{
	if (is_array($v)) $v = join('',$v);
	echo "$k=>$v\n";
	if ($p++ == 5)
	{
		echo "===BREAK===\n";
		break;
	}
}

echo "===FOREND===\n";

$it->rewind();

echo "===CHECK===\n";

var_dump($it->valid());
var_dump($it->current() == "a");

?>
===DONE===
<?php exit(0); ?>
--EXPECT--
RecursiveArrayIteratorIterator::rewind() - BEGIN
MyRecursiveArrayIterator::rewind()
RecursiveArrayIteratorIterator::callHasChildren(0) = no/no
RecursiveArrayIteratorIterator::rewind() - DONE
RecursiveArrayIteratorIterator::valid()
RecursiveArrayIteratorIterator::current()
RecursiveArrayIteratorIterator::key()
0=>a
RecursiveArrayIteratorIterator::next()
RecursiveArrayIteratorIterator::callHasChildren(0) = yes/yes
MyRecursiveArrayIterator::getChildren()
MyRecursiveArrayIterator::rewind()
RecursiveArrayIteratorIterator::beginChildren(1)
RecursiveArrayIteratorIterator::callHasChildren(1) = no/no
RecursiveArrayIteratorIterator::valid()
RecursiveArrayIteratorIterator::current()
RecursiveArrayIteratorIterator::key()
0=>ba
RecursiveArrayIteratorIterator::next()
RecursiveArrayIteratorIterator::callHasChildren(1) = yes/yes
MyRecursiveArrayIterator::getChildren()
MyRecursiveArrayIterator::rewind()
RecursiveArrayIteratorIterator::beginChildren(2)
RecursiveArrayIteratorIterator::callHasChildren(2) = no/no
RecursiveArrayIteratorIterator::valid()
RecursiveArrayIteratorIterator::current()
RecursiveArrayIteratorIterator::key()
0=>bba
RecursiveArrayIteratorIterator::next()
RecursiveArrayIteratorIterator::callHasChildren(2) = no/no
RecursiveArrayIteratorIterator::valid()
RecursiveArrayIteratorIterator::current()
RecursiveArrayIteratorIterator::key()
1=>bbb
RecursiveArrayIteratorIterator::next()
MyRecursiveArrayIterator::valid() = false
RecursiveArrayIteratorIterator::endChildren(2)
RecursiveArrayIteratorIterator::callHasChildren(1) = yes/yes
MyRecursiveArrayIterator::getChildren()
MyRecursiveArrayIterator::rewind()
RecursiveArrayIteratorIterator::beginChildren(2)
RecursiveArrayIteratorIterator::callHasChildren(2) = no/yes
RecursiveArrayIteratorIterator::valid()
RecursiveArrayIteratorIterator::current()
RecursiveArrayIteratorIterator::key()
0=>bcaa
RecursiveArrayIteratorIterator::next()
RecursiveArrayIteratorIterator::callHasChildren(2) = no/yes
RecursiveArrayIteratorIterator::valid()
RecursiveArrayIteratorIterator::current()
RecursiveArrayIteratorIterator::key()
1=>bcba
===BREAK===
===FOREND===
RecursiveArrayIteratorIterator::rewind() - BEGIN
RecursiveArrayIteratorIterator::endChildren(1)
RecursiveArrayIteratorIterator::endChildren(0)
MyRecursiveArrayIterator::rewind()
RecursiveArrayIteratorIterator::callHasChildren(0) = no/no
RecursiveArrayIteratorIterator::rewind() - DONE
===CHECK===
RecursiveArrayIteratorIterator::valid()
bool(true)
RecursiveArrayIteratorIterator::current()
bool(true)
===DONE===
