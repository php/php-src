--TEST--
Test that immutable classes can't be modified after cloning
--FILE--
<?php

immutable class Foo
{
    public int $property1;
    public string $property2 = "";
    protected $property3;
    private $property4;
}

$foo = new Foo();
$foo->property1 = 1;
$bar = clone $foo;

try {
    $bar->property1 = 2;
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    $bar->property2 = "Baz";
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    $var = &$foo->property2;
    $var = "Bazinga";
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECT--
Cannot assign value to property Foo::$property1 of an immutable class more than once
Cannot assign value to property Foo::$property2 of an immutable class more than once
