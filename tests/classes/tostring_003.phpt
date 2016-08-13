--TEST--
ZE2 __toString() in __destruct/exception
--FILE--
<?php

class Test
{
	function __toString()
	{
		throw new Exception("Damn!");
		return "Hello\n";
	}
	
	function __destruct()
	{
		echo $this;
	}
}

try
{
	$o = new Test;
	$o = NULL;
}
catch(Exception $e)
{
	var_dump($e->getMessage());
}

?>
====DONE====
--EXPECTF--
Fatal error: Method Test::__toString() must not throw an exception, caught Exception: Damn! in %stostring_003.php on line %d
