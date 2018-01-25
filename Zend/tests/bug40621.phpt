--TEST--
Bug #40621 (Crash when constructor called inappropriately (statically))
--FILE--
<?php

class Foo {
	private function __construct() { }
	function get() {
		self::__construct();
	}
}

Foo::get();

echo "Done\n";
?>
--EXPECTF--
Deprecated: Non-static method Foo::get() should not be called statically in %s on line %d

Fatal error: Uncaught Error: Non-static method Foo::__construct() cannot be called statically in %s:%d
Stack trace:
#0 %s(%d): Foo::get()
#1 {main}
  thrown in %s on line %d
