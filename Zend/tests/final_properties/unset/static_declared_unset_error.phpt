--TEST--
Test that unsetting declared static final properties is not possible
--FILE--
<?php

class Foo
{
    final public static string $property1 = "foo";
    final public static string $property2;

    final protected static string $property3;
    final protected static string $property4 = "foo";

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
