--TEST--
Enum with __debugInfo() magic method - visibility validation
--FILE--
<?php

enum Foo {
	case Bar;

	private function __debugInfo(): array {
		return [];
	}
}

var_dump(Foo::Bar);

?>
--EXPECTF--
Warning: The magic method Foo::__debugInfo() must have public visibility in %s on line %d
object(Foo)#%d (0) {
}
