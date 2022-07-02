--TEST--
Enum __METHOD__
--FILE--
<?php

enum Foo {
    case Bar;

    public function printMethod()
    {
        echo __METHOD__ . "\n";
    }
}

Foo::Bar->printMethod();

?>
--EXPECT--
Foo::printMethod
