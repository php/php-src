--TEST--
Enum __FUNCTION__
--FILE--
<?php

enum Foo {
    case Bar;

    public function printFunction()
    {
        echo __FUNCTION__ . "\n";
    }
}

Foo::Bar->printFunction();

?>
--EXPECT--
printFunction
