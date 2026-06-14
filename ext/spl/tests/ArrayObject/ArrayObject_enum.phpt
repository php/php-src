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
Deprecated: ArrayObject::__construct(): Using an object as a backing array for ArrayObject is deprecated, as it allows violating class constraints and invariants in %s on line %d

Fatal error: Uncaught InvalidArgumentException: Enums are not compatible with ArrayObject in %s:%d
%a
