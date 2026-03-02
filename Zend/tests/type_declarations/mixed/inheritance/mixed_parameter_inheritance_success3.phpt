--TEST--
Test that a parameter of no type can be overridden by the mixed type
--FILE--
<?php

class Foo
{
    public function method($a) {}
}

class Bar extends Foo
{
    public function method(mixed $a) {}
}

?>
--EXPECT--
