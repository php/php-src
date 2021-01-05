--TEST--
Test that the property initializer list can be empty
--FILE--
<?php

class Foo
{
    public function withProperties()
    {
        return clone $this with {};
    }
}

$foo = new Foo();
var_dump($foo);
$bar = $foo->withProperties();
var_dump($bar);

?>
--EXPECT--
object(Foo)#1 (0) {
}
object(Foo)#2 (0) {
}
