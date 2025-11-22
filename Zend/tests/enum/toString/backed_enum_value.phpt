--TEST--
Enum with __toString() magic method - backed enum value accessible
--FILE--
<?php

enum Foo: string {
	case Bar = "Baz";

	public function __toString() {
		return __CLASS__ . '::' . $this->name . ' = ' . $this->value;
	}
}

echo Foo::Bar;

?>
--EXPECT--
Foo::Bar = Baz
