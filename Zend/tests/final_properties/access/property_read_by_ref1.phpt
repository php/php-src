--TEST--
Test that final properties of scalar types can't be accessed by reference
--XFAIL--
Incorrect error message in case of static properties ("Cannot modify final static property ...")
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

$var = null;

try {
    $var = &$foo->property1;
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    $var = &$foo->property2;
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    $var = &$foo->property3;
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    $var = &Foo::$property4;
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    $var = &Foo::$property5;
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    $var = &Foo::$property6;
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECT--
Cannot acquire reference to final property Foo::$property1
Cannot acquire reference to final property Foo::$property2
Cannot acquire reference to final property Foo::$property3
Cannot acquire reference to final property Foo::$property4
Cannot acquire reference to final property Foo::$property5
Cannot acquire reference to final property Foo::$property6
