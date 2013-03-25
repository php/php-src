--TEST--
ZE2 Late Static Binding parent::/self:: forwarding and __callStatic
--FILE--
<?php
class A {
	static function test() {
		echo "A\n";
	}
	static function __callstatic($name, $args) {
		call_user_func("static::test");
	}
}
class B extends A {
	static function test() {
	  	echo "B\n";
	}
	static function __callstatic($name, $args) {
		parent::__callstatic($name, $args);
		call_user_func_array("parent::__callstatic", array($name, $args));
		parent::foo();
		call_user_func_array("parent::foo", $args);
		call_user_func_array(array("parent","foo"), $args);
	}
}
B::foo();
--EXPECT--
B
B
B
B
B
