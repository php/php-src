--TEST--
Enum with __debugInfo() magic method - backed enum value accessible
--FILE--
<?php

enum Foo: string {
	case Bar = "Baz";

	public function __debugInfo() {
		return [__CLASS__ . '::' . $this->name . ' = ' . $this->value];
	}
}

var_dump(Foo::Bar);

?>
--EXPECT--
enum(Foo::Bar) (1) {
  [0]=>
  string(14) "Foo::Bar = Baz"
}
