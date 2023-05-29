--TEST--
Test that "clone with" works with expressions as clone operand
--FILE--
<?php

class Foo
{
    public static function create()
    {
        return new Foo();
    }
}

$foo = clone Foo::create() with [];
var_dump($foo);

?>
--EXPECT--
object(Foo)#2 (0) {
}
