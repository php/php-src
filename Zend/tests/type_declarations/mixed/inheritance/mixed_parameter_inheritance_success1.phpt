--TEST--
Test that a mixed parameter type supports invariance
--FILE--
<?php

class Foo
{
    public function method(mixed $a) {}
}

class Bar extends Foo
{
    public function method(mixed $a) {}
}

?>
--EXPECT--
