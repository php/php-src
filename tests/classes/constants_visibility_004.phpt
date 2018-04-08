--TEST--
Only public and protected class constants should be inherited
--FILE--
<?php
class A {
	public const X = 1;
	protected const Y = 2;
	private const Z = 3;
}
class B extends A {
	static public function checkConstants() {
		var_dump(self::X);
		var_dump(self::Y);
		var_dump(self::Z);
	}
}

B::checkConstants();
?>
--EXPECTF--
int(1)
int(2)

Fatal error: Uncaught Error: Undefined class constant 'Z' in %s:11
Stack trace:
#0 %s(15): B::checkConstants()
#1 {main}
  thrown in %s on line 11
