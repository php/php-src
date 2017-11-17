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

Fatal error: Uncaught Error: Cannot access protected const A::protectedConst in %s:14
Stack trace:
#0 %s(14): constant('A::protectedCon...')
#1 {main}
  thrown in %s on line 14
