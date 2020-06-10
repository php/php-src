--TEST--
Auto implement UnitEnum interface
--FILE--
<?php

enum Foo {
    case Bar;
}

class Baz {}

var_dump(Foo::Bar instanceof UnitEnum);
var_dump((new Baz()) instanceof UnitEnum);

?>
--EXPECT--
bool(true)
bool(false)
