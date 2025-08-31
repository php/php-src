--TEST--
Test that a parameter of a nullable built-in type can be overridden by the mixed type
--FILE--
<?php

class Foo
{
    public function method(?int $a) {}
}

class Bar extends Foo
{
    public function method(mixed $a) {}
}

?>
--EXPECT--
