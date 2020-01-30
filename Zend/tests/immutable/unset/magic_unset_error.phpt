--TEST--
Test that the __unset() magic method results in an exception
--FILE--
<?php

immutable class Foo
{
    private array $properties = [];

    public function __isset($name): bool
    {
        return isset($this->properties[$name]);
    }

    public function __get($name)
    {
        return $this->properties[$name] ?? null;
    }

    public function __set($name, $value): void
    {
        $this->properties[$name] = $value;
    }

    public function __unset($name): void
    {
        unset($this->properties[$name]);
    }
}

$foo = new Foo();

try {
    unset($foo->property1);
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECT--
Cannot unset a property of an immutable class Foo
