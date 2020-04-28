--TEST--
Test that a number parameter type can't be narrowed
--FILE--
<?php

class Foo
{
    public function method(number $a) {}
}

class Bar extends Foo
{
    public function method(int $a) {}
}

?>
--EXPECTF--
Fatal error: Declaration of Bar::method(int $a) must be compatible with Foo::method(number $a) in %s on line %d
