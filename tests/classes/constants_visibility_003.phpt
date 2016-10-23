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

Fatal error: Uncaught Error: Cannot access private const A::privateConst in %s:14
Stack trace:
#0 %s(14): constant('A::privateConst')
#1 {main}
  thrown in %s on line 14
