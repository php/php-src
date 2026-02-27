--TEST--
Enum with __toString() magic method - Stringable interface added
--FILE--
<?php

enum Foo {
	case Bar;

	public function __toString() {
		return $this->name . ' is a case of the ' . __CLASS__ . ' enum';
	}
}

function printStringable(Stringable $s) {
	echo $s;
}

var_dump(class_implements(Foo::class));
printStringable(Foo::Bar);

?>
--EXPECT--
array(2) {
  ["UnitEnum"]=>
  string(8) "UnitEnum"
  ["Stringable"]=>
  string(10) "Stringable"
}
Bar is a case of the Foo enum
