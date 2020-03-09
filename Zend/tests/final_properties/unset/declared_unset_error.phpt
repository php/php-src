--TEST--
Test that declared final properties can't be unset
--FILE--
<?php

class Foo
{
    final public string $property1 = "foo";
    final public string $property2;
    final protected string $property3 = "foo";

    public function unsetProperty3(): void
    {
        unset($this->property3);
    }
}

$foo = new Foo();

try {
    unset($foo->property1);
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

unset($foo->property2);
unset($foo->property2);

try {
    $foo->unsetProperty3();
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECT--
Cannot unset final property Foo::$property1
Cannot unset final property Foo::$property3
