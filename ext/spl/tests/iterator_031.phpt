--TEST--
SPL: AppendIterator::append() rewinds when neccessary
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

class MyAppendIterator2 extends AppendIterator
{
	function __construct()
	{
		echo __METHOD__ . "\n";
	}
}

class MyAppendIterator extends AppendIterator
{
	function __construct()
	{
		parent::__construct();
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

try
{
	$aperr = new MyAppendIterator2;

}
catch(Exception $e)
{
	echo $e->getMessage() . "\n";
}

$ap = new MyAppendIterator;

try
{
	$ap->parent__construct($it);
}
catch(Exception $e)
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
MyAppendIterator2::__construct
In the constructor of MyAppendIterator2, parent::__construct() must be called and its exceptions cannot be cleared
MyAppendIterator::__construct
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
