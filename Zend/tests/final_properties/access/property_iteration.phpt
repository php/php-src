--TEST--
Test that final array properties can be iterated
--FILE--
<?php

class Foo
{
    final public array $property1 = ["foo", "bar", "baz"];
}

$foo = new Foo();

foreach ($foo->property1 as $key => $value) {
    var_dump("$key => $value");
}

?>
--EXPECT--
string(8) "0 => foo"
string(8) "1 => bar"
string(8) "2 => baz"
