--TEST--
Test that a number parameter type supports invariance
--FILE--
<?php

class Foo
{
    public function method(number $a) {}
}

class Bar extends Foo
{
    public function method(int|float $a) {}
}

class Baz extends Foo
{
    public function method(number $a) {}
}

?>
--EXPECT--
