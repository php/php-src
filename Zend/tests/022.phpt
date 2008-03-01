--TEST--
Implementating abstracting methods and optional parameters
--FILE--
<?php

abstract class Base
{
	abstract function someMethod($param);
}

class Ext extends Base
{
	function someMethod($param = "default")
	{
		echo $param, "\n";
	}
}

$a = new Ext();
$a->someMethod("foo");
$a->someMethod();
--EXPECT--
foo
default
