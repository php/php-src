--TEST--
Test that a mixed return type can't be overridden by no return type
--FILE--
<?php

class Foo
{
    public function method(): mixed {}
}

class Bar extends Foo
{
    public function method() {}
}

?>
--EXPECTF--
Fatal error: Declaration of Bar::method() must be compatible with Foo::method(): mixed in %s on line %d
