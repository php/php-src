--TEST--
Test that the __unset() magic method results in an exception
--XFAIL--
Unsetting array items is not prevented
--FILE--
<?php

class Foo
{
    final public array $properties = [
        "property1" => null,
    ];

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

unset($foo->property2);

try {
    unset($foo->properties);
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

var_dump($foo->properties);

?>
--EXPECT--
Cannot modify final property Foo::$properties after initialization
Cannot modify final property Foo::$properties after initialization
Cannot unset final property Foo::$properties
array(0) {
}
