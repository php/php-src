--TEST--
Test that the void return type can't be overridden by the mixed type
--FILE--
<?php

class Foo
{
    public function method(): void {}
}

class Bar extends Foo
{
    public function method(): mixed {}
}

?>
--EXPECTF--
Fatal error: Declaration of Bar::method(): mixed must be compatible with Foo::method(): void in %s on line %d
