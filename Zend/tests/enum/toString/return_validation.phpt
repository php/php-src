--TEST--
Enum with __toString() magic method - return validation
--FILE--
<?php

enum Foo {
	case Bar;

	public function __toString(): int {
		return 5;
	}
}

echo Foo::Bar;

?>
--EXPECTF--
Fatal error: Foo::__toString(): Return type must be string when declared in %s on line %d
