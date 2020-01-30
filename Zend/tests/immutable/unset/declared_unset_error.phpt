--TEST--
Test that declared properties of immutable classes can't be unset
--FILE--
<?php

immutable class Foo
{
    public $property1 = "foo";
    public string $property2;

    protected $property3 = "foo";
    protected string $property4 = "foo";

    public function unsetProperty3(): void
    {
        unset($this->property3);
    }

    public function unsetProperty4(): void
    {
        unset($this->property4);
    }
}

$foo = new Foo();

try {
    unset($foo->property1);
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

// Uninitialized typed properties can't be unset anyway
unset($foo->property2);
unset($foo->property2);

try {
    $foo->unsetProperty3();
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    $foo->unsetProperty4();
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}


?>
--EXPECT--
Cannot unset a property of an immutable class Foo
Cannot unset a property of an immutable class Foo
Cannot unset a property of an immutable class Foo
