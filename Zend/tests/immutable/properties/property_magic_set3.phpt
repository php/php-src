--TEST--
Test that the __set() magic method works as expected for immutable classes
--FILE--
<?php

immutable class Foo
{
    private $property1 = "";
    private string $property2;
    private string $property3 = "";

    public function __set($name, $value): void
    {
        $this->$name = $value;
    }
}

$foo = new Foo();

try {
    $foo->property1 = "foo";
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

$foo->property2 = "foo";

try {
    $foo->property3 = "foo";
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    $foo->property4 = "foo";
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECT--
Cannot assign value to property Foo::$property1 of an immutable class more than once
Cannot assign value to property Foo::$property3 of an immutable class more than once
Cannot declare dynamic property Foo:$property4 of an immutable class
