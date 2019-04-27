--TEST--
Class Property Expressions
--FILE--
<?php
class Foo {
	const BAR = 1 << 0;
	const BAZ = 1 << 1;
	public $bar = self::BAR | self::BAZ;
}
echo (new Foo)->bar;
?>
--EXPECT--
3
