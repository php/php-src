--TEST--
Enum with __debugInfo() magic method - return validation
--FILE--
<?php

enum Foo {
	case Bar;

	public function __debugInfo(): int {
		return 5;
	}
}

var_dump(Foo::Bar);

?>
--EXPECTF--
Fatal error: Foo::__debugInfo(): Return type must be ?array when declared in %s on line %d
