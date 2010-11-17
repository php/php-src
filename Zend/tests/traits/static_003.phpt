--TEST--
Traits with late static bindings.
--CREDITS--
Simas Toleikis simast@gmail.com
--FILE--
<?php

	trait TestTrait {
		public static function test() {
			return static::$test;
		}
	}

	class A {
		use TestTrait;
		protected static $test = "Test A";
	}

	class B extends A {
		protected static $test = "Test B";
	}

	echo B::test();

?>
--EXPECT--
Test B