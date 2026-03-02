--TEST--
Enum unserialize non-existent case
--FILE--
<?php

enum Foo {
    case Bar;
}

var_dump(unserialize('E:7:"Foo:Baz";'));

?>
--EXPECTF--
Warning: unserialize(): Undefined constant Foo::Baz in %s on line %d

Warning: unserialize(): Error at offset 14 of 14 bytes in %s on line %d
bool(false)
