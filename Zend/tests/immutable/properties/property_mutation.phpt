--TEST--
Test that declared properties of immutable classes can't be mutated
--FILE--
<?php

immutable class Foo
{
    public int $property1;
    protected $property2 = "Foo";
    protected string $property3 = "Bar";
    public array $property4;
    public stdclass $property5;

    public function __construct()
    {
        $this->property1 = 1;
        $this->property5 = new stdclass();
    }

    public function getProperty1(): int
    {
        return $this->property1;
    }

    public function withProperty1(int $property): Foo
    {
        $this->property1 = $property;

        return $this;
    }

    public function getProperty2(): string
    {
        return $this->property2;
    }

    public function withProperty2(string $property): Foo
    {
        $this->property2 = $property;

        return $this;
    }

    public function getProperty3(): string
    {
        return $this->property3;
    }

    public function withProperty3(string $property3): Foo
    {
        $this->property3 = $property3;

        return $this;
    }
}

$foo = new Foo();

try {
    $foo = $foo->withProperty1(1);
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    $foo = $foo->withProperty2("Bar");
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    $foo = $foo->withProperty3("Foo");
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

$foo->property4[] = 1;
$foo->property5->foo = "foo";

?>
--EXPECT--
Cannot assign value to property Foo::$property1 of an immutable class more than once
Cannot assign value to property Foo::$property2 of an immutable class more than once
Cannot assign value to property Foo::$property3 of an immutable class more than once
