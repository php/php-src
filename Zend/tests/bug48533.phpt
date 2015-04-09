--TEST--
Bug #48533 (__callStatic is not invoked for private/protected methods)
--FILE--
<?php

class foo {
	private function a() {
		var_dump(1);	
	}
	public function b() {
		var_dump(2);
	}
	protected function c() {
		var_dump(3);
	}
	static function __callstatic($a, $b) {
		var_dump('__callStatic::'. $a);
	}
	public function __call($a, $b) {
		var_dump('__call::'. $a);
	}
}

$x = new foo;
$x->a();
$x->b();
$x->c();
$x::a();
$x::b();
$x::c();

?>
--EXPECTF--
%unicode|string%(9) "__call::a"
int(2)
%unicode|string%(9) "__call::c"
%unicode|string%(15) "__callStatic::a"

Strict Standards: Non-static method foo::b() should not be called statically in %s on line %d
int(2)
%unicode|string%(15) "__callStatic::c"
