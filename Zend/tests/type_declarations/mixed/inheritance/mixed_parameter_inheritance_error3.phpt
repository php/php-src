--TEST--
Test that a mixed parameter type can't be overridden by a union of all built-in types
--FILE--
<?php

class Foo
{
    public function method(mixed $a) {}
}

class Bar extends Foo
{
    public function method(bool|int|float|string|array|object|null $a) {}
}

?>
--EXPECTF--
Fatal error: Declaration of Bar::method(object|array|string|int|float|bool|null $a) must be compatible with Foo::method(mixed $a) in %s on line %d
