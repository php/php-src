--TEST--
forward_static_call() calling outside of the inheritance chain.
--FILE--
<?php

class A
{
	const NAME = 'A';
	public static function test() {
		echo static::NAME, "\n";
	}
}

class B extends A
{
	const NAME = 'B';

	public static function test() {
		echo self::NAME, "\n";
		forward_static_call(array('parent', 'test'));
	}
}

class C
{
	const NAME = 'C';

	public static function test() {
		echo self::NAME, "\n";
		forward_static_call(array('B', 'test'));
	}
}

A::test();
echo "-\n";
B::test();
echo "-\n";
C::test();

?>
===DONE===
--EXPECT--
A
-
B
B
-
C
B
B
===DONE===
