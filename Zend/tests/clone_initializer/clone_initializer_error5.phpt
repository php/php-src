--TEST--
Test TypeError when "cloning with" non-readonly property
--FILE--
<?php

class Foo
{
    public int $bar = 1;
}

$obj1 = new Foo();

try {
    clone $obj1 with {bar: []};
} catch (TypeError $e) {
    echo $e->getMessage() . "\n";
}

try {
    clone $obj1 with {bar: []}; // The same as above but now using cache slots
} catch (TypeError $e) {
    echo $e->getMessage() . "\n";
}

var_dump($obj1->bar);

?>
--EXPECT--
Cannot assign array to property Foo::$bar of type int
Cannot assign array to property Foo::$bar of type int
int(1)
