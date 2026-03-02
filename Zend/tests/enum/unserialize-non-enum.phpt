--TEST--
Enum unserialize non-enum
--FILE--
<?php

class Foo {}

var_dump(unserialize('E:7:"Foo:Bar";'));

?>
--EXPECTF--
Warning: unserialize(): Class 'Foo' is not an enum in %s on line %d

Warning: unserialize(): Error at offset 0 of 14 bytes in %s on line %d
bool(false)
