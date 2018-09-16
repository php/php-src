--TEST--
Bug #36011 (Strict errormsg wrong for call_user_func() and the likes)
--FILE--
<?php

class TestClass
{
	static function test()
	{
		echo __METHOD__ . "()\n";
	}

	function whee()
	{
		array_map(array('TestClass', 'test'), array('array_value'));
	}

	function whee4()
	{
		call_user_func(array('TestClass', 'test'));
	}

	static function whee5()
	{
		call_user_func(array('TestClass', 'test'));
	}
}

TestClass::test();

$a = new TestClass();
$a->whee();
$a->whee4();
$a->whee5();

TestClass::whee5();

?>
===DONE===
--EXPECT--
TestClass::test()
TestClass::test()
TestClass::test()
TestClass::test()
TestClass::test()
===DONE===
