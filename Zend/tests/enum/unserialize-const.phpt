--TEST--
Enum unserialize const
--FILE--
<?php

enum Foo {
    case Bar;
    const Baz = Foo::Bar;
}

var_dump(unserialize('E:7:"Foo:Baz";'));

?>
--EXPECTF--
Warning: unserialize(): Foo::Baz is not an enum case in %s on line %d

Notice: unserialize(): Error at offset 14 of 14 bytes in %s on line %d
bool(false)
