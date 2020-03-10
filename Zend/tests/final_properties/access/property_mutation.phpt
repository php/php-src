--TEST--
Test that final properties can't be mutated
--XFAIL--
Unsetting array items doesn't work yet
--FILE--
<?php

class Foo
{
    final public int $property1;
    final protected string $property2 = "Foo";
    final public array $property3;
    final public stdclass $property4;

    public function __construct()
    {
        $this->property1 = 1;
        $this->property4 = new stdclass();
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

$foo->property3[] = 1;

try {
    $foo->property3["foo"] = 1;
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    unset($foo->property3[0]);
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

$foo->property4->foo = "foo";

?>
--EXPECT--
Cannot modify final property Foo::$property1 after initialization
Cannot modify final property Foo::$property2 after initialization
Cannot modify final property Foo::$property3 after initialization
Cannot modify final property Foo::$property3 after initialization
