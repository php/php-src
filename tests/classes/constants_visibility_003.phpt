--TEST--
Class private constant visibility
--FILE--
<?php
class A {
	private const privateConst = 'privateConst';
	static function staticConstDump() {
		var_dump(self::privateConst);
	}
	function constDump() {
		var_dump(self::privateConst);
	}
}

A::staticConstDump();
(new A())->constDump();
constant('A::privateConst');

?>
--EXPECTF--
string(12) "privateConst"
string(12) "privateConst"

Warning: constant(): Couldn't find constant A::privateConst in %s on line %d
