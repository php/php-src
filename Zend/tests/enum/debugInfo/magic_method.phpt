--TEST--
Enum supports __debugInfo() magic method
--FILE--
<?php

enum Foo {
	case Bar;

	public function __debugInfo() {
		return [$this->name . ' is a case of the ' . __CLASS__ . ' enum'];
	}
}

var_dump(Foo::Bar);

?>
--EXPECTF--
object(Foo)#%d (1) {
  [0]=>
  string(29) "Bar is a case of the Foo enum"
}
