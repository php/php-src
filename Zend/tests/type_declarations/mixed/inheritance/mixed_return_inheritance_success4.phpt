--TEST--
Test that a no return type can be overridden by the mixed type
--FILE--
<?php

class Foo
{
    public function method() {}
}

class Bar extends Foo
{
    public function method(): mixed {}
}

?>
--EXPECT--
