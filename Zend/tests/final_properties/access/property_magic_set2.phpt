--TEST--
Test that the __set() magic method works as expected for final properties
--FILE--
<?php

class Foo
{
    final private string $property1 = "";
    final private string $property2;

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
    $foo->property2 = "foo";
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}


?>
--EXPECT--
Cannot modify final property Foo::$property1 after initialization
Cannot modify final property Foo::$property2 after initialization
