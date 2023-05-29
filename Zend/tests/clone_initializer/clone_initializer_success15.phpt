--TEST--
Test that "with" is a semi-reserved keyword
--FILE--
<?php

function with() {}

trait T {
    public function bar() {}
}

class Foo
{
    public const WITH = "with";

    public function with() {}
}

?>
--EXPECT--
