--TEST--
Test that a mixed return value supports invariance
--FILE--
<?php

class Foo
{
    public function method(): mixed {}
}

class Bar extends Foo
{
    public function method(): mixed {}
}

?>
--EXPECT--
