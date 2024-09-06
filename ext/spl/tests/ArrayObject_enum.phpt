--TEST--
Enums are not compatible with ArrayObject
--FILE--
<?php

enum Foo {
    case Bar;
}

new ArrayObject(Foo::Bar);

?>
--EXPECTF--
Fatal error: Uncaught InvalidArgumentException: Enum Foo is not compatible with ArrayObject in %s:%d
%a
