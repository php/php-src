--TEST--
Test that final properties can't be passed by reference
--XFAIL--
Incorrect error message in case of static properties ("Cannot modify final static property ...")
--FILE--
<?php

class Foo
{
    final public array $property1;
    final public array $property2 = [1, 2, 3];
    final public static array $property3;
    final public static array $property4 = [1, 2, 3];
}

$foo = new Foo();

try {
    preg_match("/a/", "a", $foo->property1);
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    sort($foo->property2);
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    preg_match("/a/", "a", Foo::$property3);
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    sort(Foo::$property4);
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECT--
Cannot acquire reference to final property Foo::$property1
Cannot acquire reference to final property Foo::$property1
Cannot acquire reference to final property Foo::$property2
Cannot acquire reference to final property Foo::$property2
