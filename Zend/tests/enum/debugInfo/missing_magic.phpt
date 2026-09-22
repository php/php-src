--TEST--
When an enum does not have __debugInfo() it is printed nicely
--FILE--
<?php

enum Foo {
	case Bar;
}

var_dump(Foo::Bar);

?>
--EXPECT--
enum(Foo::Bar)
