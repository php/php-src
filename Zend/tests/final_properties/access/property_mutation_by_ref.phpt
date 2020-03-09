--TEST--
Test that declared final properties can't be mutated by reference
--FILE--
<?php

class Foo
{
    final public int $property1;
    final public int $property2 = 2;

    public function __construct()
    {
    }
}

$foo = new Foo();

$x = 1;
try {
    $foo->property1 = &$x;
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

$foo->property1 = 1;

try {
    $foo->property1 = &$x;
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    $foo->property2 = &$foo->property1;
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECT--
Cannot modify final property Foo::$property1 by reference after initialization
Cannot modify final property Foo::$property1 by reference after initialization
Cannot acquire reference on final property Foo::$property1
