--TEST--
Test that unsetting declared properties of immutable classes is not possible
--FILE--
<?php

immutable class Foo
{
    public static $property1 = "foo";
    public static string $property2;

    protected static $property3 = "foo";
    protected static string $property4 = "foo";

    public static function unsetProperty3(): void
    {
        unset(self::$property3);
    }

    public static function unsetProperty4(): void
    {
        unset(self::$property4);
    }
}

try {
    unset(Foo::$property1);
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    unset(Foo::$property2);
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    Foo::unsetProperty3();
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    Foo::unsetProperty4();
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECT--
Attempt to unset static property Foo::$property1
Attempt to unset static property Foo::$property2
Attempt to unset static property Foo::$property3
Attempt to unset static property Foo::$property4
