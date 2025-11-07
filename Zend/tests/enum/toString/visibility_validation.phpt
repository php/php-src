--TEST--
Enum with __toString() magic method - visibility validation
--FILE--
<?php

enum Foo {
	case Bar;

	private function __toString(): string {
		return '';
	}
}

echo Foo::Bar;

?>
--EXPECTF--
Warning: The magic method Foo::__toString() must have public visibility in %s on line %d

Fatal error: Access level to Foo::__toString() must be public (as in class Stringable) in %s on line %d
