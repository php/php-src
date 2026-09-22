--TEST--
Auto implement BackedEnum interface
--EXTENSIONS--
zend_test
--FILE--
<?php

enum Foo {
    case Bar;
}

enum Baz: int {
    case Qux = 0;
}

var_dump(Foo::Bar instanceof BackedEnum);
var_dump(Baz::Qux instanceof BackedEnum);
var_dump(ZendTestUnitEnum::Foo instanceof BackedEnum);
var_dump(ZendTestIntEnum::Foo instanceof BackedEnum);

?>
--EXPECT--
bool(false)
bool(true)
bool(false)
bool(true)
