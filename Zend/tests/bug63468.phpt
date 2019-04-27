--TEST--
Bug #63468 (wrong called method as callback with inheritance)
--FILE--
<?php
class Foo
{
	public function run()
	{
		return call_user_func(array('Bar', 'getValue'));
	}

	private static function getValue()
	{
		return 'Foo';
	}
}

class Bar extends Foo
{
	public static function getValue()
	{
		return 'Bar';
	}
}

$x = new Bar;
var_dump($x->run());
--EXPECT--
string(3) "Bar"
