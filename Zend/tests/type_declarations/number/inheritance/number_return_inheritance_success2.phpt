--TEST--
Test that a number return type can be overridden by any subtype
--FILE--
<?php

class Foo
{
    public function method(): number {}
}

class Bar extends Foo
{
    public function method(): int {}
}

class Baz extends Foo
{
    public function method(): float {}
}

?>
--EXPECT--
