--TEST--
Bug #30725 (PHP segfaults when an exception is thrown in getIterator() within foreach)
--FILE--
<?php

class Test implements IteratorAggregate
{
	function getIterator()
	{
		throw new Exception();
	}
}

try
{
	$it = new Test;
	foreach($it as $v)
	{
		echo "Fail\n";
	}
	echo "Wrong\n";
}
catch(Exception $e)
{
	echo "Caught\n";
}

?>
===DONE===
--EXPECT--
Caught
===DONE===
