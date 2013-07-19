--TEST--
Bug #65291 - get_defined_constants() causes PHP to crash in a very limited case.
--FILE--
<?php

trait TestTrait
{
	public static function testStaticFunction()
	{
		return __CLASS__;
	}
}
class Tester
{
	use TestTrait;
}

$foo = Tester::testStaticFunction();
get_defined_constants();

echo $foo;
?>
--EXPECT--
Tester
