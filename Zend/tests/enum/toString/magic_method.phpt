--TEST--
Enum supports __toString() magic method
--FILE--
<?php

enum Foo {
	case Bar;

	public function __toString() {
		return $this->name . ' is a case of the ' . __CLASS__ . ' enum';
	}
}

echo Foo::Bar;

?>
--EXPECT--
Bar is a case of the Foo enum
