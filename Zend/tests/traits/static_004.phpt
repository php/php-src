--TEST--
Traits with __callStatic magic method.
--CREDITS--
Simas Toleikis simast@gmail.com
--FILE--
<?php

	trait TestTrait {
		public static function __callStatic($name, $arguments) {
			return $name;
		}
	}

	class A {
		use TestTrait;
	}

	echo A::Test();

?>
--EXPECT--
Test