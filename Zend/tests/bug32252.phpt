--TEST--
Bug #32252 (Segfault when offsetSet throws an Exception (only without debug))
--FILE--
<?php

class Test implements ArrayAccess
{
	function offsetExists($offset)
	{
		echo __METHOD__ . "($offset)\n";
		return false;
	}

	function offsetGet($offset)
	{
		echo __METHOD__ . "($offset)\n";
		return null;
	}

	function offsetSet($offset, $value)
	{
		echo __METHOD__ . "($offset, $value)\n";
		throw new Exception("Ooops");
	}

	function offsetUnset($offset)
	{
		echo __METHOD__ . "($offset)\n";
	}
}

$list = new Test();
try
{
    $list[-1] = 123;
}
catch (Exception $e)
{
	echo "CAUGHT\n";
}

?>
===DONE===
--EXPECT--
Test::offsetSet(-1, 123)
CAUGHT
===DONE===
