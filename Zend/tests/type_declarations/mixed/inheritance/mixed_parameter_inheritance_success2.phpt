--TEST--
Test that a mixed parameter type can be overridden by no type
--FILE--
<?php

class Foo
{
    public function method(mixed $a) {}
}

class Bar extends Foo
{
    public function method($a) {}
}

?>
--EXPECT--
