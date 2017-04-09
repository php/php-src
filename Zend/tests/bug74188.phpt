--TEST--
Fixes bug 74188 (undeclared static variables emit a warning with ?? operator)
--FILE--
<?php
abstract class Test
{
	public static function get()
	{
		static::$a ?? true;
	}
}
Test::get();
?>
--EXPECT--
