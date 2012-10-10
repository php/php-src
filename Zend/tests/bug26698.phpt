--TEST--
Bug #26698 (Thrown exceptions while evaluting argument to pass as parameter crash PHP)
--FILE--
<?php

ini_set("report_memleaks", 0);  // the exception thrown in this test results in a memory leak, which is fine

class Object
{
	function getNone()
	{
		throw new Exception('NONE');
	}
}

class Proxy
{
	function three($a, $b, $c)
	{
	}

	function callOne()
	{
		try
		{
			$res = new Object();
			$this->three($res->getNone());
		}
		catch(Exception $e)
		{
			echo 'Caught: '.$e->getMessage()."\n";
		}
	}

	function callTwo()
	{
		try
		{
			$res = new Object();
			$this->three(1, $res->getNone());
		}
		catch(Exception $e)
		{
			echo 'Caught: '.$e->getMessage()."\n";
		}
	}

	function callThree()
	{
		try
		{
			$res = new Object();
			$this->three(1, 2, $res->getNone());
		}
		catch(Exception $e)
		{
			echo 'Caught: '.$e->getMessage()."\n";
		}
	}
}

$p = new Proxy();

$p->callOne();
$p->callTwo();
$p->callThree();
?>
===DONE===
--EXPECT--
Caught: NONE
Caught: NONE
Caught: NONE
===DONE===
