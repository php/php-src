--TEST--
Test that final properties of composite type can't be accessed by reference
--XFAIL--
Incorrect error message in case of static properties ("Cannot modify final static property ...")
--FILE--
<?php

class Foo
{
    final public array $property1 = [];
    final public stdClass $property2;

    final public static array $property3 = [];
    final public static stdClass $property4;
}

$foo = new Foo();
$foo->property2 = new stdClass();
Foo::$property4 = new stdClass();

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
    $var = &Foo::$property3;
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    $var = &Foo::$property4;
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECT--
Cannot acquire reference to final property Foo::$property1
Cannot acquire reference to final property Foo::$property2
Cannot acquire reference to final property Foo::$property3
Cannot acquire reference to final property Foo::$property4
