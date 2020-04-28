--TEST--
Test that a narrower type can be overridden by the number type
--FILE--
<?php

class Foo
{
    public function method(int $a) {}
}

class Bar extends Foo
{
    public function method(number $a) {}
}

?>
--EXPECT--
