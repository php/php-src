--TEST--
Test that a parameter of a nullable int type can be overridden by the nullable number type
--FILE--
<?php

class Foo
{
    public function method(?int $a) {}
}

class Bar extends Foo
{
    public function method(?number $a) {}
}

?>
--EXPECT--
