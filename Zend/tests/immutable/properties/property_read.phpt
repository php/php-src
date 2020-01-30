--TEST--
Test that properties of immutable classes can be read
--FILE--
<?php

immutable class Foo
{
    public $property1;
    public string $property2 = "Bar";
    public static $property3 = "Baz";
    public static int $property4;
}

$foo = new Foo();

var_dump($foo->property1);
var_dump($foo->property2);
var_dump(Foo::$property3);

$var = &$foo->property2;
var_dump($var);

?>
--EXPECT--
NULL
string(3) "Bar"
string(3) "Baz"
string(3) "Bar"
