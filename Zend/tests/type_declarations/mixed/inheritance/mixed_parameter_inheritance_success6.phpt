--TEST--
Test that a parameter of a union of all built-in types can be overridden by the mixed type
--FILE--
<?php

class Foo
{
    public function method(bool|int|float|string|array|object|null $a) {}
}

class Bar extends Foo
{
    public function method(mixed $a) {}
}

?>
--EXPECT--
