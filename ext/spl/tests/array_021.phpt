--TEST--
SPL: ArrayObject::seek() and exceptions
--SKIPIF--
<?php if (!extension_loaded("spl")) print "skip"; ?>
--FILE--
<?php

class foo extends ArrayObject
{
	public function seek($key)
	{
		echo __METHOD__ . "($key)\n";
		throw new Exception("hi");
	}
}

$test = new foo(array(1,2,3));

try
{
	$test->seek('bar');
}
catch (Exception $e)
{
	echo "got exception\n";
}

?>
===DONE===
--EXPECT--	
foo::seek(bar)
got exception
===DONE===
