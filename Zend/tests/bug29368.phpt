--TEST--
Bug #29368 (The destructor is called when an exception is thrown from the constructor)
--FILE--
<?php

class Foo
{
	function __construct()
	{
		echo __METHOD__ . "\n";
		throw new Exception;
	}
	function __destruct()
	{
		echo __METHOD__ . "\n";
	}
}

try
{
	$bar = new Foo;
} catch(Exception $exc)
{
	echo "Caught exception!\n";
}

unset($bar);

?>
===DONE===
--EXPECTF--
Foo::__construct
Caught exception!
===DONE===
