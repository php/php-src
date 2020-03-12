--TEST--
Test that final static properties can't be mutated
--XFAIL--
Object properties are fetched with incorrect access mode
--FILE--
<?php

class Foo
{
    final public static int $property1;
    final public static string $property2 = "Foo";
    final public static array $property3;
    final public static stdClass $property4;

    public function __construct()
    {
        Foo::$property1 = 1;
        Foo::$property4 = new stdClass();
    }
}

$foo = new Foo();

try {
    Foo::$property1 = 2;
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    Foo::$property2 = "bar";
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

Foo::$property3[] = 1;

try {
    Foo::$property3[] = 1;
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    Foo::$property3["foo"] = 1;
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    unset(Foo::$property3[0]);
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    unset(Foo::$property3["foo"]);
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

var_dump(Foo::$property3);

Foo::$property4->foo = "foo";

?>
--EXPECT--
Cannot modify final static property Foo::$property1 after initialization
Cannot modify final static property Foo::$property2 after initialization
Cannot modify final static property Foo::$property3 after initialization
Cannot modify final static property Foo::$property3 after initialization
Cannot modify final static property Foo::$property3 after initialization
Cannot modify final static property Foo::$property3 after initialization
array(1) {
  [0]=>
  int(1)
}
