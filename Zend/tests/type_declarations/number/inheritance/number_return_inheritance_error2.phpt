--TEST--
Test that a number return type can't be widened
--FILE--
<?php

class Foo
{
    public function method(): number {}
}

class Bar extends Foo
{
    public function method(): ?number {}
}

?>
--EXPECTF--
Fatal error: Declaration of Bar::method(): int|float|null must be compatible with Foo::method(): number in %s on line %d
