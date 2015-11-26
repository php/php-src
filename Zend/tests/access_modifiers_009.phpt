--TEST--
Inconsistencies when accessing protected members - 2
--XFAIL--
Discussion: http://marc.info/?l=php-internals&m=120221184420957&w=2
--FILE--
<?php

class A {
	static protected function f() {return 'A::f()';}
}
class B1 extends A {
	static protected function f() {return 'B1::f()';}
}
class B2 extends A {
	static public function test() {
		var_dump(is_callable('B1::f'));
		B1::f();
	}
}
B2::test();

?>
--EXPECTF--
bool(false)

Fatal error: Call to protected method B1::f() from context 'B2' in %s on line %d
