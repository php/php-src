--TEST--
Test that a mixed parameter type can't be overridden by a built-in type
--FILE--
<?php

class Foo
{
    public function method(mixed $a) {}
}

class Bar extends Foo
{
    public function method(bool $a) {}
}

?>
--EXPECTF--
Fatal error: Declaration of Bar::method(bool $a) must be compatible with Foo::method(mixed $a) in %s on line %d
