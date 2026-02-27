--TEST--
Enum with __toString() magic method - param validation
--FILE--
<?php

enum Foo {
	case Bar;

	public function __toString(mixed $arg): string {
		return '';
	}
}

echo Foo::Bar;

?>
--EXPECTF--
Fatal error: Method Foo::__toString() cannot take arguments in %s on line %d
