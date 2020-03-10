--TEST--
Test that final properties of scalar types can be accessed for reading
--FILE--
<?php

class Foo
{
    final public bool $property1 = false;
    final public int $property2 = 2;
    final public string $property3 = "Bar";

    final public static bool $property4 = false;
    final public static int $property5 = 5;
    final public static string $property6 = "Baz";
}

$foo = new Foo();

var_dump($foo->property1);
var_dump($foo->property2);
var_dump($foo->property3);

var_dump(Foo::$property4);
var_dump(Foo::$property5);
var_dump(Foo::$property6);

?>
--EXPECT--
bool(false)
int(2)
string(3) "Bar"
bool(false)
int(5)
string(3) "Baz"
