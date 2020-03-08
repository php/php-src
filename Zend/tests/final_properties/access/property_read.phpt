--TEST--
Test that final properties can be read
--FILE--
<?php

class Foo
{
    final public string $property1;
    final public string $property2 = "Bar";
    final public static string $property3;
    final public static string $property4 = "Baz";
}

$foo = new Foo();

try {
    var_dump($foo->property1);
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

$foo->property1 = "Foo";

var_dump($foo->property1);

var_dump($foo->property2);

try {
    var_dump(Foo::$property3);
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

Foo::$property3 = "Foo";

var_dump(Foo::$property3);
var_dump(Foo::$property4);

try {
    $var = &$foo->property2;
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECT--
Typed property Foo::$property1 must not be accessed before initialization
string(3) "Foo"
string(3) "Bar"
Typed static property Foo::$property3 must not be accessed before initialization
string(3) "Foo"
string(3) "Baz"
Cannot acquire reference on final property Foo::$property2
