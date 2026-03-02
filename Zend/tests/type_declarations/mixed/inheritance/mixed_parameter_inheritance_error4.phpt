--TEST--
Test that a mixed parameter type can't be overridden by a union type of classes
--FILE--
<?php

class Foo
{
    public function method(mixed $a) {}
}

class Bar extends Foo
{
    public function method(stdClass|Foo $a) {}
}

?>
--EXPECTF--
Fatal error: Declaration of Bar::method(stdClass|Foo $a) must be compatible with Foo::method(mixed $a) in %s on line %d
