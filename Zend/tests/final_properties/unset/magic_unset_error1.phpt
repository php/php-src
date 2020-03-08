--TEST--
Test that the __unset() magic method results in an exception
--FILE--
<?php

class Foo
{
    final private string $property1 = "Foo";

    public function __isset($name): bool
    {
        return isset($this->$name);
    }

    public function __get($name)
    {
        return $this->$name ?? null;
    }

    public function __set($name, $value): void
    {
        $this->$name = $value;
    }

    public function __unset($name): void
    {
        unset($this->$name);
    }
}

$foo = new Foo();

try {
    unset($foo->property1);
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

unset($foo->property2);

?>
--EXPECT--
Cannot unset final property Foo::$property1
