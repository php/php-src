--TEST--
Test that the __set() magic method works as expected for immutable classes
--XFAIL--
The restriction haven't been implemented yet
--FILE--
<?php

immutable class Foo
{
    private array $properties = [
        "property1" => "foo",
        "property2" => null,
        "property3" => [],
    ];

    public function __set($name, $value): void
    {
        $this->properties[$name] = $value;
    }
}

$foo = new Foo();

try {
    $foo->property1 = "foo";
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    $foo->property2 = "foo";
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    $foo->property3[] = "foo";
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
