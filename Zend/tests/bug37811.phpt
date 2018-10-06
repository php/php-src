--TEST--
Bug #37811 (define not using toString on objects)
--FILE--
<?php

class TestClass
{
	function __toString()
	{
		return "Foo";
	}
}

define("Bar",new TestClass);
var_dump(Bar);
define("Baz",new stdClass);
var_dump(Baz);

?>
===DONE===
--EXPECTF--
string(3) "Foo"

Warning: Constants may only evaluate to scalar values, arrays or resources in %sbug37811.php on line %d

Warning: Use of undefined constant Baz - assumed 'Baz' (this will throw an Error in a future version of PHP) in %sbug37811.php on line %d
string(3) "Baz"
===DONE===
