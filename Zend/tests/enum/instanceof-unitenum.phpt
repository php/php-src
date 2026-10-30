--TEST--
Auto implement UnitEnum interface
--EXTENSIONS--
zend_test
--FILE--
<?php

enum Foo {
    case Bar;
}

class Baz {}

var_dump(Foo::Bar instanceof UnitEnum);
var_dump((new Baz()) instanceof UnitEnum);
var_dump(ZendTestUnitEnum::Foo instanceof UnitEnum);

?>
--EXPECT--
bool(true)
bool(false)
bool(true)
