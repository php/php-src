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

Fatal error: Uncaught Error: Undefined constant 'Baz' in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
