--TEST--
Enum with __debugInfo() magic method - param validation
--FILE--
<?php

enum Foo {
	case Bar;

	public function __debugInfo(mixed $arg): array {
		return [];
	}
}

var_dump(Foo::Bar);

?>
--EXPECTF--
Fatal error: Method Foo::__debugInfo() cannot take arguments in %s on line %d
