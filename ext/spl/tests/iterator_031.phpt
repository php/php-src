--TEST--
SPL: AppendIterator::append() rewinds when necessary
--FILE--
<?php

class MyArrayIterator extends ArrayIterator
{
	function rewind()
	{
		echo __METHOD__ . "\n";
		parent::rewind();
	}
}

$it = new MyArrayIterator(array(1,2));

foreach($it as $k=>$v)
{
	echo "$k=>$v\n";
}

class MyAppendIterator extends AppendIterator
{
	function __construct()
	{
		echo __METHOD__ . "\n";
	}

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
	
	function append(Iterator $what)
	{
		echo __METHOD__ . "\n";
		parent::append($what);
	}
	
	function parent__construct()
	{
		parent::__construct();
	}
}

$ap = new MyAppendIterator;

try
{
	$ap->append($it);
}
catch(LogicException $e)
{
	echo $e->getMessage() . "\n";
}

$ap->parent__construct();

try
{
	$ap->parent__construct($it);
}
catch(BadMethodCallException $e)
{
	echo $e->getMessage() . "\n";
}

$ap->append($it);
$ap->append($it);
$ap->append($it);

foreach($ap as $k=>$v)
{
	echo "$k=>$v\n";
}

?>
===DONE===
<?php exit(0); ?>
--EXPECT--
MyArrayIterator::rewind
0=>1
1=>2
MyAppendIterator::__construct
MyAppendIterator::append
The object is in an invalid state as the parent constructor was not called
AppendIterator::getIterator() must be called exactly once per instance
MyAppendIterator::append
MyArrayIterator::rewind
MyAppendIterator::append
MyAppendIterator::append
MyAppendIterator::rewind
MyArrayIterator::rewind
MyAppendIterator::valid
0=>1
MyAppendIterator::valid
1=>2
MyArrayIterator::rewind
MyAppendIterator::valid
0=>1
MyAppendIterator::valid
1=>2
MyArrayIterator::rewind
MyAppendIterator::valid
0=>1
MyAppendIterator::valid
1=>2
MyAppendIterator::valid
===DONE===
