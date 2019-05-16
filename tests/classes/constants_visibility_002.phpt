--TEST--
Class protected constant visibility
--FILE--
<?php
class A {
	protected const protectedConst = 'protectedConst';
	static function staticConstDump() {
		var_dump(self::protectedConst);
	}
	function constDump() {
		var_dump(self::protectedConst);
	}
}

A::staticConstDump();
(new A())->constDump();
constant('A::protectedConst');

?>
--EXPECTF--
string(14) "protectedConst"
string(14) "protectedConst"

Warning: constant(): Couldn't find constant A::protectedConst in %s on line %d
