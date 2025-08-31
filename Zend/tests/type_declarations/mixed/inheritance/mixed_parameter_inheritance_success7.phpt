--TEST--
Test that a parameter of a union type of classes can be overridden by the mixed type
--FILE--
<?php

class Foo
{
    public function method(stdClass|Foo $a) {}
}

class Bar extends Foo
{
    public function method(mixed $a) {}
}

?>
--EXPECT--
