--TEST--
Enum __CLASS__
--FILE--
<?php

enum Foo {
    case Bar;

    public function printClass()
    {
        echo __CLASS__ . "\n";
    }
}

Foo::Bar->printClass();

?>
--EXPECT--
Foo
