--TEST--
Test that a number return value supports invariance
--FILE--
<?php

class Foo
{
    public function method(): number {}
}

class Bar extends Foo
{
    public function method(): int|float {}
}

class Baz extends Bar
{
    public function method(): number {}
}

?>
--EXPECT--
