--TEST--
Testing callback formats within class method
--FILE--
<?php

class foo {
	public function test() {
		call_user_func(array('FOO', 'ABC'));
		call_user_func(array($this, 'ABC'));
		foo::XYZ();
		self::WWW();
		call_user_func('FOO::ABC');
	}
	function __call($a, $b) {
		print "__call:\n";
		var_dump($a);
	}
	static public function __callStatic($a, $b) {
		print "__callstatic:\n";
		var_dump($a);
	}
}

$x = new foo;

$x->test();

$x::A();

foo::B();

$f = 'FOO';

$f::C();

$f::$f();

foo::$f();

?>
--EXPECT--
__call:
unicode(3) "ABC"
__call:
unicode(3) "ABC"
__call:
unicode(3) "XYZ"
__call:
unicode(3) "WWW"
__call:
unicode(3) "ABC"
__callstatic:
unicode(1) "A"
__callstatic:
unicode(1) "B"
__callstatic:
unicode(1) "C"
__callstatic:
unicode(3) "FOO"
__callstatic:
unicode(3) "FOO"
