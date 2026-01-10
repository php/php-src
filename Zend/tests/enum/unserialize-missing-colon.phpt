--TEST--
Enum unserialize with missing colon
--FILE--
<?php

enum Foo {
    case Bar;
}

var_dump(unserialize('E:6:"FooBar";'));

?>
--EXPECTF--
Warning: unserialize(): Invalid enum name 'FooBar' (missing colon) in %s on line %d

Warning: unserialize(): Error at offset 0 of 13 bytes in %s on line %d
bool(false)
