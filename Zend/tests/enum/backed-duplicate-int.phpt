--TEST--
Backed enums reject duplicate int values
--FILE--
<?php

enum Foo: int {
    case Bar = 0;
    case Baz = 0;
}

?>
--EXPECTF--
Fatal error: Duplicate value in enum Foo for cases Bar and Baz in %s on line %s
